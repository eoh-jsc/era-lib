/**
 * @file       TinyGsmClientEC20.hpp
 */

#ifndef INC_TINYGSM_CLIENT_EC20_HPP_
#define INC_TINYGSM_CLIENT_EC20_HPP_

// #pragma message("TinyGSM:  TinyGsmClientEC20")

// #define TINY_GSM_DEBUG Serial

#define TINY_GSM_MUX_COUNT 12
#define TINY_GSM_BUFFER_READ_AND_CHECK_SIZE

#include <TinyGsmBattery.tpp>
#include <TinyGsmCalling.tpp>
#include <TinyGsmGPRS.tpp>
#include <TinyGsmGPS.tpp>
#include <TinyGsmModem.tpp>
#include <TinyGsmSMS.tpp>
#include <TinyGsmTCP.tpp>
#include <TinyGsmTemperature.tpp>
#include <TinyGsmTime.tpp>
#include <TinyGsmNTP.tpp>

#define GSM_NL      "\r\n"
static const char   GSM_OK[] TINY_GSM_PROGMEM    = "OK" GSM_NL;
static const char   GSM_ERROR[] TINY_GSM_PROGMEM = "ERROR" GSM_NL;
#if defined         TINY_GSM_DEBUG
static const char   GSM_CME_ERROR[] TINY_GSM_PROGMEM = GSM_NL "+CME ERROR:";
static const char   GSM_CMS_ERROR[] TINY_GSM_PROGMEM = GSM_NL "+CMS ERROR:";
#endif

enum RegStatus {
    REG_NO_RESULT    = -1,
    REG_UNREGISTERED = 0,
    REG_SEARCHING    = 2,
    REG_DENIED       = 3,
    REG_OK_HOME      = 1,
    REG_OK_ROAMING   = 5,
    REG_UNKNOWN      = 4,
};

