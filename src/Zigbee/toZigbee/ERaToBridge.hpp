#ifndef INC_ERA_TO_BRIDGE_ZIGBEE_HPP_
#define INC_ERA_TO_BRIDGE_ZIGBEE_HPP_

#include <Zigbee/ERaToZigbee.hpp>

template <class Zigbee>
bool ERaToZigbee<Zigbee>::permitJoinToZigbee(const cJSON* const root) {
    AFAddrType_t dstAddr {
        .addr = {
            .nwkAddr = CommandZigbee::AddrBroadcastJoin
        },
        .addrMode = AddressModeT::ADDR_BROADCAST
    };
    ResultT status {ResultT::RESULT_SUCCESSFUL};

    cJSON* item = cJSON_GetObjectItem(root, "ieee_addr");
    if (cJSON_IsString(item)) {
        StringToIEEE(item->valuestring, dstAddr.addr.ieeeAddr);
        this->findDeviceInfoWithIEEEAddr(dstAddr);
    }
    item = cJSON_GetObjectItem(root, "nwk_addr");
    if (cJSON_IsNumber(item)) {
        dstAddr.addr.nwkAddr = item->valueint;
        dstAddr.addrMode = AddressModeT::ADDR_16BIT;
    }
    item = cJSON_GetObjectItem(root, "value");
    if (!cJSON_IsBool(item)) {
        return false;
    }
    if (item->valueint) {
        status = this->thisZigbee().permitJoinDuration(dstAddr, 254);
    }
    else {
        status = this->thisZigbee().permitJoinDuration(dstAddr, 0);
    }
    this->createBridgeDataZigbee("permit_join", status, root);
    return true;
}

template <class Zigbee>
void ERaToZigbee<Zigbee>::createBridgeDataZigbee(const char* subTopic, ResultT status, const cJSON* const item) {
    char topic[MAX_TOPIC_LENGTH] {0};
    cJSON* root = cJSON_CreateObject();
    if (root == nullptr) {
        return;
    }

    FormatString(topic, TOPIC_ZIGBEE_BRIDGE_RESPONSE, subTopic);
    cJSON_AddStringToObject(root, "type", "data_response");
    cJSON_AddItemToObject(root, "data", cJSON_Duplicate(item, true));
    cJSON_AddStringToObject(root, "status", ((status == ResultT::RESULT_SUCCESSFUL) ? "ok" : "error"));

    this->thisZigbee().publishZigbeeData(topic, root, true, false);

    cJSON_Delete(root);
    root = nullptr;
}

#endif /* INC_ERA_TO_BRIDGE_ZIGBEE_HPP_ */
