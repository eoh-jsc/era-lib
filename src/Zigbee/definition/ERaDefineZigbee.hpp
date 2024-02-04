#ifndef INC_ERA_DEFINE_ZIGBEE_HPP_
#define INC_ERA_DEFINE_ZIGBEE_HPP_

#include <stdint.h>
#include "define/afZigbee.hpp"
#include "define/appZigbee.hpp"
#include "define/channelZigbee.hpp"
#include "define/clusterZigbee.hpp"
#include "define/commonZigbee.hpp"
#include "define/dataTypeZigbee.hpp"
#include "define/deviceZigbee.hpp"
#include "define/directionZigbee.hpp"
#include "define/endpointZigbee.hpp"
#include "define/foundationZigbee.hpp"
#include "define/frameTypeZigbee.hpp"
#include "define/macZigbee.hpp"
#include "define/manufZigbee.hpp"
#include "define/otherZigbee.hpp"
#include "define/statusZigbee.hpp"
#include "define/sysZigbee.hpp"
#include "define/typeZigbee.hpp"
#include "define/utilZigbee.hpp"
#include "define/zbZigbee.hpp"
#include "define/zdoZigbee.hpp"

#if !defined(MAX_DEVICE_ZIGBEE)
    #define MAX_DEVICE_ZIGBEE           20
#endif

#if !defined(DISABLE_SCALE_ZIGBEE_DATA)
    #define ENABLE_SCALE_ZIGBEE_DATA
#endif

#if defined(ZIGBEE_PUBLISH_RETAINED)
    /* OK, use the specified value */
#elif defined(ERA_MQTT_PUBLISH_RETAINED)
    #define ZIGBEE_PUBLISH_RETAINED     ERA_MQTT_PUBLISH_RETAINED
#else
    #define ZIGBEE_PUBLISH_RETAINED     true
#endif

#define NO_NWK_ADDR                     (uint16_t)0x0000
#define NO_LOAD_ENDPOINT                (uint8_t)0xFF
#define ZCL_DATA_MIN                    3
#define MAX_TIMEOUT                     5000
#define DEFAULT_TIMEOUT                 2000
#define RESPONSE_TIMEOUT                1000
#define MIN_TIMEOUT                     500
#define NWK_ADDR_BROADCAST              (uint16_t)0xFFFF
#define NWK_ADDR_COORDINATOR            (uint16_t)0x0000
#define NWK_ADDR_COORDINATOR_INIT       (uint16_t)0xFFFE
#define NWK_ADDR_BROADCAST_JOIN         (uint16_t)0xFFFC
#define NWK_ADDR_BROADCAST_GP           (uint16_t)0xFFFD

#define PERMIT_JOIN_INTERVAL            200 * 1000
#define PING_INTERVAL                   60 * 60 * 1000
#define JOIN_TIMEOUT                    60 * 1000

#define LENGTH_EXTADDR_IEEE             8
#define LENGTH_NETWORK_KEY              16
#define LENGTH_BUFFER                   50

#define TRANSMIT_POWER_CC2652P_DEFAULT  (uint8_t)0x09
#define TRANSMIT_POWER_CC2652P_MAX      (uint8_t)0x14

/* Takes a byte out of a uint32 : var - uint32,  ByteNum - byte to take out (0 - 3) */
#define BREAK_UINT32(var, ByteNum) \
        ((uint8_t)((uint32_t)(((var) >> ((ByteNum) * 8)) & 0x00FF)))

#define BUILD_UINT8(hiByte, loByte) \
        ((uint8_t)(((loByte) & 0x0F) + (((hiByte) & 0x0F) << 4)))
