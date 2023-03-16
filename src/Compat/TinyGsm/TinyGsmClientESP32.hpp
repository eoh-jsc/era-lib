/**
 * @file       TinyGsmClientESP32.hpp
 * TinyGsm for ESP32 base TinyGsmClientESP8266
 */

#ifndef INC_TINYGSM_CLIENT_ESP32_HPP_
#define INC_TINYGSM_CLIENT_ESP32_HPP_

// #pragma message("TinyGSM:  TinyGsmClientESP32")

// #define TINY_GSM_DEBUG Serial

// #define TINY_GSM_GET_AVAILABLE

#ifndef TINY_GSM_MAX_SEND_RETRY
    #define TINY_GSM_MAX_SEND_RETRY 5
#endif

#define TINY_GSM_MUX_COUNT          5
#define TINY_GSM_BUFFER_READ_AND_CHECK_SIZE

#include <TinyGsmModem.tpp>
#include <TinyGsmSSL.tpp>
#include <TinyGsmTCP.tpp>
#include <TinyGsmWifi.tpp>

#define GSM_NL      "\r\n"
static const char   GSM_OK[] TINY_GSM_PROGMEM    = "OK" GSM_NL;
static const char   GSM_ERROR[] TINY_GSM_PROGMEM = "ERROR" GSM_NL;
static uint8_t      TINY_GSM_TCP_KEEP_ALIVE      = 120;

// <stat> status of ESP32 station interface
// 2 : ESP32 station connected to an AP and has obtained IP
// 3 : ESP32 station created a TCP or UDP transmission
// 4 : the TCP or UDP transmission of ESP32 station disconnected
// 5 : ESP32 station did NOT connect to an AP
enum RegStatus {
	REG_OK_IP     = 2,
	REG_OK_TCP    = 3,
	REG_OK_NO_TCP = 4,
	REG_DENIED    = 5,
	REG_UNKNOWN   = 6,
};

