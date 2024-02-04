#ifndef INC_ERA_ZIGBEE_HPP_
#define INC_ERA_ZIGBEE_HPP_

#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <ERa/ERaParam.hpp>
#include <ERa/ERaTimer.hpp>
#include <Utility/ERaUtility.hpp>
#include <Zigbee/ERaZigbeeConfig.hpp>
#include <Zigbee/ERaZigbeeState.hpp>
#include <Zigbee/ERaToZigbee.hpp>
#include <Zigbee/ERaFromZigbee.hpp>
#include <Zigbee/ERaDBZigbee.hpp>
#include "utility/ERaUtilityZigbee.hpp"

using namespace std;

template <class Api>
class ERaZigbee
    : public ERaToZigbee< ERaZigbee<Api> >
    , public ERaFromZigbee< ERaZigbee<Api> >
    , public ERaDBZigbee< ERaZigbee<Api> >
{
    const char* TAG = "Zigbee";
    const uint8_t SOF = 0xFE;
    const uint8_t DataStart = 4;

    const uint8_t PositionSOF = 0;
    const uint8_t PositionDataLength = 1;
    const uint8_t PositionCmd0 = 2;
    const uint8_t PositionCmd1 = 3;
    
    const uint8_t MinMessageLength = 5;
    const uint8_t MaxDataSize = 250;

    const uint8_t BEACON_MAX_DEPTH = 0x0F;
    const OptionsAFT Options = OptionsAFT::DEFAULT_OP;
    const uint8_t Radius = 2 * BEACON_MAX_DEPTH; // 0x1E

    const uint16_t DefaultPanId = 0x2706;
    const ZBChannelT DefaultChannel = ZBChannelT::CHANNEL_11;
    const ZBChannelT ChannelList[16] = {ZBChannelT::CHANNEL_11, ZBChannelT::CHANNEL_12, ZBChannelT::CHANNEL_13, ZBChannelT::CHANNEL_14, ZBChannelT::CHANNEL_15,
                                        ZBChannelT::CHANNEL_16, ZBChannelT::CHANNEL_17, ZBChannelT::CHANNEL_18, ZBChannelT::CHANNEL_19, ZBChannelT::CHANNEL_20,
                                        ZBChannelT::CHANNEL_21, ZBChannelT::CHANNEL_22, ZBChannelT::CHANNEL_23, ZBChannelT::CHANNEL_24, ZBChannelT::CHANNEL_25,
                                        ZBChannelT::CHANNEL_26};
    const uint8_t DefaultNetworkKey[16] = {0xF4, 0x3C, 0x95, 0xC2, 0x88, 0x27, 0x06, 0x95, 0xC5, 0x51, 0x2B, 0xB1, 0xB6, 0x57, 0x1A, 0x24};
    const uint8_t DefaultExtPanId[8] = {0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD};
    const uint8_t TcLinkKey[32] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                0x5a, 0x69, 0x67, 0x42, 0x65, 0x65, 0x41, 0x6c, 0x6c, 0x69, 0x61, 0x6e, 0x63, 0x65, 0x30, 0x39,
                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; /* ZigBeeAlliance09 */

    typedef void* TaskHandle_t;
    typedef void* QueueMessage_t;
    friend class ERaToZigbee< ERaZigbee<Api> >;
    friend class ERaFromZigbee< ERaZigbee<Api> >;
    friend class ERaDBZigbee< ERaZigbee<Api> >;

protected:
    typedef ERaToZigbee< ERaZigbee<Api> > ToZigbee;
    typedef ERaFromZigbee< ERaZigbee<Api> > FromZigbee;
    typedef ERaDBZigbee< ERaZigbee<Api> > DBZigbee;

public:
    ERaZigbee()
        : messageHandle(NULL)
        , initialized(false)
        , timerPing()
        , timerJoin()
        , device(InfoDevice_t::instance())
        , coordinator(InfoCoordinator_t::instance())
        , stream(NULL)
        , mutexData(NULL)
        , _zigbeeTask(NULL)
        , _controlZigbeeTask(NULL)
        , _responseZigbeeTask(NULL)
    {}
    ~ERaZigbee()
    {}

    void setZigbeeStream(Stream& _stream) {
        this->stream = &_stream;
    }

protected:
    void begin() { 
        this->configZigbee();
        if ((ZigbeeState::is(ZigbeeStateT::STATE_ZB_IGNORE)) ||
            (ToZigbee::CommandZigbee::pingSystem(5, 1000, ERaWatchdogFeed) != ResultT::RESULT_SUCCESSFUL)) {
            this->serialEnd();
            ZigbeeState::set(ZigbeeStateT::STATE_ZB_IGNORE);
            return;
        }
        InfoDevice_t::getInstance();
        InfoCoordinator_t::getInstance();
        this->timerPing = this->timer.setInterval(PING_INTERVAL, [=](void* args) {
            this->zigbeeTimerCallback(args);
        }, &this->timerPing);
        this->initZigbeeTask();
    }

    void run() {
        if ((this->device == nullptr) ||
            (this->coordinator == nullptr)) {
            return;
        }

        this->runEvent(false);
        this->runResponse(false);
        this->runControl(false);
        ERA_ZIGBEE_YIELD();
    }

    void runEvent(bool forever = true) {
        for (;;) {
            switch (ZigbeeState::get()) {
                case ZigbeeStateT::STATE_ZB_INIT_SUCCESSFUL:
                    ZigbeeState::set(ZigbeeStateT::STATE_ZB_RUNNING);
                    break;
                case ZigbeeStateT::STATE_ZB_DEVICE_JOINED:
                    if (this->timerJoined == false) {
                        this->timerJoined = this->timer.setTimeout(JOIN_TIMEOUT, [=](void* args) {
                            this->zigbeeTimerCallback(args);
                        }, &this->timerJoined);
                    }
                case ZigbeeStateT::STATE_ZB_RUNNING:
                case ZigbeeStateT::STATE_ZB_PERMIT_JOIN:
                    this->handleZigbeeData();
                    break;
                case ZigbeeStateT::STATE_ZB_DEVICE_INTERVIEWING:
                    if (this->interviewDevice()) {
                        this->permitJoinDuration(this->coordinator->permitJoin.address, 0);
                        this->readDataDevice();
                        ZigbeeState::set(ZigbeeStateT::STATE_ZB_RUNNING);
                    }
                    else {
                        ZigbeeState::set(ZigbeeStateT::STATE_ZB_PERMIT_JOIN);
                    }
                    break;
                case ZigbeeStateT::STATE_ZB_FACTORY_RESET:
                    this->factoryResetZigbee();
                    break;
                case ZigbeeStateT::STATE_ZB_IGNORE:
                case ZigbeeStateT::STATE_ZB_INIT_FAIL:
                case ZigbeeStateT::STATE_ZB_INIT_FORMAT:
                case ZigbeeStateT::STATE_ZB_INIT_MAX:
                default:
                    this->initZigbee(false);
                    break;
            }
            this->timer.run();
            if (!forever) {
                break;
            }
            ERA_ZIGBEE_YIELD();
        }
    }

    void runControl(bool forever = true) {
        for (;;) {
            switch (ZigbeeState::get()) {
                case ZigbeeStateT::STATE_ZB_DEVICE_INTERVIEWING:
                    this->timer.run();
                case ZigbeeStateT::STATE_ZB_FACTORY_RESET:
                case ZigbeeStateT::STATE_ZB_IGNORE:
                case ZigbeeStateT::STATE_ZB_INIT_FAIL:
                case ZigbeeStateT::STATE_ZB_INIT_FORMAT:
                case ZigbeeStateT::STATE_ZB_INIT_MAX:
                    break;
                default:
                    this->getZigbeeAction();
                    break;
            }
            if (!forever) {
                break;
            }
            ERA_ZIGBEE_YIELD();
        }
    }

    void runResponse(bool forever = true) {
        for (;;) {
            switch (ZigbeeState::get()) {
                case ZigbeeStateT::STATE_ZB_FACTORY_RESET:
                case ZigbeeStateT::STATE_ZB_IGNORE:
                case ZigbeeStateT::STATE_ZB_INIT_FAIL:
                case ZigbeeStateT::STATE_ZB_INIT_FORMAT:
                case ZigbeeStateT::STATE_ZB_INIT_MAX:
                    break;
                default:
                    this->handleDefaultResponse();
                    break;
            }
            if (!forever) {
                break;
            }
            ERA_ZIGBEE_YIELD();
        }
    }

    void initZigbeeTask();

#if defined(LINUX)
    static void* zigbeeTask(void* args);
    static void* controlZigbeeTask(void* args);
    static void* responseZigbeeTask(void* args);
#else
    static void zigbeeTask(void* args);
    static void controlZigbeeTask(void* args);
    static void responseZigbeeTask(void* args);
#endif

    cJSON* findDevicePayload(const char* topic);
    bool addZigbeeAction(const ZigbeeActionT type, const char* ieeeAddr, cJSON* const payload);

private:
    void configZigbee();
    void serialEnd();
    void initZigbee(bool format, bool invalid = false);
    void startZigbee(bool& format, bool& invalid);
    void factoryResetZigbee();
    void handleZigbeeData();
    void handleDefaultResponse();
    bool processZigbee(uint8_t* buffer,
                        int length,
                        int maxLength,
                        uint8_t* payload,
                        uint8_t& index,
                        uint8_t zStackLength,
                        uint8_t* cmdStatus = nullptr,
                        Response_t* rspWait = nullptr,
                        void* value = nullptr);
    bool interviewDevice();
    void removeDevice(const cJSON* const root, AFAddrType_t& dstAddr);
    void removeDeviceWithAddr(AFAddrType_t& dstAddr);
    void pingCoordinator();

    template <int size>
    void generateNetworkKey(uint8_t(&nwkKey)[size]);

    void createInfoCoordinator();
    ResultT permitJoinDuration(AFAddrType_t& dstAddr, uint8_t seconds);
    ResultT readAttrDevice(AFAddrType_t& dstAddr,
                            ClusterIDT zclId,
                            vector<uint16_t> attributes,
                            size_t limit = 2,
                            void* value = nullptr);
    ResultT writeAttrDevice(AFAddrType_t& dstAddr,
                            ClusterIDT zclId,
                            vector<DataWriteAttr_t> data,
                            size_t limit = 2);
    void readDataDevice();

    template <int inSize, int outSize>
    bool isClusterExist(const ClusterIDT(&inZcl)[inSize], const ClusterIDT(&outZcl)[outSize], const ClusterIDT zclId);
    template <int size>
    const ConfigBindReport_t* isBindReportExist(const ConfigBindReport_t(&inConfig)[size], const ClusterIDT zclId);
    template <typename T, int size>
    bool isElementExist(const T(&elementList)[size], const T element);
    template <typename T>
    bool isElementExist(const std::vector<T>& elementList, const T element);

    void publishZigbeeData(const IdentDeviceAddr_t* deviceInfo, bool specific = false, bool retained = ZIGBEE_PUBLISH_RETAINED);
    void publishZigbeeData(const char* topic, cJSON* payload, bool specific = true, bool retained = ZIGBEE_PUBLISH_RETAINED);
    bool actionZigbee(const ZigbeeActionT type, const char* ieeeAddr, const cJSON* const payload);
    void getZigbeeAction();
    void zigbeeTimerCallback(void* args);

    void beginReadFromFlash(const char* filename, bool force = true) {
        this->thisApi().beginReadFromFlash(filename, force);
    }

    char* readLineFromFlash(bool force = true) {
        return this->thisApi().readLineFromFlash(force);
    }

    void endReadFromFlash(bool force = true) {
        this->thisApi().endReadFromFlash(force);
    }

    void beginWriteToFlash(const char* filename, bool force = true) {
        this->thisApi().beginWriteToFlash(filename, force);
    }

    void writeLineToFlash(const char* buf, bool force = true) {
        this->thisApi().writeLineToFlash(buf, force);
    }

    void endWriteToFlash(bool force = true) {
        this->thisApi().endWriteToFlash(force);
    }

    void writeDataToFlash(const char* filename, const char* buf, bool force = true) {
        this->thisApi().writeToFlash(filename, buf, force);
    }

    char* readDataFromFlash(const char* filename, bool force = true) {
        return this->thisApi().readFromFlash(filename, force);
    }

    void removeDataFromFlash(const char* filename, bool force = true) {
        this->thisApi().removeFromFlash(filename, force);
    }

    bool isRequest() {
        return this->queue.readable();
    }

    ZigbeeAction_t& getRequest() {
        return this->queue;
    }

    bool isResponse() {
        return this->queueRsp.readable();
    }

    Response_t& getResponse() {
        return this->queueRsp;
    }

    bool isDefaultRsp() {
        return this->queueDefaultRsp.readable();
    }

    DefaultRsp_t& getDefaultRsp() {
        return this->queueDefaultRsp;
    }

    inline
    const Api& thisApi() const {
        return static_cast<const Api&>(*this);
    }

    inline
    Api& thisApi() {
        return static_cast<Api&>(*this);
    }

    ERaQueue<ZigbeeAction_t, ZIGBEE_MAX_ACTION> queue;
    ERaQueue<Response_t, ZIGBEE_MAX_RESPONSE> queueRsp;
    ERaQueue<DefaultRsp_t, ZIGBEE_MAX_DEFAULT_RESPONSE> queueDefaultRsp;
    QueueMessage_t messageHandle;
    bool initialized;

    ERaTimer timer;
    ERaTimer::iterator timerPing;
    ERaTimer::iterator timerJoin;
    ERaTimer::iterator timerJoined;

    InfoDevice_t*& device;
    InfoCoordinator_t*& coordinator;
    Stream* stream;
    ERaMutex_t mutexData;
    TaskHandle_t _zigbeeTask;
    TaskHandle_t _controlZigbeeTask;
    TaskHandle_t _responseZigbeeTask;
};

