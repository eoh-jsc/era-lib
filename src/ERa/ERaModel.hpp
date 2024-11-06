#ifndef INC_ERA_MODEL_HPP_
#define INC_ERA_MODEL_HPP_

#include <ERa/ERaConfig.hpp>

class ERaModel
{
public:
    ERaModel()
        : pORG(ERA_ORG_NAME)
        , pModel(ERA_MODEL_NAME)
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

private:
    const char* pORG;
    const char* pModel;
};

#endif /* INC_ERA_MODEL_HPP_ */