class TinyGsmEC20
    : public TinyGsmModem<TinyGsmEC20>
    , public TinyGsmGPRS<TinyGsmEC20>
    , public TinyGsmTCP<TinyGsmEC20, TINY_GSM_MUX_COUNT>
    , public TinyGsmCalling<TinyGsmEC20>
    , public TinyGsmSMS<TinyGsmEC20>
    , public TinyGsmTime<TinyGsmEC20>
    , public TinyGsmNTP<TinyGsmEC20>
    , public TinyGsmGPS<TinyGsmEC20>
    , public TinyGsmBattery<TinyGsmEC20>
    , public TinyGsmTemperature<TinyGsmEC20>
{
    friend class TinyGsmModem<TinyGsmEC20>;
    friend class TinyGsmGPRS<TinyGsmEC20>;
    friend class TinyGsmTCP<TinyGsmEC20, TINY_GSM_MUX_COUNT>;
    friend class TinyGsmCalling<TinyGsmEC20>;
    friend class TinyGsmSMS<TinyGsmEC20>;
    friend class TinyGsmTime<TinyGsmEC20>;
    friend class TinyGsmNTP<TinyGsmEC20>;
    friend class TinyGsmGPS<TinyGsmEC20>;
    friend class TinyGsmBattery<TinyGsmEC20>;
    friend class TinyGsmTemperature<TinyGsmEC20>;

    /*
    * Inner Client
    */
public:
    class GsmClientEC20
        : public GsmClient
    {
        friend class TinyGsmEC20;

    public:
        GsmClientEC20()
        {}

        explicit GsmClientEC20(TinyGsmEC20& modem, uint8_t mux = 0) {
            this->init(&modem, mux);
        }

        bool init(TinyGsmEC20* modem, uint8_t mux = 0) {
            this->at       = modem;
            this->sock_available = 0;
            this->prev_check     = 0;
            this->sock_connected = false;
            this->got_data       = false;

            if (mux < TINY_GSM_MUX_COUNT) {
                this->mux = mux;
            }
            else {
                this->mux = (mux % TINY_GSM_MUX_COUNT);
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
            uint32_t startMillis = millis();
            this->dumpModemBuffer(maxWaitMs);
            this->at->sendAT(GF("+QICLOSE="), this->mux);
            this->sock_connected = false;
            this->at->waitResponse((maxWaitMs - (millis() - startMillis)));
        }
        void stop() override {
            this->stop(15000L);
        }

        /*
        * Extended API
        */

        String remoteIP() TINY_GSM_ATTR_NOT_IMPLEMENTED;
    };

    /*
    * Inner Secure Client
    */

    /*
    class GsmClientSecureEC20
        : public GsmClientEC20
    {
    public:
        GsmClientSecure()
        {}

        GsmClientSecure(TinyGsmEC20& modem, uint8_t mux = 0)
            : public GsmClient(modem, mux)
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
    */

    /*
    * Constructor
    */
public:
    explicit TinyGsmEC20(Stream& stream)
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
        DBG(GF("### TinyGSM Compiled Module:  TinyGsmClientEC20"));

        if (!this->testAT()) {
            return false;
        }

        this->sendAT(GF("E0"));  // Echo Off
        if (this->waitResponse() != 1) {
            return false;
        }

    #ifdef TINY_GSM_DEBUG
        this->sendAT(GF("+CMEE=2"));  // turn on verbose error codes
    #else
        this->sendAT(GF("+CMEE=0"));  // turn off error codes
    #endif
        this->waitResponse();

        DBG(GF("### Modem:"), this->getModemName());

        // Disable time and time zone URC's
        this->sendAT(GF("+CTZR=0"));
        if (this->waitResponse(10000L) != 1) {
            return false;
        }

        // Enable automatic time zone update
        this->sendAT(GF("+CTZU=1"));
        if (this->waitResponse(10000L) != 1) {
            return false;
        }

        SimStatus ret = this->getSimStatus();
        // if the sim isn't ready and a pin has been provided, try to unlock the sim
        if ((ret != SIM_READY) && (pin != NULL) && (strlen(pin) > 0)) {
            this->simUnlock(pin);
            return (this->getSimStatus() == SIM_READY);
        }
        else {
            // if the sim is ready, or it's locked but no pin has been provided,
            // return true
            return ((ret == SIM_READY) || (ret == SIM_LOCKED));
        }
    }

    String getModemNameImpl() {
        String name = "EC20F";

        this->sendAT(GF("+CGMM"));
        String res2;
        if (this->waitResponse(1000L, res2) != 1) {
            return name;
        }
        res2.replace(GSM_NL "OK" GSM_NL, "");
        res2.replace("_", " ");
        res2.trim();

        name = res2;
        DBG("### Modem:", name);
        return name;
    }

    bool factoryDefaultImpl() {  // these commands aren't supported
        return false;
    }

    /*
    * Power functions
    */
protected:
    bool restartImpl(const char* pin = NULL) {
        if (!this->testAT()) {
            return false;
        }
        if (!this->setPhoneFunctionality(1, true)) {
            return false;
        }
        this->waitResponse(10000L, GF("RDY"));
        return this->init(pin);
    }

    bool powerOffImpl() {
        this->sendAT(GF("+QPOWD=1"));
        this->waitResponse(300);  // returns OK first
        return this->waitResponse(300, GF("POWERED DOWN")) == 1;
    }

    bool radioOffImpl() {
        if (!this->setPhoneFunctionality(4)) {
            return false;
        }
        delay(3000);
        return true;
    }
    // When entering into sleep mode is enabled, DTR is pulled up, and WAKEUP_IN
    // is pulled up, the module can directly enter into sleep mode.If entering
    // into sleep mode is enabled, DTR is pulled down, and WAKEUP_IN is pulled
    // down, there is a need to pull the DTR pin and the WAKEUP_IN pin up first,
    // and then the module can enter into sleep mode.
    bool sleepEnableImpl(bool enable = true) {
        this->sendAT(GF("+QSCLK="), enable);
        return (this->waitResponse() == 1);
    }

    bool setPhoneFunctionalityImpl(uint8_t fun, bool reset = false) {
        this->sendAT(GF("+CFUN="), fun, reset ? ",1" : "");
        return (this->waitResponse(10000L, GF("OK")) == 1);
    }

    /*
    * Generic network functions
    */
public:
    RegStatus getRegistrationStatus() {
        // Check first for EPS registration
        RegStatus epsStatus = (RegStatus)this->getRegistrationStatusXREG("CEREG");

        // If we're connected on EPS, great!
        if ((epsStatus == REG_OK_HOME) ||
            (epsStatus == REG_OK_ROAMING)) {
            return epsStatus;
        }
        else {
            // Otherwise, check generic network status
            return (RegStatus)this->getRegistrationStatusXREG("CREG");
        }
    }

protected:
    bool isNetworkConnectedImpl() {
        RegStatus s = this->getRegistrationStatus();
        return ((s == REG_OK_HOME) || (s == REG_OK_ROAMING));
    }

public:
    String getNetworkModes() {
        this->sendAT(GF("+QCFG=?"));
        if (this->waitResponse(GF(GSM_NL "+QCFG:")) != 1) {
            return "";
        }
        String res = this->stream.readStringUntil('\n');
        this->waitResponse();
        return res;
    }

    int16_t getNetworkMode() {
        this->sendAT(GF("+QCFG=\"nwscanmode\""));
        if (this->waitResponse(GF(GSM_NL "+QCFG:")) != 1) {
            return false;
        }
        int16_t mode = this->streamGetIntBefore('\n');
        this->waitResponse();
        return mode;
    }

    bool setNetworkMode(uint8_t mode) {
        this->sendAT(GF("+QCFG=\"nwscanmode\","), mode);
        return (this->waitResponse() == 1);
    }

    String getLocalIPImpl() {
        // this->sendAT(GF("+IPADDR"));  // Inquire Socket PDP address
        this->sendAT(GF("+CGPADDR=1"));  // Show PDP address
        if (this->waitResponse(10000L, GF("+CGPADDR:")) != 1) {
            return "";
        }
        String res;
        this->streamSkipUntil('\"');
        res = this->stream.readStringUntil('\"');
        if (this->waitResponse() != 1) {
            return "";
        }
        return res;
    }

    /*
    * GPRS functions
    */
protected:
    bool gprsConnectImpl(const char* apn, const char* user = NULL,
                        const char* pwd = NULL) {
        this->gprsDisconnect();

        // Configure the TCPIP Context
        this->sendAT(GF("+QICSGP=1,1,\""), apn, GF("\",\""), user,
                    GF("\",\""), pwd, GF("\""), GF(","), 1);
        if (this->waitResponse() != 1) {
            return false;
        }
        // Activate GPRS/CSD Context
        this->sendAT(GF("+QIACT=1"));
        if (this->waitResponse(150000L) != 1) {
            return false;
        }

        // Attach to Packet Domain service - is this necessary?
        this->sendAT(GF("+CGATT=1"));
        if (this->waitResponse(60000L) != 1) {
            return false;
        }

        return true;
    }

    bool gprsDisconnectImpl() {
        this->sendAT(GF("+QIDEACT=1"));  // Deactivate the bearer context
        if (this->waitResponse(40000L) != 1) {
            return false;
        }
        return true;
    }

    bool isGprsConnectedImpl() {
        this->sendAT(GF("+CGATT?"));
        // May return +CGATT: 1, 0.  We just confirm that the first number is 1
        if (this->waitResponse(GF(GSM_NL "+CGATT: 1")) != 1) {
            return false;
        }
        this->waitResponse();

        // Check local IP
        return (this->localIP() != IPAddress(0, 0, 0, 0));
    }

    /*
    * SIM card functions
    */
protected:
    String getSimCCIDImpl() {
        this->sendAT(GF("+QCCID"));
        if (this->waitResponse(GF(GSM_NL "+QCCID:")) != 1) {
            return "";
        }
        String res = this->stream.readStringUntil('\n');
        this->waitResponse();
        res.trim();
        return res;
    }

    /*
    * Phone Call functions
    */
protected:
    bool callHangupImpl() {
        this->sendAT(GF("+CHUP"));
        return (this->waitResponse() == 1);
    }

    /*
    * Messaging functions
    */
protected:
    // Follows all messaging functions per template

    /*
    * GSM Location functions
    */
protected:
    // NOTE:  As of application firmware version 01.016.01.016 triangulated
    // locations can be obtained via the QuecLocator service and accompanying AT
    // commands.  As this is a separate paid service which I do not have access
    // to, I am not implementing it here.

    /*
    * GPS/GNSS/GLONASS location functions
    */
protected:
    // enable GPS
    bool enableGPSImpl() {
        this->sendAT(GF("+QGPS=1"));
        if (this->waitResponse() != 1) {
            return false;
        }
        return true;
    }

    bool disableGPSImpl() {
        this->sendAT(GF("+QGPSEND"));
        if (this->waitResponse() != 1) {
            return false;
        }
        return true;
    }

    // get the RAW GPS output
    String getGPSrawImpl() {
        this->sendAT(GF("+QGPSLOC=2"));
        if (this->waitResponse(10000L, GF(GSM_NL "+QGPSLOC:")) != 1) {
            return "";
        }
        String res = this->stream.readStringUntil('\n');
        this->waitResponse();
        res.trim();
        return res;
    }

    // get GPS informations
    bool getGPSImpl(float* lat, float* lon, float* speed = 0, float* alt = 0,
                    int* vsat = 0, int* usat = 0, float* accuracy = 0,
                    int* year = 0, int* month = 0, int* day = 0, int* hour = 0,
                    int* minute = 0, int* second = 0) {
        this->sendAT(GF("+QGPSLOC=2"));
        if (this->waitResponse(10000L, GF(GSM_NL "+QGPSLOC:")) != 1) {
            // NOTE:  Will return an error if the position isn't fixed
            return false;
        }

        // init variables
        float ilat         = 0;
        float ilon         = 0;
        float ispeed       = 0;
        float ialt         = 0;
        int   iusat        = 0;
        float iaccuracy    = 0;
        int   iyear        = 0;
        int   imonth       = 0;
        int   iday         = 0;
        int   ihour        = 0;
        int   imin         = 0;
        float secondWithSS = 0;

        // UTC date & Time
        ihour        = this->streamGetIntLength(2);      // Two digit hour
        imin         = this->streamGetIntLength(2);      // Two digit minute
        secondWithSS = this->streamGetFloatBefore(',');  // 6 digit second with subseconds

        ilat         = this->streamGetFloatBefore(',');  // Latitude
        ilon         = this->streamGetFloatBefore(',');  // Longitude
        iaccuracy    = this->streamGetFloatBefore(',');  // Horizontal precision
        ialt         = this->streamGetFloatBefore(',');  // Altitude from sea level
        this->streamSkipUntil(',');                   // GNSS positioning mode
        this->streamSkipUntil(',');  // Course Over Ground based on true north
        this->streamSkipUntil(',');  // Speed Over Ground in Km/h
        ispeed       = this->streamGetFloatBefore(',');  // Speed Over Ground in knots

        iday         = this->streamGetIntLength(2);    // Two digit day
        imonth       = this->streamGetIntLength(2);    // Two digit month
        iyear        = this->streamGetIntBefore(',');  // Two digit year

        iusat        = this->streamGetIntBefore(',');  // Number of satellites,
        this->streamSkipUntil('\n');  // The error code of the operation. If it is not
                                // 0, it is the type of error.

        // Set pointers
        if (lat != NULL) {
            *lat = ilat;
        }
        if (lon != NULL) {
            *lon = ilon;
        }
        if (speed != NULL) {
            *speed = ispeed;
        }
        if (alt != NULL) {
            *alt = ialt;
        }
        if (vsat != NULL) {
            *vsat = 0;
        }
        if (usat != NULL) {
            *usat = iusat;
        }
        if (accuracy != NULL) {
            *accuracy = iaccuracy;
        }
        if (iyear < 2000) {
            iyear += 2000;
        }
        if (year != NULL) {
            *year = iyear;
        }
        if (month != NULL) {
            *month = imonth;
        }
        if (day != NULL) {
            *day = iday;
        }
        if (hour != NULL) {
            *hour = ihour;
        }
        if (minute != NULL) {
            *minute = imin;
        }
        if (second != NULL) {
            *second = static_cast<int>(secondWithSS);
        }

        this->waitResponse();  // Final OK
        return true;
    }

    /*
    * Time functions
    */
protected:
    String getGSMDateTimeImpl(TinyGSMDateTimeFormat format) {
        this->sendAT(GF("+QLTS=2"));
        if (this->waitResponse(2000L, GF("+QLTS: \"")) != 1) {
            return "";
        }

        String res;

        switch (format) {
            case DATE_FULL:
                res = this->stream.readStringUntil('"');
                break;
            case DATE_TIME:
                this->streamSkipUntil(',');
                res = this->stream.readStringUntil('"');
                break;
            case DATE_DATE:
                res = stream.readStringUntil(',');
                break;
        }
        this->waitResponse();  // Ends with OK
        return res;
    }

    // The EC20 returns UTC time instead of local time as other modules do in
    // response to CCLK, so we're using QLTS where we can specifically request
    // local time.
    bool getNetworkTimeImpl(int* year, int* month, int* day, int* hour,
                            int* minute, int* second, float* timezone) {
        this->sendAT(GF("+QLTS=2"));
        if (this->waitResponse(2000L, GF("+QLTS: \"")) != 1) {
            return false;
        }

        int iyear     = 0;
        int imonth    = 0;
        int iday      = 0;
        int ihour     = 0;
        int imin      = 0;
        int isec      = 0;
        int itimezone = 0;

        // Date & Time
        iyear       = this->streamGetIntBefore('/');
        imonth      = this->streamGetIntBefore('/');
        iday        = this->streamGetIntBefore(',');
        ihour       = this->streamGetIntBefore(':');
        imin        = this->streamGetIntBefore(':');
        isec        = this->streamGetIntLength(2);
        char tzSign = this->stream.read();
        itimezone   = this->streamGetIntBefore(',');
        if (tzSign == '-') {
            itimezone = itimezone * -1;
        }
        this->streamSkipUntil('\n');  // DST flag

        // Set pointers
        if (iyear < 2000) {
            iyear += 2000;
        }
        if (year != NULL) {
            *year = iyear;
        }
        if (month != NULL) {
            *month = imonth;
        }
        if (day != NULL) {
            *day = iday;
        }
        if (hour != NULL) {
            *hour = ihour;
        }
        if (minute != NULL) {
            *minute = imin;
        }
        if (second != NULL) {
            *second = isec;
        }
        if (timezone != NULL) {
            *timezone = static_cast<float>(itimezone) / 4.0;
        }

        // Final OK
        this->waitResponse();  // Ends with OK
        return true;
    }

    /*
    * NTP server functions
    */

    byte NTPServerSyncImpl(String server = "pool.ntp.org", byte = -5) {
        // Request network synchronization
        // AT+QNTP=<contextID>,<server>[,<port>][,<autosettime>]
        this->sendAT(GF("+QNTP=1,\""), server, '"');
        if (this->waitResponse(10000L, GF("+QNTP:"))) {
            String result = this->stream.readStringUntil(',');
            this->streamSkipUntil('\n');
            result.trim();
            if (this->TinyGsmIsValidNumber(result)) {
                return result.toInt();
            }
        }
        else {
            return -1;
        }
        return -1;
    }

    String ShowNTPErrorImpl(byte error) TINY_GSM_ATTR_NOT_IMPLEMENTED;

    /*
    * Battery functions
    */
protected:
    // returns volts, multiply by 1000 to get mV
    uint16_t getBattVoltageImpl() {
        this->sendAT(GF("+CBC"));
        if (this->waitResponse(GF(GSM_NL "+CBC:")) != 1) {
            return 0;
        }

        // get voltage in VOLTS
        float voltage = this->streamGetFloatBefore('\n');
        // Wait for final OK
        this->waitResponse();
        // Return millivolts
        uint16_t res = voltage * 1000;
        return res;
    }

    int8_t getBattPercentImpl() TINY_GSM_ATTR_NOT_AVAILABLE;

    uint8_t getBattChargeStateImpl() TINY_GSM_ATTR_NOT_AVAILABLE;

    bool getBattStatsImpl(uint8_t& chargeState, int8_t& percent,
                            uint16_t& milliVolts) {
        chargeState = 0;
        percent     = 0;
        milliVolts  = this->getBattVoltage();
        return true;
    }

    /*
    * Temperature functions
    */
protected:
    // get temperature in degree celsius
    uint16_t getTemperatureImpl() {
        this->sendAT(GF("+QTEMP"));
        if (this->waitResponse(GF(GSM_NL "+QTEMP:")) != 1) {
            return 0;
        }
        // return temperature in C
        uint16_t res = this->streamGetIntBefore(',');  // read PMIC (primary ic) temperature
        this->streamSkipUntil(',');         // skip XO temperature ??
        this->streamSkipUntil('\n');        // skip PA temperature ??
        // Wait for final OK
        this->waitResponse();
        return res;
    }

    /*
    * Client related functions
    */
protected:
    bool modemConnect(const char* host, uint16_t port, uint8_t mux,
                        bool ssl = false, int timeout_s = 150) {
        if (ssl) {
            DBG("SSL not yet supported on this module!");
        }

        uint32_t timeout_ms = ((uint32_t)timeout_s) * 1000;

        // <PDPcontextID>(1-16), <connectID>(0-11),
        // "TCP/UDP/TCP LISTENER/UDPSERVICE", "<IP_address>/<domain_name>",
        // <remote_port>,<local_port>,<access_mode>(0-2; 0=buffer)
        this->sendAT(GF("+QIOPEN=1,"), mux, GF(",\""), GF("TCP"),
                    GF("\",\""), host, GF("\","), port, GF(",0,0"));
        this->waitResponse();

        if (this->waitResponse(timeout_ms, GF(GSM_NL "+QIOPEN:")) != 1) {
            return false;
        }

        if (this->streamGetIntBefore(',') != mux) {
            return false;
        }
        // Read status
        return (this->streamGetIntBefore('\n') == 0);
    }

    int16_t modemSend(const void* buff, size_t len, uint8_t mux) {
        this->sendAT(GF("+QISEND="), mux, ',', (uint16_t)len);
        if (this->waitResponse(GF(">")) != 1) {
            return 0;
        }
        this->stream.write(reinterpret_cast<const uint8_t*>(buff), len);
        this->stream.flush();
        if (this->waitResponse(GF(GSM_NL "SEND OK")) != 1) {
            return 0;
        }
        // TODO(?): Wait for ACK? AT+QISEND=id,0
        return len;
    }

    size_t modemRead(size_t size, uint8_t mux) {
        if (!this->sockets[mux]) {
            return 0;
        }
        this->sendAT(GF("+QIRD="), mux, ',', (uint16_t)size);
        if (this->waitResponse(GF("+QIRD:")) != 1) {
            return 0;
        }
        int16_t len = this->streamGetIntBefore('\n');

        for (int i = 0; i < len; i++) {
            this->moveCharFromStreamToFifo(mux);
        }
        this->waitResponse();
        // DBG("### READ:", len, "from", mux);
        size_t available = this->modemGetAvailable(mux);
        if (available && ((available & 0xFFFF) == 0)) {
            available--;
        }
        this->sockets[mux]->sock_available = available;
        return len;
    }

    size_t modemGetAvailable(uint8_t mux) {
        if (!this->sockets[mux]) {
            return 0;
        }
        this->sendAT(GF("+QIRD="), mux, GF(",0"));
        size_t result = 0;
        if (this->waitResponse(GF("+QIRD:")) == 1) {
            this->streamSkipUntil(',');  // Skip total received
            this->streamSkipUntil(',');  // Skip have read
            result = this->streamGetIntBefore('\n');
            if (result) {
                DBG("### DATA AVAILABLE:", result, "on", mux);
            }
            this->waitResponse();
        }
        if (!result) {
            this->sockets[mux]->sock_connected = this->modemGetConnected(mux);
        }
        return result;
    }

    bool modemGetConnected(uint8_t mux) {
        this->sendAT(GF("+QISTATE=1,"), mux);
        // +QISTATE: 0,"TCP","151.139.237.11",80,5087,4,1,0,0,"uart1"

        if (this->waitResponse(GF("+QISTATE:")) != 1) {
            return false;
        }

        this->streamSkipUntil(',');                  // Skip mux
        this->streamSkipUntil(',');                  // Skip socket type
        this->streamSkipUntil(',');                  // Skip remote ip
        this->streamSkipUntil(',');                  // Skip remote port
        this->streamSkipUntil(',');                  // Skip local port
        int8_t res = this->streamGetIntBefore(',');  // socket state

        this->waitResponse();

        // 0 Initial, 1 Opening, 2 Connected, 3 Listening, 4 Closing
        return (res == 2);
    }

    /*
    * Utilities
    */
public:
    // TODO(vshymanskyy): Optimize this!
    int8_t waitResponse(uint32_t timeout_ms, String& data,
                        GsmConstStr r1 = GFP(GSM_OK),
                        GsmConstStr r2 = GFP(GSM_ERROR),
#if defined(TINY_GSM_DEBUG)
                        GsmConstStr r3 = GFP(GSM_CME_ERROR),
                        GsmConstStr r4 = GFP(GSM_CMS_ERROR),
#else
                        GsmConstStr r3 = NULL, GsmConstStr r4 = NULL,
#endif
                        GsmConstStr r5 = NULL) {
        /*String r1s(r1); r1s.trim();
        String r2s(r2); r2s.trim();
        String r3s(r3); r3s.trim();
        String r4s(r4); r4s.trim();
        String r5s(r5); r5s.trim();
        DBG("### ..:", r1s, ",", r2s, ",", r3s, ",", r4s, ",", r5s);*/
        data.reserve(64);
        uint8_t  index       = 0;
        uint32_t startMillis = millis();
        do {
            TINY_GSM_YIELD();
            while (this->stream.available() > 0) {
                TINY_GSM_YIELD();
                int8_t a = this->stream.read();
                if (a <= 0) {
                    continue;  // Skip 0x00 bytes, just in case
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
#if defined(TINY_GSM_DEBUG)
                    if (r3 == GFP(GSM_CME_ERROR)) {
                        this->streamSkipUntil('\n');  // Read out the error
                    }
#endif
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
                else if (data.endsWith(GF(GSM_NL "+QIURC:"))) {
                    this->streamSkipUntil('\"');
                    String urc = this->stream.readStringUntil('\"');
                    this->streamSkipUntil(',');
                    if (urc == "recv") {
                        int8_t mux = this->streamGetIntBefore('\n');
                        DBG("### URC RECV:", mux);
                        if ((mux >= 0) && (mux < TINY_GSM_MUX_COUNT) && this->sockets[mux]) {
                            this->sockets[mux]->got_data = true;
                        }
                    }
                    else if (urc == "closed") {
                        int8_t mux = this->streamGetIntBefore('\n');
                        DBG("### URC CLOSE:", mux);
                        if ((mux >= 0) && (mux < TINY_GSM_MUX_COUNT) && this->sockets[mux]) {
                            this->sockets[mux]->sock_connected = false;
                        }
                    }
                    else {
                        this->streamSkipUntil('\n');
                    }
                    data = "";
                }
            }
        } while ((millis() - startMillis) < timeout_ms);
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
                        GsmConstStr r2 = GFP(GSM_ERROR),
#if defined(TINY_GSM_DEBUG)
                        GsmConstStr r3 = GFP(GSM_CME_ERROR),
                        GsmConstStr r4 = GFP(GSM_CMS_ERROR),
#else
                        GsmConstStr r3 = NULL, GsmConstStr r4 = NULL,
#endif
                        GsmConstStr r5 = NULL) {
        String data;
        return this->waitResponse(timeout_ms, data, r1, r2, r3, r4, r5);
    }

    int8_t waitResponse(GsmConstStr r1 = GFP(GSM_OK),
                        GsmConstStr r2 = GFP(GSM_ERROR),
#if defined(TINY_GSM_DEBUG)
                        GsmConstStr r3 = GFP(GSM_CME_ERROR),
                        GsmConstStr r4 = GFP(GSM_CMS_ERROR),
#else
                        GsmConstStr r3 = NULL, GsmConstStr r4 = NULL,
#endif
                        GsmConstStr r5 = NULL) {
        return this->waitResponse(1000, r1, r2, r3, r4, r5);
    }

public:
    Stream& stream;

protected:
    inline
    int streamGetIntBefore(char lastChar) {
        char   buf[10];

        size_t bytesRead = this->stream.readBytesUntil(lastChar,
                                    buf, static_cast<size_t>(10));
        // if we read 7 or more bytes, it's an overflow
        if (bytesRead && (bytesRead < 10)) {
            buf[bytesRead] = '\0';
            int res    = atoi(buf);
            return res;
        }

        return -9999;
    }

    GsmClientEC20* sockets[TINY_GSM_MUX_COUNT];
    const char*    gsmNL = GSM_NL;
};

#endif  /* INC_TINYGSM_CLIENT_EC20_HPP_ */