class TinyGsmESP32
	: public TinyGsmModem<TinyGsmESP32>
	, public TinyGsmWifi<TinyGsmESP32>
	, public TinyGsmTCP<TinyGsmESP32, TINY_GSM_MUX_COUNT>
	, public TinyGsmSSL<TinyGsmESP32>
{
	friend class TinyGsmModem<TinyGsmESP32>;
	friend class TinyGsmWifi<TinyGsmESP32>;
	friend class TinyGsmTCP<TinyGsmESP32, TINY_GSM_MUX_COUNT>;
	friend class TinyGsmSSL<TinyGsmESP32>;

	/*
	* Inner Client
	*/
public:
	class GsmClientESP32
		: public GsmClient
	{
		friend class TinyGsmESP32;

	public:
		GsmClientESP32()
        {}

		explicit GsmClientESP32(TinyGsmESP32& modem, uint8_t muxNo = 0)
        {
			this->init(&modem, muxNo);
		}

		bool init(TinyGsmESP32* modem, uint8_t muxNo = 0) {
			this->at             = modem;
			this->sock_available = 0;
			this->prev_check     = 0;
			this->sock_connected = false;
			this->got_data       = false;

			if (muxNo < TINY_GSM_MUX_COUNT) {
				this->mux = muxNo;
			}
            else {
				this->mux = (muxNo % TINY_GSM_MUX_COUNT);
			}
			this->at->sockets[this->mux] = this;

			return true;
		}

	public:
		virtual int connect(const char* host, uint16_t port, int timeout_s) {
			this->stop();
			TINY_GSM_YIELD();
			this->rx.clear();
			this->sock_connected = this->at->modemConnect(host, port, this->mux, false, timeout_s);
			return this->sock_connected;
		}
		TINY_GSM_CLIENT_CONNECT_OVERRIDES

		void stop(uint32_t maxWaitMs) {
			TINY_GSM_YIELD();
			this->at->sendAT(GF("+CIPCLOSE="), this->mux);
			this->sock_connected = false;
			this->at->waitResponse(maxWaitMs);
			this->rx.clear();
		}
		void stop() override {
			this->stop(5000L);
		}

		/*
		 * Extended API
		 */

		String remoteIP() TINY_GSM_ATTR_NOT_IMPLEMENTED;
	};

	/*
	 * Inner Secure Client
	 */
public:
	class GsmClientSecureESP32
		: public GsmClientESP32
	{
	public:
		GsmClientSecureESP32()
        {}

		explicit GsmClientSecureESP32(TinyGsmESP32& modem, uint8_t muxNo = 0)
				: GsmClientESP32(modem, muxNo)
        {}

	public:
		int connect(const char* host, uint16_t port, int timeout_s) override {
			this->stop();
			TINY_GSM_YIELD();
			this->rx.clear();
			this->sock_connected = this->at->modemConnect(host, port, this->mux, true, timeout_s);
			return this->sock_connected;
		}
		TINY_GSM_CLIENT_CONNECT_OVERRIDES
	};

	/*
	* Constructor
	*/
public:
	explicit TinyGsmESP32(Stream& stream)
        : stream(stream)
    {
		memset(this->sockets, 0, sizeof(this->sockets));
	}

	/*
	* Basic functions
	*/
protected:
	bool initImpl(const char* pin = NULL) {
		DBG(GF("### TinyGSM Version:"), TINYGSM_VERSION);
		DBG(GF("### TinyGSM Compiled Module:  TinyGsmClientESP32"));

		if (!this->testAT()) {
            return false;
        }
		if (pin && (strlen(pin) > 0)) {
			DBG("ESP32 modules do not use an unlock pin!");
		}
		this->sendAT(GF("E0")); // Echo Off
		if (this->waitResponse() != 1) {
            return false;
        }
		this->sendAT(GF("+CIPMUX=1"));  // Enable Multiple Connections
		if (this->waitResponse() != 1) {
            return false;
        }
		this->sendAT(GF("+CWMODE=1"));  // Put into "station" mode
		if (this->waitResponse() != 1) {
		    this->sendAT(GF("+CWMODE_CUR=1"));  // Attempt "current" station mode command
                                                // for some firmware variants if needed
			if (this->waitResponse() != 1) {
                return false;
            }
		}
		this->sendAT(GF("+CIPRECVMODE=1"));
		if (this->waitResponse() != 1) {
            return false;
        }
		DBG(GF("### Modem:"), this->getModemName());
		return true;
	}

	String getModemNameImpl() {
		return "ESP32";
	}

	void setBaudImpl(uint32_t baud) {
		this->sendAT(GF("+UART_CUR="), baud, "8,1,0,0");
		if (this->waitResponse() != 1) {
			this->sendAT(GF("+UART="), baud, "8,1,0,0");    // Really old firmwares might need this
			// if (this->waitResponse() != 1) {
			//     this->sendAT(GF("+IPR="), baud);         // First release firmwares might need this
			// }
			this->waitResponse();
		}
	}

	bool factoryDefaultImpl() {
		this->sendAT(GF("+RESTORE"));
		return (this->waitResponse() == 1);
	}

	String getModemInfoImpl() {
		this->sendAT(GF("+GMR"));
		String res;
		if (this->waitResponse(1000L, res) != 1) {
            return "";
        }
		res.replace(GSM_NL "OK" GSM_NL, "");
		res.replace(GSM_NL, " ");
		res.trim();
		return res;
	}

  /*
   * Power functions
   */
protected:
	bool restartImpl(const char* pin = NULL) {
		if (!this->testAT()) {
            return false;
        }
		this->sendAT(GF("+RST"));
		if (this->waitResponse(10000L) != 1) {
            return false;
        }
		if (this->waitResponse(10000L, GF(GSM_NL "ready" GSM_NL)) != 1) {
            return false;
        }
		delay(500);
		return this->init(pin);
	}

	bool powerOffImpl() {
		this->sendAT(GF("+GSLP=0"));  // Power down indefinitely - until manually reset!
		return (this->waitResponse() == 1);
	}

	bool radioOffImpl() TINY_GSM_ATTR_NOT_IMPLEMENTED;

	bool sleepEnableImpl(bool enable = true) TINY_GSM_ATTR_NOT_AVAILABLE;

	bool setPhoneFunctionalityImpl(uint8_t fun, bool reset = false) TINY_GSM_ATTR_NOT_IMPLEMENTED;

  /*
   * Generic network functions
   */
public:
	RegStatus getRegistrationStatus() {
		this->sendAT(GF("+CIPSTATUS"));
		if (this->waitResponse(3000, GF("STATUS:")) != 1) {
            return REG_UNKNOWN;
        }
		int8_t status = this->waitResponse(GFP(GSM_ERROR), GF("2"),
                                        GF("3"), GF("4"), GF("5"));
		this->waitResponse();  // Returns an OK after the status
		return (RegStatus)status;
	}

protected:
	int8_t getSignalQualityImpl() {
		this->sendAT(GF("+CWJAP?"));
		int8_t res1 = this->waitResponse(GF("No AP"), GF("+CWJAP:"));
		if (res1 != 2) {
			this->waitResponse();
			this->sendAT(GF("+CWJAP_CUR?"));    // attempt "current" as used by some firmware
										        // versions
			int8_t res1 = this->waitResponse(GF("No AP"), GF("+CWJAP_CUR:"));
			if (res1 != 2) {
				this->waitResponse();
				return 0;
			}
		}
		this->streamSkipUntil(',');         // Skip SSID
		this->streamSkipUntil(',');         // Skip BSSID/MAC address
		this->streamSkipUntil(',');         // Skip Chanel number
		int8_t res2 = this->stream.parseInt();    // Read RSSI
		this->waitResponse();               // Returns an OK after the value
		return res2;
	}

	bool isNetworkConnectedImpl() {
		RegStatus s = this->getRegistrationStatus();
		if ((s == REG_OK_IP) || (s == REG_OK_TCP)) {
			// with these, we're definitely connected
			return true;
		}
        else if (s == REG_OK_NO_TCP) {
			// with this, we may or may not be connected
			if (this->getLocalIP() == "") {
				return false;
			}
            else {
				return true;
			}
		}
        else {
			return false;
		}
	}

	String getLocalIPImpl() {
		// attempt with and without 'current' flag
		this->sendAT(GF("+CIPSTA?"));
		int8_t res1 = this->waitResponse(GF("ERROR"), GF("+CIPSTA:"));
		if (res1 != 2) {
			this->sendAT(GF("+CIPSTA_CUR?"));
			res1 = this->waitResponse(GF("ERROR"), GF("+CIPSTA_CUR:"));
			if (res1 != 2) {
                return "";
            }
		}
		String res2 = this->stream.readStringUntil('\n');
		res2.replace("ip:", "");  // newer firmwares have this
		res2.replace("\"", "");
		res2.trim();
		this->waitResponse();
		return res2;
	}

	/*
	* WiFi functions
	*/
protected:
	bool networkConnectImpl(const char* ssid, const char* pwd) {
		// attempt first without than with the 'current' flag used in some firmware
		// versions
		this->sendAT(GF("+CWJAP=\""), ssid, GF("\",\""), pwd, GF("\""));
		if (this->waitResponse(30000L, GFP(GSM_OK), GF(GSM_NL "FAIL" GSM_NL)) != 1) {
			this->sendAT(GF("+CWJAP_CUR=\""), ssid, GF("\",\""), pwd, GF("\""));
			if (this->waitResponse(30000L, GFP(GSM_OK), GF(GSM_NL "FAIL" GSM_NL)) != 1) {
				return false;
			}
		}

		return true;
	}

	bool networkDisconnectImpl() {
		this->sendAT(GF("+CWQAP"));
		bool retVal = (this->waitResponse(10000L) == 1);
		this->waitResponse(GF("WIFI DISCONNECT"));
		return retVal;
	}

	/*
	* Client related functions
	*/
protected:
	bool modemConnect(const char* host, uint16_t port, uint8_t mux,
					bool ssl = false, int timeout_s = 75) {
		uint32_t timeout_ms = ((uint32_t)timeout_s) * 1000;
		if (ssl) {
			this->sendAT(GF("+CIPSSLSIZE=4096"));
			this->waitResponse();
		}
		this->sendAT(GF("+CIPSTART="), mux, ',', ssl ? GF("\"SSL") : GF("\"TCP"),
					GF("\",\""), host, GF("\","), port, GF(","),
					TINY_GSM_TCP_KEEP_ALIVE);
		// TODO(?): Check mux
		int8_t rsp = this->waitResponse(timeout_ms, GFP(GSM_OK), GFP(GSM_ERROR),
										GF("ALREADY CONNECT"));
		// if (rsp == 3) {
        //     this->waitResponse();
        // }
		// May return "ERROR" after the "ALREADY CONNECT"
		return (rsp == 1);
	}

	int16_t modemSend(const void* buff, size_t len, uint8_t mux) {
        int retry = TINY_GSM_MAX_SEND_RETRY;
        while (retry--) {
            this->sendAT(GF("+CIPSEND="), mux, ',', (uint16_t)len);
            if (this->waitResponse(GF(">")) != 1) {
                continue;
            }
            this->stream.write(reinterpret_cast<const uint8_t*>(buff), len);
            this->stream.flush();
            if (this->waitResponse(10000L, GF(GSM_NL "SEND OK" GSM_NL)) != 1) {
                continue;
            }
            return len;
        }
        return 0;
	}

	size_t modemRead(size_t size, uint8_t mux) {
		if (!this->sockets[mux]) {
            return 0;
        }
		this->sendAT(GF("+CIPRECVDATA="), mux, ',', (uint16_t)size);
		if (this->waitResponse(GF("+CIPRECVDATA:")) != 1) {
            return 0;
        }
		// int16_t len_requested = size;
		//  ^^ Requested number of data bytes (1-1460 bytes)to be read
		int16_t len_confirmed = this->streamGetIntBefore(',');
		// ^^ The data length which read in the buffer
		for (int i = 0; i < len_confirmed; ++i) {
			uint32_t startMillis = millis();
			while (!this->stream.available() &&
				(millis() - startMillis < this->sockets[mux]->_timeout)) {
				TINY_GSM_YIELD();
			}
			char c = this->stream.read();
			this->sockets[mux]->rx.put(c);
		}
		// this->sockets[mux]->sock_available = modemGetAvailable(mux);
		DBG("### READ:", len_confirmed, "from", mux);
		this->sockets[mux]->sock_available -= len_confirmed;
		this->waitResponse();
		return len_confirmed;
	}

#ifdef TINY_GSM_GET_AVAILABLE
	size_t modemGetAvailable(uint8_t mux) {
		if (!this->sockets[mux]) {
            return 0;
        }
		if (this->sockets[mux]->sock_available) {
			return this->sockets[mux]->sock_available;
		}
		this->sendAT(GF("+CIPRECVLEN?"));
		int16_t result = 0;
		if (this->waitResponse(GF("+CIPRECVLEN:")) == 1) {
			for (size_t i = 0; i < TINY_GSM_MUX_COUNT; ++i) {
				if (i != mux) {
					this->streamSkipUntil(',');
					continue;
				}
				if (mux < TINY_GSM_MUX_COUNT - 1) {
					result = this->streamGetIntBefore(',');
				}
				else {
					result = this->streamGetIntBefore('\n');
				}
				break;
			}
			this->waitResponse();
		}
		DBG("### Available:", result, "on", mux);
		if (result < 0) {
			this->sockets[mux]->sock_connected = this->modemGetConnected(mux);
			return 0;
		}
		return result;
	}
#else
	size_t modemGetAvailable(uint8_t mux) {
		if (!this->sockets[mux]) {
            return 0;
        }
		return this->sockets[mux]->sock_available;
	}
#endif

	bool modemGetConnected(uint8_t mux) {
		this->sendAT(GF("+CIPSTATUS"));
		if (this->waitResponse(3000, GF("STATUS:")) != 1) {
            return false;
        }
		int8_t status = this->waitResponse(GFP(GSM_ERROR), GF("2"),
										GF("3"), GF("4"), GF("5"));
		if (status != 3) {
			// if the status is anything but 3, there are no connections open
			this->waitResponse();  // Returns an OK after the status
			for (int muxNo = 0; muxNo < TINY_GSM_MUX_COUNT; ++muxNo) {
				if (this->sockets[muxNo]) {
                    this->sockets[muxNo]->sock_connected = false;
                }
			}
			return false;
		}
		bool verified_connections[TINY_GSM_MUX_COUNT] = {0, 0, 0, 0, 0};
		for (int muxNo = 0; muxNo < TINY_GSM_MUX_COUNT; ++muxNo) {
			uint8_t has_status = this->waitResponse(GF("+CIPSTATUS:"), GFP(GSM_OK),
													GFP(GSM_ERROR));
			if (has_status == 1) {
				int8_t returned_mux = this->streamGetIntBefore(',');
				this->streamSkipUntil(',');   // Skip mux
				this->streamSkipUntil(',');   // Skip type
				this->streamSkipUntil(',');   // Skip remote IP
				this->streamSkipUntil(',');   // Skip remote port
				this->streamSkipUntil(',');   // Skip local port
				this->streamSkipUntil('\n');  // Skip client/server type
				verified_connections[returned_mux] = 1;
			}
			if (has_status == 2) {  // once we get to the ok, stop
                break;
            }
		}
		for (int muxNo = 0; muxNo < TINY_GSM_MUX_COUNT; ++muxNo) {
			if (this->sockets[muxNo]) {
				this->sockets[muxNo]->sock_connected = verified_connections[muxNo];
			}
		}
		return verified_connections[mux];
	}

	/*
	* Utilities
	*/
public:
	// TODO: Optimize this!
	int8_t waitResponse(uint32_t timeout_ms, String& data,
						GsmConstStr r1 = GFP(GSM_OK),
						GsmConstStr r2 = GFP(GSM_ERROR), GsmConstStr r3 = NULL,
						GsmConstStr r4 = NULL, GsmConstStr r5 = NULL) {
		/* String r1s(r1); r1s.trim();
		String r2s(r2); r2s.trim();
		String r3s(r3); r3s.trim();
		String r4s(r4); r4s.trim();
		String r5s(r5); r5s.trim();
		DBG("### ..:", r1s, ",", r2s, ",", r3s, ",", r4s, ",", r5s); */
		data.reserve(64);
		uint8_t  index       = 0;
		uint32_t startMillis = millis();
		do {
			TINY_GSM_YIELD();
			while (this->stream.available() > 0) {
				TINY_GSM_YIELD();
				int8_t a = this->stream.read();
				if (a <= 0) {  // Skip 0x00 bytes, just in case
                    continue;
                }
				data += static_cast<char>(a);
				if (r1 && data.endsWith(r1)) {
					index = 1;
					goto finish;
				}
                else if (r2 && data.endsWith(r2)) {
					index = 2;
					goto finish;
				}
                else if (r3 && data.endsWith(r3)) {
					index = 3;
					goto finish;
				}
                else if (r4 && data.endsWith(r4)) {
					index = 4;
					goto finish;
				}
                else if (r5 && data.endsWith(r5)) {
					index = 5;
					goto finish;
				}
                else if (data.endsWith(GF("+IPD,"))) {
					int8_t  mux = this->streamGetIntBefore(',');
					int16_t len = this->streamGetIntBefore('\n');
					if ((mux >= 0) && (mux < TINY_GSM_MUX_COUNT) && this->sockets[mux]) {
						this->sockets[mux]->got_data = true;
#ifdef TINY_GSM_GET_AVAILABLE
						if ((len >= 0) && (len <= 1024)) {
                            this->sockets[mux]->sock_available = len;
                        }
#else
						this->sockets[mux]->sock_available = len;
#endif
						DBG("### Got Data:", mux);
					}
					data = "";
				} else if (data.endsWith(GF("CLOSED"))) {
					int8_t muxStart = TinyGsmMax(0, data.lastIndexOf(GSM_NL, data.length() - 8));
					int8_t coma = data.indexOf(',', muxStart);
					int8_t mux  = data.substring(muxStart, coma).toInt();
					if ((mux >= 0) && (mux < TINY_GSM_MUX_COUNT) && this->sockets[mux]) {
						this->sockets[mux]->sock_connected = false;
					}
					data = "";
					DBG("### Closed: ", mux);
				}
			}
		} while (millis() - startMillis < timeout_ms);
		finish:
		if (!index) {
			data.trim();
			if (data.length()) {
                DBG("### Unhandled:", data);
            }
			data = "";
		}
		data.replace(GSM_NL, "/");
		DBG('<', index, '>', data);
		return index;
	}

	int8_t waitResponse(uint32_t timeout_ms, GsmConstStr r1 = GFP(GSM_OK),
						GsmConstStr r2 = GFP(GSM_ERROR), GsmConstStr r3 = NULL,
						GsmConstStr r4 = NULL, GsmConstStr r5 = NULL) {
		String data;
		return this->waitResponse(timeout_ms, data, r1, r2, r3, r4, r5);
	}

	int8_t waitResponse(GsmConstStr r1 = GFP(GSM_OK),
						GsmConstStr r2 = GFP(GSM_ERROR), GsmConstStr r3 = NULL,
						GsmConstStr r4 = NULL, GsmConstStr r5 = NULL) {
		return this->waitResponse(1000, r1, r2, r3, r4, r5);
	}

public:
	Stream& stream;

protected:
	GsmClientESP32* sockets[TINY_GSM_MUX_COUNT];
	const char*     gsmNL = GSM_NL;
};

#endif  /* INC_TINYGSM_CLIENT_ESP32_HPP_ */