#define BUILD_UINT16(fiByte) ((uint16_t)(*reinterpret_cast<uint16_t*>(&fiByte)))
#define BUILD_UINT24(fiByte) (((uint32_t)(*reinterpret_cast<uint32_t*>(&fiByte))) & 0x00FFFFFF)
#define BUILD_UINT32(fiByte) ((uint32_t)(*reinterpret_cast<uint32_t*>(&fiByte)))
#define BUILD_UINT40(fiByte) (((uint64_t)(*reinterpret_cast<uint64_t*>(&fiByte))) & 0xFFFFFFFFFF)
#define BUILD_UINT48(fiByte) (((uint64_t)(*reinterpret_cast<uint64_t*>(&fiByte))) & 0xFFFFFFFFFFFF)
#define BUILD_UINT52(fiByte) (((uint64_t)(*reinterpret_cast<uint64_t*>(&fiByte))) & 0xFFFFFFFFFFFFFF)
#define BUILD_UINT64(fiByte) ((uint64_t)(*reinterpret_cast<uint64_t*>(&fiByte)))
#define BUILD_FLOAT(value)   ((float)(*reinterpret_cast<float*>(&value)))
#define BUILD_DOUBLE(value)  ((double)(*reinterpret_cast<double*>(&value)))

#define HI_UINT16(a) (((a) >> 8) & 0xFF)
#define LO_UINT16(a) ((a) & 0xFF)
#define HI_UINT8(a) (((a) >> 4) & 0x0F)
#define LO_UINT8(a) ((a) & 0x0F)

#define STRINGIFYING_ZCL(key)       StringifyZcl_t({ZCL_CLUSTER_##key, #key})
#define STRINGIFYING_DATATYPE(key)  StringifyDataType_t({zcl_##key, #key})

namespace ZigbeeNamespace {

    typedef struct __StringifyZcl_t {
        enum ClusterIDT zclId;
        const char* zclName;
    } StringifyZcl_t;

    typedef struct __StringifyDataType_t {
        enum DataTypeT type;
        const char* typeName;
    } StringifyDataType_t;

    typedef struct __KeyDataZigbee_t {
        enum ClusterIDT zclId;
        uint8_t numKeyIn;
        const char* keyInZcl[10];
        uint8_t numKeyOut;
        const char* keyOutZcl[10];
    } KeyDataZigbee_t;

    typedef struct __ScaleDataModel_t {
        const char* modelName;
        float scale;
        uint32_t divide;
    } ScaleDataModel_t;

    typedef struct __ScaleDataZigbee_t {
        const char* key;
        float defaultScale;
        uint32_t defaultDivide;
        uint8_t numScale;
        ScaleDataModel_t scaleModel[10];
    } ScaleDataZigbee_t;

    typedef struct __ConfigReport_t {
        uint16_t attribute;
        enum DataTypeT dataType;
        uint16_t minInterval;
        uint16_t maxInterval;
        uint64_t minChange;
    } ConfigReport_t;

    typedef struct __ConfigBindReport_t {
        enum ClusterIDT zclId;
        uint8_t numReport;
        ConfigReport_t report[20];
        enum ManufacturerCodesT manufCode;
        uint8_t numReadAttr;
        uint16_t attribute[20];
        uint8_t numWriteAttr;
        struct {
            uint16_t attribute;
            enum DataTypeT type;
            uint8_t length;
            uint8_t data[20];
        } writeAttribute[20];
        
    } ConfigBindReport_t;

    typedef struct __Response_t {
        uint16_t nwkAddr;
        uint8_t type;
        uint8_t subSystem;
        uint8_t command;
        enum ClusterIDT zclId;
        uint8_t transId;
        uint8_t transIdZcl;
        uint8_t cmdStatus;
        uint32_t timeout;
    } Response_t;

    typedef struct __InitEndpoint_t
    {
        enum EndpointListT endpoint;
        enum ProfileIDT profileId;
        enum DeviceIDT deviceId;
        uint8_t appDevVer;
        enum NetworkLatencyReqT latency;
        uint8_t numZclIn;
        enum ClusterIDT zclIdIn[10];
        uint8_t numZclOut;
        enum ClusterIDT zclIdOut[10];
    } InitEndpoint_t;

    typedef struct __CommandsZcl_t {
        enum ClusterIDT zclId;
        uint8_t command;
        vector<uint8_t>* data;
    } CommandsZcl_t;

