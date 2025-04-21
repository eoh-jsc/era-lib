#include <arpa/inet.h>
#include <errno.h>
#include <memory.h>
#include <netdb.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

#include "posix_tls.hpp"

static void net_prepare(void) {
  signal(SIGPIPE, SIG_IGN);
}

static int lwmqtt_mbedtls_net_connect(mbedtls_net_context *ctx, const char *host, const char *port, int proto) {
  int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
  struct addrinfo hints, *addr_list, *cur;

  // prepare network
  net_prepare();

  // do name resolution with both IPv6 and IPv4
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = proto == MBEDTLS_NET_PROTO_UDP ? SOCK_DGRAM : SOCK_STREAM;
  hints.ai_protocol = proto == MBEDTLS_NET_PROTO_UDP ? IPPROTO_UDP : IPPROTO_TCP;

  if (getaddrinfo(host, port, &hints, &addr_list) != 0) {
    return MBEDTLS_ERR_NET_UNKNOWN_HOST;
  }

  // try the sockaddrs until a connection succeeds
  ret = MBEDTLS_ERR_NET_UNKNOWN_HOST;
  for (cur = addr_list; cur != NULL; cur = cur->ai_next) {
    ctx->fd = (int)socket(cur->ai_family, cur->ai_socktype, cur->ai_protocol);
    if (ctx->fd < 0) {
      ret = MBEDTLS_ERR_NET_SOCKET_FAILED;
      continue;
    }

    int rc = fcntl(ctx->fd, F_SETFL, fcntl(ctx->fd, F_GETFL, 0) | O_NONBLOCK);
    if (rc < 0) {
    }
    else {
      rc = connect(ctx->fd, cur->ai_addr, cur->ai_addrlen);
    }
    if (rc == 0 || errno == EINPROGRESS) {
      ret = 0;
      break;
    }

    mbedtls_net_close(ctx);
    ret = MBEDTLS_ERR_NET_CONNECT_FAILED;
  }

  freeaddrinfo(addr_list);

  return ret;
}

void lwmqtt_posix_tls_network_init(lwmqtt_posix_tls_network_t *network, bool verify, const uint8_t *ca_buf, size_t ca_len) {
  if (!network) {
    return;
  }

  network->verify = verify;
  network->ca_buf = (uint8_t*)ca_buf;
  network->ca_len = ca_len;
}