template <class Api>
void ERaZigbee<Api>::initZigbee(bool format, bool invalid) {
    if (ZigbeeState::get() == ZigbeeStateT::STATE_ZB_INIT_FORMAT) {
        format = true;
    }
    do {
        switch (ZigbeeState::get()) {
            case ZigbeeStateT::STATE_ZB_INIT_SUCCESSFUL:
                break;
            default:
                this->startZigbee(format, invalid);
                break;
        }
        ERaDelay(1000);
    } while (ZigbeeState::get() != ZigbeeStateT::STATE_ZB_INIT_SUCCESSFUL);
}

template <class Api>
void ERaZigbee<Api>::handleDefaultResponse() {
    if (!this->isDefaultRsp()) {
        return;
    }
    DefaultRsp_t& rsp = this->getDefaultRsp();
    static SyncRsp_t syncRsp {};

    syncRsp = {
        .transId = rsp.transId,
        .frameCtrl = rsp.frameCtrl,
        .dstAddr = rsp.dstAddr,
        .dstEndpoint = rsp.dstEndpoint,
        .srcEndPoint = rsp.srcEndpoint,
        .zclId = rsp.zclId
    };

    switch (rsp.zclId) {
        default:
            if (ZigbeeState::get() == ZigbeeStateT::STATE_ZB_DEVICE_INTERVIEWING) {
                if (rsp.dstAddr != this->device->address.addr.nwkAddr) {
                    break;
                }
            }
            if (rsp.isFirst) {
                ToZigbee::CommandZigbee::defaultResponse(rsp, ZnpCommandStatusT::SUCCESS_STATUS);
            }
            break;
    }
}