    typedef struct __HeaderZclFrame_t {
        enum FrameTypeT frameType;
        uint16_t manufCode;
        enum DirectionT direction;
        bool disableRsp;
        uint8_t reservedBits;
    } HeaderZclFrame_t;

    typedef struct __DataAttr_t
    {
        uint16_t attribute;
        void* value;
    } DataAttr_t;

    typedef struct __DataWriteAttr_t
    {
        uint16_t attribute;
        enum DataTypeT type;
        vector<uint8_t> data;
    } DataWriteAttr_t;

    typedef struct __DataReadAttrRsp_t
    {
        uint16_t attribute;
        enum DataTypeT type;
        vector<uint8_t> data;
        bool found;
    } DataReadAttrRsp_t;

    typedef struct __DefaultRsp_t
    {
        __DefaultRsp_t()
            : isFirst(true)
            , isSent(false)
            , dstAddr(NWK_ADDR_BROADCAST)
            , dstEndpoint(EndpointListT::ENDPOINT_NONE)
            , zclId(ClusterIDT::NO_CLUSTER_ID)
            , manufCode(ManufacturerCodesT::MANUF_CODE_NONE)
            , transId(0)
            , isSpecific(false)
            , hasResponse(false)
            , attribute(0)
            , value(0)
            , isCmd(false)
        {}

        bool isFirst;
        bool isSent;
        uint16_t dstAddr;
        enum EndpointListT dstEndpoint;
        enum EndpointListT srcEndpoint;
        enum ClusterIDT zclId;
        enum ManufacturerCodesT manufCode;
        uint8_t transId;
        uint8_t frameCtrl;
        uint8_t isSpecific;
        uint8_t cmdId;
        bool supportOTA;
        bool hasResponse;
        uint16_t attribute;
        uint64_t value;
        bool isCmd;
    } DefaultRsp_t;

    typedef struct __SyncRsp_t
    {
        uint8_t transId;
        uint8_t frameCtrl;
        uint16_t dstAddr;
        enum EndpointListT dstEndpoint;
        enum EndpointListT srcEndPoint;
        enum ClusterIDT zclId;
    } SyncRsp_t;

    typedef struct __NewCluster_t
    {
        enum ClusterIDT zclId;
        enum EndpointListT srcEndpoint;
    } NewCluster_t;

    typedef struct __AFAddrType_t {
        struct {
            uint16_t nwkAddr;
            uint8_t ieeeAddr[LENGTH_EXTADDR_IEEE];
        } addr;
        enum AddressModeT addrMode;
        enum EndpointListT endpoint;
        uint16_t panId;
    } AFAddrType_t;

    typedef struct __ZigbeeData_t {
        char* topic;
        cJSON* payload;
    } ZigbeeData_t;

    typedef struct __ZigbeeAction_t {
        enum ZigbeeActionT type;
        char* ieeeAddr;
        cJSON* payload;
    } ZigbeeAction_t;

    typedef struct __IdentDeviceAddr_t {
        uint8_t typeDevice;
        AFAddrType_t address;
        uint8_t appVer;
        bool isConnected;
        char modelName[50];
        ZigbeeData_t data;
        uint8_t receiveId;
    } IdentDeviceAddr_t;

    typedef struct __DataAFMsg_t {
        uint16_t groupId;
        enum ClusterIDT zclId;
        AFAddrType_t srcAddr;
        enum EndpointListT dstEndpoint;
        uint8_t wasBroadcast;
        uint8_t linkQuality;
        uint8_t securityUse;
        uint32_t timestamp;
        uint8_t seqNum;
        uint8_t len;
        uint8_t frameCtrl;
        enum ManufacturerCodesT manufCode;
        uint8_t receiveId;
        uint8_t cmdId;
        uint8_t pDataLen;
        uint8_t* pData;
        uint16_t parentAddr;
        uint8_t radius;
        uint8_t ddr;
        const IdentDeviceAddr_t* deviceInfo;
    } DataAFMsg_t;

