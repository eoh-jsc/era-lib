#ifndef INC_ERA_BUTTON_HPP_
#define INC_ERA_BUTTON_HPP_

#include <stdlib.h>
#include <stdint.h>
#include <ERa/ERaDefine.hpp>
#include <ERa/ERaDetect.hpp>
#include <Utility/ERaQueue.hpp>

enum ButtonEventT {
    BUTTON_ON_CHANGE = 0x01,
    BUTTON_ON_FALLING = 0x02,
    BUTTON_ON_RISING = 0x04,
    BUTTON_ON_HOLD = 0x08,
    BUTTON_ON_MULTI = 0x10,
    BUTTON_ON_DELETE = 0x80
};

class ERaButton
{
#if defined(ERA_HAS_FUNCTIONAL_H)
    typedef std::function<void(uint16_t, ButtonEventT)> ButtonCallback_t;
    typedef std::function<void(uint16_t, ButtonEventT, void*)> ButtonCallback_p_t;
    typedef std::function<int(uint16_t)> ReadPinHandler_t;
#else
    typedef void (*ButtonCallback_t)(uint16_t, ButtonEventT);
    typedef void (*ButtonCallback_p_t)(uint16_t, ButtonEventT, void*);
    typedef int (*ReadPinHandler_t)(uint16_t);
#endif

    const static int MAX_BUTTONS = 5;
    typedef struct __Button_t {
        unsigned long prevTimeout;
        unsigned long prevMillis;
        unsigned long delay;
        unsigned long prevMulti;
        unsigned long delayMulti;
        ERaButton::ReadPinHandler_t readPin;
        ERaButton::ButtonCallback_t callback;
        ERaButton::ButtonCallback_p_t callback_p;
        void* param;
        uint8_t flag;
        uint16_t pin;
        bool invert;
        bool state;
        bool prevState;
        bool pressed;
        bool onHold;
        bool reload;
        bool enable;
        uint8_t called;
        uint8_t countMulti;
        uint8_t numberMulti;
    } Button_t;

public:
    class iterator
    {
    public:
        iterator()
            : bt(nullptr)
            , pBt(nullptr)
        {}
        iterator(ERaButton* _bt, Button_t* _pBt)
            : bt(_bt)
            , pBt(_pBt)
        {}
        ~iterator()
        {}

        operator Button_t*() const {
            return this->pBt;
        }

        operator bool() const {
            return this->isValid();
        }

        bool isValid() const {
            return ((this->bt != nullptr) && (this->pBt != nullptr));
        }

        Button_t* getId() const {
            return this->pBt;
        }

        bool isEnable() const {
            if (!this->isValid()) {
                return false;
            }
            return this->bt->isEnable(this->pBt);
        }

        void enable() const {
            if (!this->isValid()) {
                return;
            }
            this->bt->enable(this->pBt);
        }

        void disable() const {
            if (!this->isValid()) {
                return;
            }
            this->bt->disable(this->pBt);
        }

        void deleteButton() {
            if (!this->isValid()) {
                return;
            }
            this->bt->deleteButton(this->pBt);
            this->invalidate();
        }

        iterator& onChange() {
            if (this->isValid()) {
                this->bt->onChange(this->pBt);
            }
            return (*this);
        }

        iterator& onFalling() {
            if (this->isValid()) {
                this->bt->onFalling(this->pBt);
            }
            return (*this);
        }

        iterator& onRising() {
            if (this->isValid()) {
                this->bt->onRising(this->pBt);
            }
            return (*this);
        }

        iterator& onHold(unsigned long delay) {
            if (this->isValid()) {
                this->bt->onHold(this->pBt, delay);
            }
            return (*this);
        }

        iterator& onMulti(uint8_t num, unsigned long delay) {
            if (this->isValid()) {
                this->bt->onMulti(this->pBt, num, delay);
            }
            return (*this);
        }

