#ifndef INC_ERA_BUTTON_HPP_
#define INC_ERA_BUTTON_HPP_

#include <stdlib.h>
#include <stdint.h>
#include <ERa/ERaDefine.hpp>
#include <ERa/ERaDetect.hpp>
#include <Utility/ERaQueue.hpp>

#if defined(__has_include) &&       \
    __has_include(<functional>) &&  \
    !defined(ERA_IGNORE_STD_FUNCTIONAL_STRING)
    #include <functional>
    #define BUTTON_HAS_FUNCTIONAL_H
#endif

class ERaButton
{
#if defined(BUTTON_HAS_FUNCTIONAL_H)
    typedef std::function<void(void)> ButtonCallback_t;
	typedef std::function<void(void*)> ButtonCallback_p_t;
    typedef std::function<int(uint8_t)> ReadPinHandler_t;
#else
    typedef void (*ButtonCallback_t)(void);
	typedef void (*ButtonCallback_p_t)(void*);
    typedef int (*ReadPinHandler_t)(uint8_t);
#endif

	const static int MAX_BUTTONS = 5;
    enum ButtonFlagT {
        BUTTON_ON_CHANGE = 0x01,
        BUTTON_ON_FALLING = 0x02,
        BUTTON_ON_RISING = 0x04,
        BUTTON_ON_HOLD = 0x08,
        BUTTON_ON_DELETE = 0x80
    };
	typedef struct __Button_t {
        unsigned long prevTimeout;
		unsigned long prevMillis;
        unsigned long delay;
        ERaButton::ReadPinHandler_t readPin;
		ERaButton::ButtonCallback_t callback;
		ERaButton::ButtonCallback_p_t callback_p;
		ERaButton::ButtonCallback_t callback_falling;
		ERaButton::ButtonCallback_p_t callback_falling_p;
		ERaButton::ButtonCallback_t callback_rising;
		ERaButton::ButtonCallback_p_t callback_rising_p;
		ERaButton::ButtonCallback_t callback_hold;
		ERaButton::ButtonCallback_p_t callback_hold_p;
		void* param;
		void* param_falling;
		void* param_rising;
		void* param_hold;
        uint8_t pin;
        bool invert;
        bool state;
        bool prevState;
        bool pressed;
		bool enable;
		uint8_t called;
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

        void onChange(ERaButton::ButtonCallback_t cb) {
            if (this->isValid()) {
                this->bt->onChange(this->pBt, cb);
            }
        }

        void onChange(ERaButton::ButtonCallback_p_t cb, void* arg) {
            if (this->isValid()) {
                this->bt->onChange(this->pBt, cb, arg);
            }
        }

        void onFalling(ERaButton::ButtonCallback_t cb) {
            if (this->isValid()) {
                this->bt->onFalling(this->pBt, cb);
            }
        }

        void onFalling(ERaButton::ButtonCallback_p_t cb, void* arg) {
            if (this->isValid()) {
                this->bt->onFalling(this->pBt, cb, arg);
            }
        }

        void onRising(ERaButton::ButtonCallback_t cb) {
            if (this->isValid()) {
                this->bt->onRising(this->pBt, cb);
            }
        }

        void onRising(ERaButton::ButtonCallback_p_t cb, void* arg) {
            if (this->isValid()) {
                this->bt->onRising(this->pBt, cb, arg);
            }
        }

        void onHold(unsigned long delay, ERaButton::ButtonCallback_t cb) {
            if (this->isValid()) {
                this->bt->onHold(this->pBt, delay, cb);
            }
        }

        void onHold(unsigned long delay, ERaButton::ButtonCallback_p_t cb, void* arg) {
            if (this->isValid()) {
                this->bt->onHold(this->pBt, delay, cb, arg);
            }
        }

        void deleteButton() {
            if (!this->isValid()) {
                return;
            }
            this->bt->deleteButton(this->pBt);
            this->invalidate();
        }

        bool isEnable() {
            if (!this->isValid()) {
                return false;
            }
            return this->bt->isEnable(this->pBt);
        }

        void enable() {
            if (this->isValid()) {
                this->bt->enable(this->pBt);
            }
        }

        void disable() {
            if (this->isValid()) {
                this->bt->disable(this->pBt);
            }
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

    iterator setButton(uint8_t pin, ERaButton::ReadPinHandler_t readPin, bool invert = false) {
        return iterator(this, this->setupButton(pin, readPin, invert));
    }

    void setTimeout(unsigned long _timeout) {
        this->timeout = _timeout;
    }

    unsigned long getTimeout() {
        return this->timeout;
    }

    void onChange(Button_t* pButton, ERaButton::ButtonCallback_t cb);
    void onChange(Button_t* pButton, ERaButton::ButtonCallback_p_t cb, void* arg);
    void onFalling(Button_t* pButton, ERaButton::ButtonCallback_t cb);
    void onFalling(Button_t* pButton, ERaButton::ButtonCallback_p_t cb, void* arg);
    void onRising(Button_t* pButton, ERaButton::ButtonCallback_t cb);
    void onRising(Button_t* pButton, ERaButton::ButtonCallback_p_t cb, void* arg);
    void onHold(Button_t* pButton, unsigned long delay, ERaButton::ButtonCallback_t cb);
    void onHold(Button_t* pButton, unsigned long delay, ERaButton::ButtonCallback_p_t cb, void* arg);
    void deleteButton(Button_t* pButton);
    bool isEnable(Button_t* pButton);
    void enable(Button_t* pButton);
    void disable(Button_t* pButton);
    void enableAll();
    void disableAll();

protected:
private:
    Button_t* setupButton(uint8_t pin, ERaButton::ReadPinHandler_t readPin, bool invert = false);
    bool isButtonFree();

    bool isValidButton(const Button_t* pButton) {
        if (pButton == nullptr) {
            return false;
        }
        return ((pButton->callback != nullptr) || (pButton->callback_p != nullptr) ||
                (pButton->callback_falling != nullptr) || (pButton->callback_falling_p != nullptr) ||
                (pButton->callback_rising != nullptr) || (pButton->callback_rising_p != nullptr) ||
                (pButton->callback_hold != nullptr) || (pButton->callback_hold_p != nullptr));
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
};

#endif /* INC_ERA_BUTTON_HPP_ */