    typedef struct __PermitJoin_t {
        __PermitJoin_t()
            : enable(false)
            , installCode(false)
            , address {
                .addr = {
                    .nwkAddr = NWK_ADDR_BROADCAST_JOIN
                },
                .addrMode = AddressModeT::ADDR_BROADCAST
            }
        {}

        bool enable;
        bool installCode;
        AFAddrType_t address;
    } PermitJoin_t;

    typedef struct __InfoEndpoint_t {
        enum EndpointListT endpoint;
        enum ProfileIDT profileId;
        enum DeviceIDT deviceId;
        uint8_t deviceVer;
        uint8_t inZclCount;
        enum ClusterIDT inZclIdList[20];
        uint8_t outZclCount;
        enum ClusterIDT outZclIdList[20];
        bool isBasic;
        bool pollControl;
        bool ias;
    } InfoEndpoint_t;

    typedef struct __NwkKeyDesc_t {
        uint8_t keySeqNum;
        uint8_t key[LENGTH_NETWORK_KEY];
    } NwkKeyDesc_t;

    typedef struct __ApsTcLinkKey_t {
        uint32_t txFrmCntr;
        uint32_t rxFrmCntr;
        uint8_t extAddr[LENGTH_EXTADDR_IEEE];
        uint8_t keyAttr;
        uint8_t keyType;
        uint8_t seedShiftIcIndex;
    } ApsTcLinkKey_t;

    typedef struct __NwkIBInfo_t {
        uint8_t sequenceNum;
        uint8_t passiveAckTimeout;
        uint8_t maxBroadcastRetries;
        uint8_t maxChildren;
        uint8_t maxDepth;
        uint8_t maxRouters;
        uint8_t dummyNeighborTable;
        uint8_t broadcastDeliveryTime;
        uint8_t reportConstantCost;
        uint8_t routeDiscRetries;
        uint8_t dummyRoutingTable;
        uint8_t secureAllFrames;
        uint8_t securityLevel;
        uint8_t symLink;
        uint8_t capabilityFlags;
        uint16_t transactionPersistenceTime;
        uint8_t nwkProtocolVersion;
        uint8_t routeDiscoveryTime;
        uint8_t routeExpiryTime;
        uint16_t nwkDevAddr;
        uint8_t nwkLogicalChannel;
        uint16_t nwkCoordAddr;
        uint8_t nwkCoordExtAddr[LENGTH_EXTADDR_IEEE];
        uint16_t nwkPanId;
        uint8_t nwkState;
        enum ZBChannelT channelList;
        uint8_t beaconOrder;
        uint8_t superFrameOrder;
        uint8_t scanDuration;
        uint8_t battLifeExt;
        uint32_t allocatedRouterAddr;
        uint32_t allocatedEndDeviceAddr;
        uint8_t nodeDepth;
        uint8_t extPanId[LENGTH_EXTADDR_IEEE];
        uint8_t nwkKeyLoaded;
        NwkKeyDesc_t spare1;
        NwkKeyDesc_t spare2;
        uint8_t spare3;
        uint8_t spare4;
        uint8_t nwkLinkStatusPeriod;
        uint8_t nwkRouterAgeLimit;
        uint8_t nwkUseMultiCast;
        uint8_t nwkIsConcentrator;
        uint8_t nwkConcentratorDiscoveryTime;
        uint8_t nwkConcentratorRadius;
        uint8_t nwkAllFresh;
        uint16_t nwkManagerAddr;
        uint16_t nwkTotalTransmissions;
        uint8_t nwkUpdateId;
    } NwkIBInfo_t;

    typedef struct __NwkMaterialDesc_t {
        uint32_t frameCounter;
        uint8_t extPanId[LENGTH_EXTADDR_IEEE];
    } NwkMaterialDesc_t;