lwmqtt_err_t lwmqtt_posix_tls_network_connect(lwmqtt_posix_tls_network_t *network, char *host, int port) {
  // close any open socket
  lwmqtt_posix_tls_network_disconnect(network);

  static char port_string[10] {0};
  memset(port_string, 0, sizeof(port_string));
  snprintf(port_string, sizeof(port_string), "%d", port);

  // initialize support structures
  mbedtls_net_init(&network->socket);
  mbedtls_ssl_init(&network->ssl);
  mbedtls_ssl_config_init(&network->conf);
  mbedtls_x509_crt_init(&network->cacert);
  mbedtls_ctr_drbg_init(&network->ctr_drbg);
  mbedtls_entropy_init(&network->entropy);

  // setup entropy source
  int ret = mbedtls_ctr_drbg_seed(&network->ctr_drbg, mbedtls_entropy_func, &network->entropy, NULL, 0);
  if (ret != 0) {
    return LWMQTT_NETWORK_FAILED_CONNECT;
  }

  // parse ca certificate
  if (network->ca_buf) {
    ret = mbedtls_x509_crt_parse(&network->cacert, network->ca_buf, network->ca_len);
    if (ret != 0) {
      return LWMQTT_NETWORK_FAILED_CONNECT;
    }
  }

  // connect socket
  ret = lwmqtt_mbedtls_net_connect(&network->socket, host, port_string, MBEDTLS_NET_PROTO_TCP);
  if (ret != 0) {
    return LWMQTT_NETWORK_FAILED_CONNECT;
  }

  // load defaults
  ret = mbedtls_ssl_config_defaults(&network->conf, MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM,
                                    MBEDTLS_SSL_PRESET_DEFAULT);
  if (ret != 0) {
    return LWMQTT_NETWORK_FAILED_CONNECT;
  }

  // set ca certificate
  if (network->ca_buf) {
    mbedtls_ssl_conf_ca_chain(&network->conf, &network->cacert, NULL);
  }

  // set auth mode
  mbedtls_ssl_conf_authmode(&network->conf, (network->verify) ? MBEDTLS_SSL_VERIFY_REQUIRED : MBEDTLS_SSL_VERIFY_NONE);

  // set rng callback
  mbedtls_ssl_conf_rng(&network->conf, mbedtls_ctr_drbg_random, &network->ctr_drbg);

  // setup ssl context
  ret = mbedtls_ssl_setup(&network->ssl, &network->conf);
  if (ret != 0) {
    return LWMQTT_NETWORK_FAILED_CONNECT;
  }

  // set hostname
  ret = mbedtls_ssl_set_hostname(&network->ssl, host);
  if (ret != 0) {
    return LWMQTT_NETWORK_FAILED_CONNECT;
  }

  // set bio callbacks
  mbedtls_ssl_set_bio(&network->ssl, &network->socket, mbedtls_net_send, mbedtls_net_recv, NULL);

  // verify certificate if requested
  if (network->verify) {
    uint32_t flags = mbedtls_ssl_get_verify_result(&network->ssl);
    if (flags != 0) {
      char verify_buf[100] = {0};
      mbedtls_x509_crt_verify_info(verify_buf, sizeof(verify_buf), "  ! ", flags);
    }
  }

  // perform handshake
  ret = mbedtls_ssl_handshake(&network->ssl);
  if (ret != 0 && ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
    return LWMQTT_NETWORK_FAILED_CONNECT;
  }

  return LWMQTT_SUCCESS;
}

lwmqtt_err_t lwmqtt_posix_tls_network_wait(lwmqtt_posix_tls_network_t *network, bool *connected, uint32_t timeout) {
  // prepare sets
  fd_set set;
  fd_set ex_set;
  FD_ZERO(&set);
  FD_ZERO(&ex_set);
  FD_SET(network->socket.fd, &set);
  FD_SET(network->socket.fd, &ex_set);

  // wait for data
  struct timeval t = {.tv_sec = timeout / 1000, .tv_usec = (timeout % 1000) * 1000};
  int result = select(network->socket.fd + 1, NULL, &set, &ex_set, &t);
  if (result < 0 || FD_ISSET(network->socket.fd, &ex_set)) {
    return LWMQTT_NETWORK_FAILED_CONNECT;
  }

  // set whether socket is connected
  *connected = result > 0;

  // set socket to blocking
  int ret = mbedtls_net_set_block(&network->socket);
  if (ret < 0) {
    return LWMQTT_NETWORK_FAILED_CONNECT;
  }

  return LWMQTT_SUCCESS;
}

void lwmqtt_posix_tls_network_disconnect(lwmqtt_posix_tls_network_t *network) {
  // check if network is available
  if (!network) {
    return;
  }
  if (network->socket.fd < 0) {
    return;
  }

  // cleanup resources
  int ret;
  do {
    ret = mbedtls_ssl_close_notify(&network->ssl);
  } while (ret == MBEDTLS_ERR_SSL_WANT_WRITE);

  mbedtls_x509_crt_free(&network->cacert);
  mbedtls_entropy_free(&network->entropy);
  mbedtls_ssl_config_free(&network->conf);
  mbedtls_ctr_drbg_free(&network->ctr_drbg);
  mbedtls_ssl_free(&network->ssl);
  mbedtls_net_free(&network->socket);

  network->socket.fd = -1;
}