template <class Api>
bool ERaZigbee<Api>::processZigbee(uint8_t* buffer,
                                    int length,
                                    int maxLength,
                                    uint8_t* payload,
                                    uint8_t& index,
                                    uint8_t zStackLength,
                                    uint8_t* cmdStatus,
                                    Response_t* rspWait,
                                    void* value) {
    if (!length) {
        return false;
    }
    Response_t rsp {0};
    for (int i = 0; i < length; ++i) {
        uint8_t b = buffer[i];

        if (index >= maxLength) {
            index = 0;
            continue;
        }
        if ((b == this->SOF) && !index) {
            payload[index++] = b;
            continue;
        }
        if (index == 1) {
            zStackLength = b;
        }
        if (index) {
            payload[index++] = b;
        }
        if (index && ((payload[this->PositionSOF] != this->SOF) ||
                    (zStackLength > this->MaxDataSize))) {
            index = 0;
            continue;
        }
        if (index == zStackLength + this->MinMessageLength) {
            if (payload[this->PositionSOF] == this->SOF) {
                ERaLogHex("ZB <<", payload, index);
                rsp = FromZigbee::fromZigbee(payload, value);
                if (rsp.type != TypeT::ERR) {
                    if (rspWait == nullptr) {
                        // sync
                        if (this->queueRsp.writeable()) {
                            this->queueRsp += rsp;
                        }
                    }
                    else {
                        if (cmdStatus != nullptr) {
                            if (CheckAFdata_t(rsp, *rspWait)) {
                                *cmdStatus = rsp.cmdStatus;
                            }
                            if (CompareRsp_t(rsp, *rspWait)) {
                                return true;
                            }
                            if (CheckRsp_t(rsp, *rspWait)) {
                                // sync
                                if (this->queueRsp.writeable()) {
                                    this->queueRsp += rsp;
                                }
                            }
                        }
                    }
                }
            }
            index = 0;
            zStackLength = 0;
            memset(payload, 0, maxLength);
        }
    }
    if (index && (payload[this->PositionSOF] != this->SOF)) {
        index = 0;
    }
    return false;
}