    typedef struct __ApsLinkKey_t {
        uint8_t key[LENGTH_NETWORK_KEY];
        uint32_t txFrmCntr;
        uint32_t rxFrmCntr;
    } ApsLinkKey_t;

    typedef struct __ExtNwkInfo_t {
        uint16_t shortAddr;
        enum DevStatesT states;
        uint16_t panId;
        uint16_t parentAddr;
        uint8_t extPanId[LENGTH_EXTADDR_IEEE];
        uint8_t extParentAddr[LENGTH_EXTADDR_IEEE];
        uint8_t channel;
    } ExtNwkInfo_t;

    typedef struct __InfoCoordinator_t {
        __InfoCoordinator_t()
            : factoryReset(false)
            , regInterpan(false)
            , transmitPower(TRANSMIT_POWER_CC2652P_DEFAULT)
            , address {
                .addr = {
                        .nwkAddr = NWK_ADDR_COORDINATOR_INIT
                },
                .addrMode = AddressModeT::ADDR_64BIT,
                .endpoint = ENDPOINT1,
                .panId = 0x0001
            }
            , channel(ZBChannelT::NO_LOAD_CHANNEL)
            , epCount(NO_LOAD_ENDPOINT)
            , epTick(0)
        {}

        static __InfoCoordinator_t*& instance() {
            static __InfoCoordinator_t* _instance = nullptr;
            return _instance;
        }
        static __InfoCoordinator_t* getInstance() {
            if (__InfoCoordinator_t::instance() == nullptr) {
                void* ptr = ERA_CALLOC(1, sizeof(__InfoCoordinator_t));
                __InfoCoordinator_t::instance() = new(ptr) __InfoCoordinator_t();
            }
            return __InfoCoordinator_t::instance();
        }
        void reset(__InfoCoordinator_t*& _instance = __InfoCoordinator_t::instance()) {
            _instance->freeAllDevice();
            memset((void*)_instance, 0, sizeof(__InfoCoordinator_t));
            new(_instance) __InfoCoordinator_t();
        }
        void freeAllDevice() {
            for (size_t i = 0; i < this->deviceCount; ++i) {
                if (this->deviceIdent[i].data.topic != nullptr) {
                    free(this->deviceIdent[i].data.topic);
                    this->deviceIdent[i].data.topic = nullptr;
                }
                if (this->deviceIdent[i].data.payload != nullptr) {
                    cJSON_Delete(this->deviceIdent[i].data.payload);
                    this->deviceIdent[i].data.payload = nullptr;
                }
            }
        }
        void clearAllDevice() {
            this->freeAllDevice();
            this->deviceCount = 0;
            memset(this->deviceIdent, 0, sizeof(this->deviceIdent));
        }

        bool lock;
        bool factoryReset;
        PermitJoin_t permitJoin;
        bool regInterpan;
        uint8_t transmitPower;
        uint8_t deviceType;
        enum DeviceStateListT deviceState;
        AFAddrType_t address;
        uint8_t extAddr[LENGTH_EXTADDR_IEEE];
        enum ManufacturerCodesT manufCode;
        char manufName[50];
        enum ZBChannelT channel;
        uint8_t networkKey[LENGTH_NETWORK_KEY];
        bool enableKey;
        uint8_t extPanId[LENGTH_EXTADDR_IEEE];
        enum DevStatesT states;
        uint8_t epCount;
        uint8_t epTick;
        InfoEndpoint_t epList[20];
        uint16_t extGroup;
        uint8_t deviceCount;
        IdentDeviceAddr_t deviceIdent[MAX_DEVICE_ZIGBEE];
        NwkKeyDesc_t activeKeyDesc;
        NwkKeyDesc_t alternKeyDesc;
        uint8_t apsExtPanId[LENGTH_EXTADDR_IEEE];
        ApsTcLinkKey_t tcLinkKey;
        NwkIBInfo_t nIB;
        NwkMaterialDesc_t materialDesc;
        ApsLinkKey_t linkKey;
        uint8_t hasConfigured;
        ExtNwkInfo_t nwkInfo;
        enum ReasonResetZigbeeT reasonRst;
        uint8_t transportRev;
        ZnpVersionT product;
        uint8_t majorRel;
        uint8_t minorRel;
        uint8_t maintRel;
        uint8_t hwRev;
        NvItemsIdsT nvItem;
        NvItemsIdsT nvItemSys;
        uint32_t revision;
        enum APPCFCommStatusT commStatus;
        enum FormatDataT formatData;
        bool memAligned;
        bool supportsLed;
        bool supportsAssocAdd;
        bool supportsAssocRemove;
    } InfoCoordinator_t;

