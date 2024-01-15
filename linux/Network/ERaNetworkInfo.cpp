#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <linux/wireless.h>
#include <ifaddrs.h>
#include <Network/ERaNetworkInfo.hpp>

static int16_t erssi {100};
static bool iswireless {false};
static char essid[IW_ESSID_MAX_SIZE + 1] {
    'S', 'o', 'c', 'k', 'e', 't', '\0'
};
static char ifrName[IFNAMSIZ] {
    'w', 'l', 'a', 'n', '0', '\0'
};

void GetNetworkInfo(void) {
    int fd = -1;
    struct iwreq wrq;
    struct ifaddrs* ifaddr;
    struct ifaddrs* ifa;
    struct iw_statistics iwstats;

    if (getifaddrs(&ifaddr) < 0) {
        perror("Error getting interface addresses");
        return;
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL) {
            continue;
        }
        if (ifa->ifa_addr->sa_family != AF_INET) {
            continue;
        }
        if (ifa->ifa_flags & IFF_LOOPBACK) {
            continue;
        }
        if (!(ifa->ifa_flags & IFF_RUNNING)) {
            continue;
        }

        strncpy(wrq.ifr_name, ifa->ifa_name, IFNAMSIZ);
        wrq.ifr_name[IFNAMSIZ - 1] = '\0';

        fd = socket(AF_INET, SOCK_DGRAM, 0);
        if (fd < 0) {
            perror("Error opening INET socket");
            continue;
        }

        if (ioctl(fd, SIOCGIWNAME, &wrq) == 0) {
            iswireless = true;
            printf("IF Wireless: %s\r\n", wrq.ifr_name);
        }
        else {
            iswireless = false;
            printf("IF: %s\r\n", wrq.ifr_name);
        }
        strcpy(ifrName, wrq.ifr_name);
        break;
    }

    freeifaddrs(ifaddr);

    if (fd < 0) {
        return;
    }

    if (!iswireless) {
        close(fd);
        return;
    }

    wrq.u.essid.pointer = essid;
    wrq.u.essid.length = IW_ESSID_MAX_SIZE;
    if (ioctl(fd, SIOCGIWESSID, &wrq) < 0) {
        perror("Can't open socket to obtain essid");
        close(fd);
        return;
    }

    uint16_t length = wrq.u.essid.length;
    if (length > IW_ESSID_MAX_SIZE) {
        length = IW_ESSID_MAX_SIZE;
    }
    essid[length] = '\0';
    printf("ESSID is: %s\r\n", wrq.u.essid.pointer);

    memset(&iwstats, 0, sizeof(iwstats));

    wrq.u.data.pointer = &iwstats;
    wrq.u.data.length = sizeof(struct iw_statistics);
    wrq.u.data.flags = 1;

    if (ioctl(fd, SIOCGIWSTATS, &wrq) < 0) {
        perror("Can't open socket to obtain iwstats");
        close(fd);
        return;
    }

    erssi = (iwstats.qual.level - 100);
    printf("Signal: %d\r\n", erssi);
    close(fd);
}

const char* GetLocalIP(const char* interface) {
    if (interface == NULL) {
        interface = ifrName;
    }

    int fd = -1;
    struct ifreq ifr;
    static char localIP[32] {0};

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        return localIP;
    }

    /* Get an IPv4 IP address */
    ifr.ifr_addr.sa_family = AF_INET;

    /* IP address attached to ifr: "wlan0", "eth0" */
    strncpy(ifr.ifr_name, interface, IFNAMSIZ - 1);

    ioctl(fd, SIOCGIFADDR, &ifr);

    close(fd);

    memset(localIP, 0, sizeof(localIP));
    snprintf(localIP, sizeof(localIP), "%s", inet_ntoa(((struct sockaddr_in*)&ifr.ifr_addr)->sin_addr));

    printf("%s IP Address is: %s\r\n", interface, localIP);

    return localIP;
}

const char* GetMACAddress(const char* interface) {
    if (interface == NULL) {
        interface = ifrName;
    }

    int fd = -1;
    struct ifreq ifr;
    static char macAddr[32] {0};

    fd = socket(PF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        return macAddr;
    }

    /* Get an IPv4 IP address */
    ifr.ifr_addr.sa_family = AF_INET;

    /* IP address attached to ifr: "wlan0", "eth0" */
    strncpy(ifr.ifr_name, interface, IFNAMSIZ - 1);

    ioctl(fd, SIOCGIFHWADDR, &ifr);

    close(fd);

    memset(macAddr, 0, sizeof(macAddr));
    snprintf(macAddr, sizeof(macAddr), "%02X:%02X:%02X:%02X:%02X:%02X",
                                       (uint8_t)ifr.ifr_hwaddr.sa_data[0],
                                       (uint8_t)ifr.ifr_hwaddr.sa_data[1],
                                       (uint8_t)ifr.ifr_hwaddr.sa_data[2],
                                       (uint8_t)ifr.ifr_hwaddr.sa_data[3],
                                       (uint8_t)ifr.ifr_hwaddr.sa_data[4],
                                       (uint8_t)ifr.ifr_hwaddr.sa_data[5]);

    printf("%s MAC Address is: %s\r\n", interface, macAddr);

    return macAddr;
}

const char* GetNetworkProtocol(void) {
    return ifrName;
}

const char* GetSSIDNetwork(void) {
    if (!iswireless) {
        return "Ethernet";
    }
    return essid;
}

int16_t GetRSSINetwork(void) {
    if (!iswireless) {
        return 100;
    }
    return erssi;
}