template <class Api>
cJSON* ERaZigbee<Api>::findDevicePayload(const char* topic) {
    if (topic == nullptr) {
        return nullptr;
    }
    if (this->coordinator == nullptr) {
        return nullptr;
    }
    IdentDeviceAddr_t* deviceInfo = std::find_if(std::begin(this->coordinator->deviceIdent), std::end(this->coordinator->deviceIdent),
                                                find_devicePayloadWithTopic_t(topic));
    if (deviceInfo == std::end(this->coordinator->deviceIdent)) {
        return nullptr;
    }
    return deviceInfo->data.payload;
}

template <class Api>
void ERaZigbee<Api>::publishZigbeeData(const IdentDeviceAddr_t* deviceInfo, bool specific, bool retained) {
    if ((deviceInfo == nullptr) ||
        (deviceInfo == std::end(this->coordinator->deviceIdent))) {
        return;
    }
    if ((deviceInfo->data.topic == nullptr) ||
        (deviceInfo->data.payload == nullptr)) {
        return;
    }
    this->thisApi().zigbeeDataWrite(deviceInfo->data.topic, deviceInfo->data.payload, specific, retained);
}

template <class Api>
void ERaZigbee<Api>::publishZigbeeData(const char* topic, cJSON* payload, bool specific, bool retained) {
    if ((topic == nullptr) ||
        (payload == nullptr)) {
        return;
    }
    this->thisApi().zigbeeDataWrite(topic, payload, specific, retained);
}