    typedef struct __AnnceDevice_t
    {
        AFAddrType_t dstAddr;
        bool alternatePanId;
        enum TypeAnnceDeviceT type;
        enum PowerSourceT power;
        bool isIdle;
    } AnnceDevice_t;

    typedef struct __GetValueAttr_t
    {
        enum ClusterIDT zclId;
        uint16_t attribute;
        enum DataTypeT type;
        uint64_t value;
    } GetValueAttr_t;

    typedef struct __InfoDevice_t
    {
        __InfoDevice_t() 
            : isJoing(false)
            , isFailed(false)
            , hasAnnounce(false)
            , address {
                .addr = {},
                .addrMode = AddressModeT::ADDR_16BIT
            }
            , hasManufCode(false)
            , power(PWS_NEED_GET)
        {}

        static __InfoDevice_t*& instance() {
            static __InfoDevice_t* _instance = nullptr;
            return _instance;
        }
        static __InfoDevice_t* getInstance() {
            if (__InfoDevice_t::instance() == nullptr) {
                void* ptr = ERA_CALLOC(1, sizeof(__InfoDevice_t));
                __InfoDevice_t::instance() = new(ptr) __InfoDevice_t();
            }
            return __InfoDevice_t::instance();
        }
        void reset(__InfoDevice_t*& _instance = __InfoDevice_t::instance()) {
            memset((void*)_instance, 0, sizeof(__InfoDevice_t));
            new(_instance) __InfoDevice_t();
        }

        bool isJoing;
        bool isFailed;
        bool hasAnnounce;
        bool readDone;
        uint8_t deviceType;
        enum DeviceStateListT deviceState;
        AFAddrType_t address;
        uint16_t parentAddr;
        enum DevStatesT states;
        bool hasManufCode;
        enum ManufacturerCodesT manufCode;
        char manufName[50];
        uint8_t epCount;
        uint8_t epTick;
        InfoEndpoint_t epList[20];
        enum TypeDeviceT typeDevice;
        enum PowerSourceT power;
        uint8_t appVer;
        uint8_t zclVer;
        uint8_t stackVer;
        uint8_t hwVer;
        uint16_t batVoltage;
        uint8_t batPercent;
        uint16_t revision;
        uint32_t time;
        bool enableDevice;
        char modelName[50];
        char dataCode[20];
        char swBuild[20];
        bool ias;
        bool pollControl;
        bool checkin;
        uint32_t checkinInterval;
        enum EndpointListT iasEp;
        enum EndpointListT basicEp;
        bool enRoll;
        uint8_t ieeeIAS[LENGTH_EXTADDR_IEEE];
        AnnceDevice_t annceDevice;
        GetValueAttr_t valueAttr;
    } InfoDevice_t;

};

#include "define/structZigbee.hpp"
#include <Zigbee/zCluster/ERaZclZigbee.hpp>

#define TOPIC_ZIGBEE_BRIDGE_INFO                "/zigbee/bridge/info"
#define TOPIC_ZIGBEE_BRIDGE_EVENT               "/zigbee/bridge/event"
#define TOPIC_ZIGBEE_BRIDGE_RESPONSE            "/zigbee/bridge/response/%s"
#define TOPIC_ZIGBEE_DEVICE_EVENT               "/zigbee/device"

using namespace ZigbeeNamespace;

#endif /* INC_ERA_DEFINE_ZIGBEE_HPP_ */