        iterator& setBounceTime(unsigned long timeout = 50UL) {
            if (this->isValid()) {
                this->bt->setBounceTime(timeout);
            }
            return (*this);
        }

        iterator& reload(bool enable = true) {
            if (this->isValid()) {
                this->bt->reload(this->pBt, enable);
            }
            return (*this);
        }

        uint8_t getButtonPressCount() {
            uint8_t count {0};
            if (this->isValid()) {
                count = this->bt->getButtonPressCount(this->pBt);
            }
            return count;
        }

    protected:
    private:
        void invalidate() {
            this->bt = nullptr;
            this->pBt = nullptr;
        }

        ERaButton* bt;
        Button_t* pBt;
    };

    ERaButton();
    ~ERaButton()
    {}

    void run();

    iterator setButton(uint16_t pin, ERaButton::ReadPinHandler_t readPin,
                        ERaButton::ButtonCallback_t cb, bool invert = false) {
        return iterator(this, this->setupButton(pin, readPin, cb, invert));
    }

    iterator setButton(uint16_t pin, ERaButton::ReadPinHandler_t readPin,
                        ERaButton::ButtonCallback_p_t cb, void* args, bool invert = false) {
        return iterator(this, this->setupButton(pin, readPin, cb, args, invert));
    }

    void setBounceTime(unsigned long _timeout = 50UL) {
        this->timeout = _timeout;
    }

    void setTimeout(unsigned long _timeout = 50UL) {
        this->timeout = _timeout;
    }

    unsigned long getTimeout() {
        return this->timeout;
    }

    void onChange(Button_t* pButton);
    void onFalling(Button_t* pButton);
    void onRising(Button_t* pButton);
    void onHold(Button_t* pButton, unsigned long delay);
    void onMulti(Button_t* pButton, uint8_t num, unsigned long delay);
    void reload(Button_t* pButton, bool enable);
    uint8_t getButtonPressCount(Button_t* pButton);
    void deleteButton(Button_t* pButton);
    bool isEnable(Button_t* pButton);
    void enable(Button_t* pButton);
    void disable(Button_t* pButton);
    void enableAll();
    void disableAll();

protected:
private:
    Button_t* setupButton(uint16_t pin, ERaButton::ReadPinHandler_t readPin,
                        ERaButton::ButtonCallback_t cb, bool invert = false);
    Button_t* setupButton(uint16_t pin, ERaButton::ReadPinHandler_t readPin,
                        ERaButton::ButtonCallback_p_t cb, void* args, bool invert = false);
    bool deleteHandler();
    bool isButtonFree();

    bool isValidButton(const Button_t* pButton) const {
        if (pButton == nullptr) {
            return false;
        }
        return ((pButton->callback != nullptr) || (pButton->callback_p != nullptr));
    }

    bool isCalledFlag(Button_t* pButton, uint8_t mask) {
        if (pButton == nullptr) {
            return false;
        }
        bool ret = (this->getFlag(pButton->called, mask) &&
                    this->getFlag(pButton->flag, mask));
        this->setFlag(pButton->called, mask, false);
        return ret;
    }

    bool isCalled(Button_t* pButton, uint8_t mask) {
        if (pButton == nullptr) {
            return false;
        }
        bool ret = this->getFlag(pButton->called, mask);
        this->setFlag(pButton->called, mask, false);
        return ret;
    }

    void setFlag(uint8_t& flags, uint8_t mask, bool value) {
        if (value) {
            flags |= mask;
        }
        else {
            flags &= ~mask;
        }
    }

    bool getFlag(uint8_t flags, uint8_t mask) {
        return (flags & mask) == mask;
    }

    ERaList<Button_t*> button;
    unsigned int numButton;
    unsigned long timeout;

    using ButtonIterator = typename ERaList<Button_t*>::iterator;
};

using ButtonEntry = ERaButton::iterator;

#endif /* INC_ERA_BUTTON_HPP_ */
