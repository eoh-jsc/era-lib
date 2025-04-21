#ifndef INC_ERA_MODEL_HPP_
#define INC_ERA_MODEL_HPP_

#include <ERa/ERaConfig.hpp>

enum ERaDeviceTypeT {
    ERA_GATEWAY,
    ERA_END_DEVICE,

    ERA_DEVICE_TYPE_MAX,
};

class ERaModel
{
    const char* DeviceType[ERA_DEVICE_TYPE_MAX] = {
        "gateway",
        "end_device",
    };

public:
    ERaModel()
        : pORG(ERA_ORG_NAME)
        , pModel(ERA_MODEL_NAME)
        , pDeviceName(ERA_DEVICE_NAME)
        , pDeviceType(ERA_DEVICE_TYPE)
        , pDeviceSecret(ERA_DEVICE_SECRET)
    {}
    ~ERaModel()
    {}

    void setVendorName(const char* name) {
        this->setERaORG(name);
    }

    void setVendorPrefix(const char* prefix) {
        this->setERaModel(prefix);
    }

    void setERaORG(const char* org) {
        if (org == nullptr) {
            return;
        }
        this->pORG = org;
    }

    const char* getERaORG() const {
        return this->pORG;
    }

    void setERaModel(const char* model) {
        if (model == nullptr) {
            return;
        }
        this->pModel = model;
    }

    const char* getERaModel() const {
        return this->pModel;
    }

    void setDeviceName(const char* name) {
        if (name == nullptr) {
            return;
        }
        this->pDeviceName = name;
    }

    const char* getDeviceName() const {
        return this->pDeviceName;
    }

    void setDeviceType(ERaDeviceTypeT type) {
        if (type >= ERA_DEVICE_TYPE_MAX) {
            return;
        }
        this->pDeviceType = DeviceType[type];
    }

    const char* getDeviceType() const {
        return this->pDeviceType;
    }

    void setDeviceSecretKey(const char* key) {
        if (key == nullptr) {
            return;
        }
        this->pDeviceSecret = key;
    }

    const char* getDeviceSecretKey() const {
        return this->pDeviceSecret;
    }

private:
    const char* pORG;
    const char* pModel;
    const char* pDeviceName;
    const char* pDeviceType;
    const char* pDeviceSecret;
};

#endif /* INC_ERA_MODEL_HPP_ */