template <class Api>
bool ERaZigbee<Api>::actionZigbee(const ZigbeeActionT type, const char* ieeeAddr, const cJSON* const payload) {
    if (ieeeAddr == nullptr || payload == nullptr) {
        return false;
    }

    AFAddrType_t dstAddr {0};

    switch (type) {
        case ZigbeeActionT::ZIGBEE_ACTION_SET:
        case ZigbeeActionT::ZIGBEE_ACTION_GET:
            if (CompareString(ieeeAddr, "group")) {
                dstAddr.addrMode = AddressModeT::ADDR_GROUP;
            }
            else {
                dstAddr.addrMode = AddressModeT::ADDR_16BIT;
                StringToIEEE(ieeeAddr, dstAddr.addr.ieeeAddr);
            }
            ToZigbee::toZigbee(payload, dstAddr, ((type == ZigbeeActionT::ZIGBEE_ACTION_SET) 
                                                ? ToZigbee::ConvertToZigbeeT::CONVERT_SET_TO_ZIGBEE
                                                : ToZigbee::ConvertToZigbeeT::CONVERT_GET_TO_ZIGBEE));
            break;
        case ZigbeeActionT::ZIGBEE_ACTION_PERMIT_JOIN:
            ToZigbee::permitJoinToZigbee(payload);
            break;
        case ZigbeeActionT::ZIGBEE_ACTION_REMOVE_DEVICE:
            this->removeDevice(payload, dstAddr);
            break;
        default:
            break;
    }

    return true;
}

template <class Api>
void ERaZigbee<Api>::getZigbeeAction() {
    if (!this->isRequest()) {
        return;
    }
    ZigbeeAction_t& req = this->getRequest();
    if (req.ieeeAddr == nullptr) {
        return;
    }
    if (req.payload == nullptr) {
        free(req.ieeeAddr);
        req.ieeeAddr = nullptr;
        return;
    }
    this->actionZigbee(req.type, req.ieeeAddr, req.payload);
    free(req.ieeeAddr);
    cJSON_Delete(req.payload);
    req.ieeeAddr = nullptr;
    req.payload = nullptr;
}

template <class Api>
bool ERaZigbee<Api>::addZigbeeAction(const ZigbeeActionT type, const char* ieeeAddr, cJSON* const payload) {
    if (ieeeAddr == nullptr || payload == nullptr) {
        return false;
    }
    if (!this->queue.writeable()) {
        return false;
    }
    char* buf = (char*)ERA_MALLOC(strlen(ieeeAddr) + 1);
    if (buf == nullptr) {
        return false;
    }
    ZigbeeAction_t req;
    req.type = type;
    req.ieeeAddr = buf;
    req.payload = payload;
    memset(req.ieeeAddr, 0, strlen(ieeeAddr) + 1);
    strcpy(req.ieeeAddr, ieeeAddr);
    this->queue += req;
    return true;
}

template <class S, typename... Args>
class SerialZigbee {
public:
    static S& serial(Args... tail) {
        static S _serial(tail...);
        return _serial;
    }
};

#include "adapter/ERaReportingZigbee.hpp"
#include "adapter/ERaEndpointsZigbee.hpp"
#include "adapter/ERaAdapterZigbee.hpp"
#include "adapter/ERaDeviceZigbee.hpp"

#endif /* INC_ERA_ZIGBEE_HPP_ */