lwmqtt_err_t lwmqtt_posix_tls_network_peek(lwmqtt_posix_tls_network_t *network, size_t *available, uint32_t timeout) {
  // set timeout
  struct timeval t = {.tv_sec = timeout / 1000, .tv_usec = (timeout % 1000) * 1000};
  int rc = setsockopt(network->socket.fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&t, sizeof(t));
  if (rc < 0) {
    return LWMQTT_NETWORK_FAILED_READ;
  }

  // set socket to non blocking
  int ret = mbedtls_net_set_nonblock(&network->socket);
  if (ret != 0) {
    return LWMQTT_NETWORK_FAILED_READ;
  }

  // TODO: Directly peek on underlying socket?

  // check if socket is valid
  ret = mbedtls_ssl_read(&network->ssl, NULL, 0);
  if (ret < 0 && ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
    return LWMQTT_NETWORK_FAILED_READ;
  }

  // set available bytes
  *available = mbedtls_ssl_get_bytes_avail(&network->ssl);

  // set socket back to blocking
  ret = mbedtls_net_set_block(&network->socket);
  if (ret != 0) {
    return LWMQTT_NETWORK_FAILED_READ;
  }

  return LWMQTT_SUCCESS;
}

lwmqtt_err_t lwmqtt_posix_tls_network_select(lwmqtt_posix_tls_network_t *network, bool *available, uint32_t timeout) {
  // prepare set
  fd_set set;
  fd_set ex_set;
  FD_ZERO(&set);
  FD_ZERO(&ex_set);
  FD_SET(network->socket.fd, &set);
  FD_SET(network->socket.fd, &ex_set);

  // wait for data
  struct timeval t = {.tv_sec = timeout / 1000, .tv_usec = (timeout % 1000) * 1000};
  int result = select(network->socket.fd + 1, &set, NULL, &ex_set, &t);
  if (result < 0 || FD_ISSET(network->socket.fd, &ex_set)) {
    return LWMQTT_NETWORK_FAILED_READ;
  }

  // set whether data is available
  *available = result > 0;

  return LWMQTT_SUCCESS;
}

lwmqtt_err_t lwmqtt_posix_tls_network_read(void *ref, uint8_t *buffer, size_t len, size_t *received, uint32_t timeout) {
  // cast network reference
  lwmqtt_posix_tls_network_t *n = (lwmqtt_posix_tls_network_t *)ref;

  // set timeout
  struct timeval t = {.tv_sec = timeout / 1000, .tv_usec = (timeout % 1000) * 1000};
  int rc = setsockopt(n->socket.fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&t, sizeof(t));
  if (rc < 0) {
    return LWMQTT_NETWORK_FAILED_READ;
  }

  // read from socket
  int ret = mbedtls_ssl_read(&n->ssl, buffer, len);
  if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
    return LWMQTT_SUCCESS;
  } else if (ret <= 0) {
    return LWMQTT_NETWORK_FAILED_READ;
  }

  // increment counter
  *received += ret;

  return LWMQTT_SUCCESS;
}

lwmqtt_err_t lwmqtt_posix_tls_network_write(void *ref, uint8_t *buffer, size_t len, size_t *sent, uint32_t timeout) {
  // cast network reference
  lwmqtt_posix_tls_network_t *n = (lwmqtt_posix_tls_network_t *)ref;

  // set timeout
  struct timeval t = {.tv_sec = timeout / 1000, .tv_usec = (timeout % 1000) * 1000};
  int rc = setsockopt(n->socket.fd, SOL_SOCKET, SO_SNDTIMEO, (char *)&t, sizeof(t));
  if (rc < 0) {
    return LWMQTT_NETWORK_FAILED_WRITE;
  }

  // write to socket
  int ret = mbedtls_ssl_write(&n->ssl, buffer, len);
  if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
    return LWMQTT_SUCCESS;
  } else if (ret < 0) {
    return LWMQTT_NETWORK_FAILED_WRITE;
  }

  // increment counter
  *sent += ret;

  return LWMQTT_SUCCESS;
}
