#ifndef INC_WRAPPER_OBJECT_HPP_
#define INC_WRAPPER_OBJECT_HPP_

#include "WrapperBase.hpp"

class WrapperObject
    : public WrapperBase
{
public:
    WrapperObject(ERaDataJson& js)
        : value(js)
        , local()
    {
        this->type = WrapperTypeT::WRAPPER_TYPE_OBJECT;
    }

    virtual WrapperObject& operator = (const ERaDataJson& rjs) {
        this->value = rjs;
        return (*this);
    }

    bool updated() override {
        if ((this->value == nullptr) &&
            (this->local == nullptr)) {
            return false;
        }
        if (this->value == this->local) {
            return false;
        }
        this->local = this->value;
        return true;
    }

    bool isSendJSON() const override {
        return this->value.isSendJSON();
    }

protected:
    double get() const override {
        return 0.0f;
    }

    void set(double num) override {
        (void)num;
    }

    const void* getPointer() const override {
        return (const void*)this->value.getString();
    }

    void setPointer(const void* cptr) override {
        cJSON* root = cJSON_Parse((const char*)cptr);
        if (root != nullptr) {
            this->value.setObject(root, true);
        }
        root = nullptr;
    }

protected:
    ERaDataJson& value;
    ERaDataJson  local;
};

#endif /* INC_WRAPPER_OBJECT_HPP_ */
