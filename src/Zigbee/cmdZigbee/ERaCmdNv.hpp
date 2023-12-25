#ifndef INC_ERA_COMMAND_NV_ITEM_ZIGBEE_HPP_
#define INC_ERA_COMMAND_NV_ITEM_ZIGBEE_HPP_

#include <Zigbee/ERaCommandZigbee.hpp>

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::readItemZstack(NvItemsIdsT itemId, uint16_t offset, void* value) {
    this->coordinator->nvItem = itemId;

    uint16_t length {0};
    if (this->readOSALLength(itemId, &length) != ResultT::RESULT_SUCCESSFUL) {
        return ResultT::RESULT_FAIL;
    }
    if (!length) {
        return ResultT::RESULT_SUCCESSFUL;
    }
    return this->readOSALItemExt(itemId, offset, value);
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::readItemZstacks(NvItemsIdsT itemId, uint16_t offset, size_t limit, void* value) {
    for (size_t i = 0; i < limit; ++i) {
        if (this->readItemZstack(itemId, offset, value) == ResultT::RESULT_SUCCESSFUL) {
            return ResultT::RESULT_SUCCESSFUL;
        }
    }
    return ResultT::RESULT_FAIL;
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::writeItemZstack(NvItemsIdsT itemId, uint16_t offset, vector<uint8_t>& data) {
    uint16_t length {0};
    ZnpCommandStatusT status {ZnpCommandStatusT::SUCCESS_STATUS};
    if (this->readOSALLength(itemId, &length) != ResultT::RESULT_SUCCESSFUL) {
        return ResultT::RESULT_FAIL;
    }
    if (!length) {
        return ResultT::RESULT_SUCCESSFUL;
    }
    if (this->writeOSALItemExt(itemId, offset, data, &status) != ResultT::RESULT_SUCCESSFUL) {
        return ResultT::RESULT_FAIL;
    }
    return ((status == ZnpCommandStatusT::SUCCESS_STATUS) ? ResultT::RESULT_SUCCESSFUL : ResultT::RESULT_FAIL);
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::updateItemZstack(NvItemsIdsT itemId, uint16_t offset, vector<uint8_t>& data, void* value) {
    return ResultT::RESULT_FAIL;
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::deleteItemZstack(NvItemsIdsT itemId) {
    uint16_t length {0};
    if (this->readOSALLength(itemId, &length) != ResultT::RESULT_SUCCESSFUL) {
        return ResultT::RESULT_FAIL;
    }
    if (!length) {
        return ResultT::RESULT_SUCCESSFUL;
    }
    return this->deleteOSALItem(itemId, length);
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::readOSALLength(NvItemsIdsT itemId, void* value) {
    vector<uint8_t> payload;
    payload.push_back(LO_UINT16(itemId));
    payload.push_back(HI_UINT16(itemId));
    return this->thisToZigbee().createCommandBuffer(payload, TypeT::SREQ,
                                                    SubsystemT::SYS_INTER, SYSCommandsT::SYS_OSAL_NV_LENGTH,
                                                    TypeT::SRSP, SYSCommandsT::SYS_OSAL_NV_LENGTH,
                                                    value);
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::readOSALItem(NvItemsIdsT itemId, uint8_t offset, void* value) {
    vector<uint8_t> payload;
    payload.push_back(LO_UINT16(itemId));
    payload.push_back(HI_UINT16(itemId));
    payload.push_back(offset);
    return this->thisToZigbee().createCommandBuffer(payload, TypeT::SREQ,
                                                    SubsystemT::SYS_INTER, SYSCommandsT::SYS_OSAL_NV_READ,
                                                    TypeT::SRSP, SYSCommandsT::SYS_OSAL_NV_READ,
                                                    value);
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::readOSALItemExt(NvItemsIdsT itemId, uint16_t offset, void* value) {
    vector<uint8_t> payload;
    payload.push_back(LO_UINT16(itemId));
    payload.push_back(HI_UINT16(itemId));
    payload.push_back(LO_UINT16(offset));
    payload.push_back(HI_UINT16(offset));
    return this->thisToZigbee().createCommandBuffer(payload, TypeT::SREQ,
                                                    SubsystemT::SYS_INTER, SYSCommandsT::SYS_OSAL_NV_READ_EXT,
                                                    TypeT::SRSP, SYSCommandsT::SYS_OSAL_NV_READ_EXT,
                                                    value);
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::writeOSALItem(NvItemsIdsT itemId, uint8_t offset, vector<uint8_t>& data, void* value) {
    vector<uint8_t> payload;
    payload.push_back(LO_UINT16(itemId));
    payload.push_back(HI_UINT16(itemId));
    payload.push_back(offset);
    payload.push_back(data.size());
    payload.insert(payload.end(), data.begin(), data.end());
    return this->thisToZigbee().createCommandBuffer(payload, TypeT::SREQ,
                                                    SubsystemT::SYS_INTER, SYSCommandsT::SYS_OSAL_NV_WRITE,
                                                    TypeT::SRSP, SYSCommandsT::SYS_OSAL_NV_WRITE,
                                                    value);
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::writeOSALItemExt(NvItemsIdsT itemId, uint16_t offset, vector<uint8_t>& data, void* value) {
    vector<uint8_t> payload;
    payload.push_back(LO_UINT16(itemId));
    payload.push_back(HI_UINT16(itemId));
    payload.push_back(LO_UINT16(offset));
    payload.push_back(HI_UINT16(offset));
    payload.push_back(LO_UINT16(data.size()));
    payload.push_back(HI_UINT16(data.size()));
    payload.insert(payload.end(), data.begin(), data.end());
    return this->thisToZigbee().createCommandBuffer(payload, TypeT::SREQ,
                                                    SubsystemT::SYS_INTER, SYSCommandsT::SYS_OSAL_NV_WRITE_EXT,
                                                    TypeT::SRSP, SYSCommandsT::SYS_OSAL_NV_WRITE_EXT,
                                                    value);
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::writeOSALItemInit(NvItemsIdsT itemId, uint16_t itemLen, vector<uint8_t>& data) {
    vector<uint8_t> payload;
    payload.push_back(LO_UINT16(itemId));
    payload.push_back(HI_UINT16(itemId));
    payload.push_back(LO_UINT16(itemLen));
    payload.push_back(HI_UINT16(itemLen));
    payload.push_back(data.size());
    payload.insert(payload.end(), data.begin(), data.end());
    return this->thisToZigbee().createCommandBuffer(payload, TypeT::SREQ,
                                                    SubsystemT::SYS_INTER, SYSCommandsT::SYS_OSAL_NV_ITEM_INIT,
                                                    TypeT::SRSP, SYSCommandsT::SYS_OSAL_NV_ITEM_INIT);
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::deleteOSALItem(NvItemsIdsT itemId, uint16_t len) {
    vector<uint8_t> payload;
    payload.push_back(LO_UINT16(itemId));
    payload.push_back(HI_UINT16(itemId));
    payload.push_back(LO_UINT16(len));
    payload.push_back(HI_UINT16(len));
    return this->thisToZigbee().createCommandBuffer(payload, TypeT::SREQ,
                                                    SubsystemT::SYS_INTER, SYSCommandsT::SYS_OSAL_NV_DELETE,
                                                    TypeT::SRSP, SYSCommandsT::SYS_OSAL_NV_DELETE);
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::readNVItemZstack(NvSystemIdsT sysId, NvItemsIdsT itemId, uint16_t subId, uint16_t offset, void* value) {
    this->coordinator->nvItemSys = itemId;

    uint16_t length {0};
    if (this->readNVLength(sysId, itemId, subId, &length) != ResultT::RESULT_SUCCESSFUL) {
        return ResultT::RESULT_FAIL;
    }
    if (!length) {
        return ResultT::RESULT_READ_DONE;
    }
    return this->readNVItem(sysId, itemId, subId, offset, length, value);
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::writeNVItemZstack(NvSystemIdsT sysId, NvItemsIdsT itemId, uint16_t subId, uint16_t offset, vector<uint8_t>& data) {
    uint16_t length {0};
    ZnpCommandStatusT status {ZnpCommandStatusT::SUCCESS_STATUS};
    if (this->readNVLength(sysId, itemId, subId, &length) != ResultT::RESULT_SUCCESSFUL) {
        return ResultT::RESULT_FAIL;
    }
    if (!length) {
        return ResultT::RESULT_SUCCESSFUL;
    }
    if (this->writeNVItem(sysId, itemId, subId, offset, data, &status) != ResultT::RESULT_SUCCESSFUL) {
        return ResultT::RESULT_FAIL;
    }
    return ((status == ZnpCommandStatusT::SUCCESS_STATUS) ? ResultT::RESULT_SUCCESSFUL : ResultT::RESULT_FAIL);
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::readNVLength(NvSystemIdsT sysId, NvItemsIdsT itemId, uint16_t subId, void* value) {
    vector<uint8_t> payload;
    payload.push_back(sysId);
    payload.push_back(LO_UINT16(itemId));
    payload.push_back(HI_UINT16(itemId));
    payload.push_back(LO_UINT16(subId));
    payload.push_back(HI_UINT16(subId));
    return this->thisToZigbee().createCommandBuffer(payload, TypeT::SREQ,
                                                    SubsystemT::SYS_INTER, SYSCommandsT::SYS_NV_LENGTH,
                                                    TypeT::SRSP, SYSCommandsT::SYS_NV_LENGTH,
                                                    value);
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::readNVItem(NvSystemIdsT sysId, NvItemsIdsT itemId, uint16_t subId, uint16_t offset, uint8_t len, void* value) {
    vector<uint8_t> payload;
    payload.push_back(sysId);
    payload.push_back(LO_UINT16(itemId));
    payload.push_back(HI_UINT16(itemId));
    payload.push_back(LO_UINT16(subId));
    payload.push_back(HI_UINT16(subId));
    payload.push_back(LO_UINT16(offset));
    payload.push_back(HI_UINT16(offset));
    payload.push_back(len);
    return this->thisToZigbee().createCommandBuffer(payload, TypeT::SREQ,
                                                    SubsystemT::SYS_INTER, SYSCommandsT::SYS_NV_READ,
                                                    TypeT::SRSP, SYSCommandsT::SYS_NV_READ,
                                                    value);
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::writeNVItem(NvSystemIdsT sysId, NvItemsIdsT itemId, uint16_t subId, uint16_t offset, vector<uint8_t>& data, void* value) {
    vector<uint8_t> payload;
    payload.push_back(sysId);
    payload.push_back(LO_UINT16(itemId));
    payload.push_back(HI_UINT16(itemId));
    payload.push_back(LO_UINT16(subId));
    payload.push_back(HI_UINT16(subId));
    payload.push_back(LO_UINT16(offset));
    payload.push_back(HI_UINT16(offset));
    payload.push_back(data.size());
    payload.insert(payload.end(), data.begin(), data.end());
    return this->thisToZigbee().createCommandBuffer(payload, TypeT::SREQ,
                                                    SubsystemT::SYS_INTER, SYSCommandsT::SYS_NV_WRITE,
                                                    TypeT::SRSP, SYSCommandsT::SYS_NV_WRITE,
                                                    value);
}

template <class ToZigbee>
ResultT ERaCommandZigbee<ToZigbee>::deleteNVItem(NvSystemIdsT sysId, NvItemsIdsT itemId, uint16_t subId) {
    vector<uint8_t> payload;
    payload.push_back(sysId);
    payload.push_back(LO_UINT16(itemId));
    payload.push_back(HI_UINT16(itemId));
    payload.push_back(LO_UINT16(subId));
    payload.push_back(HI_UINT16(subId));
    return this->thisToZigbee().createCommandBuffer(payload, TypeT::SREQ,
                                                    SubsystemT::SYS_INTER, SYSCommandsT::SYS_NV_DELETE,
                                                    TypeT::SRSP, SYSCommandsT::SYS_NV_DELETE);
}

#endif /* INC_ERA_COMMAND_NV_ITEM_ZIGBEE_HPP_ */
