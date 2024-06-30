#ifndef INC_WRAPPER_ARRAY_HPP_
#define INC_WRAPPER_ARRAY_HPP_

#include "WrapperObject.hpp"

class WrapperArray
    : public WrapperObject
{
public:
    WrapperArray(ERaDataJson& js)
        : WrapperObject(js)
    {
        this->type = WrapperTypeT::WRAPPER_TYPE_ARRAY;
    }
    ~WrapperArray() override
    {
        if (!this->options) {
            return;
        }
        this->value.~ERaDataJson();
    }

    WrapperObject& operator = (const ERaDataJson& rjs) override {
        if (rjs.isArray()) {
            this->value = rjs;
        }
        return (*this);
    }

    void setOptions(int option) override {
        this->options = option;
    }

    void setPointer(const void* cptr) override {
        cJSON* root = cJSON_Parse((const char*)cptr);
        if (cJSON_IsArray(root)) {
            this->value.setObject(root, true);
        }
        else {
            cJSON_Delete(root);
        }
        root = nullptr;
    }

private:
    int options;
};

#endif /* INC_WRAPPER_ARRAY_HPP_ */
