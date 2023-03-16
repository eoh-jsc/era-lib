#include <Utility/ERaUtility.hpp>
#include <ERa/ERaButton.hpp>

ERaButton::ERaButton()
    : numButton(0)
    , timeout(50L)
{}

void ERaButton::run() {
	unsigned long currentMillis = ERaMillis();
    const ERaList<Button_t*>::iterator* e = this->button.end();
    for (ERaList<Button_t*>::iterator* it = this->button.begin(); it != e; it = it->getNext()) {
        Button_t* pButton = it->get();
        if (!this->isValidButton(pButton)) {
            continue;
        }
        if (currentMillis - pButton->prevTimeout < this->timeout) {
            continue;
        }
        unsigned long skipTimes = (currentMillis - pButton->prevTimeout) / this->timeout;
        // update time
        pButton->prevTimeout += this->timeout * skipTimes;
        // call callback
        if (!pButton->enable) {
            continue;
        }
        if (pButton->readPin == nullptr) {
            continue;
        }
        if (!pButton->invert) {
            pButton->state = pButton->readPin(pButton->pin);
        }
        else {
            pButton->state = !pButton->readPin(pButton->pin);
        }
        if (pButton->state != pButton->prevState) {
            pButton->prevState = pButton->state;
            this->setFlag(pButton->called, ButtonFlagT::BUTTON_ON_CHANGE, true);
            if (pButton->state) {
                this->setFlag(pButton->called, ButtonFlagT::BUTTON_ON_RISING, true);
            }
            else {
                this->setFlag(pButton->called, ButtonFlagT::BUTTON_ON_FALLING, true);
            }
        }
        if (!pButton->state && !pButton->pressed) {
            pButton->pressed = true;
            pButton->prevMillis = currentMillis;
        }
        else if (!pButton->state && pButton->pressed) {
            if (currentMillis - pButton->prevMillis >= pButton->delay) {
                pButton->prevMillis = currentMillis;
                this->setFlag(pButton->called, ButtonFlagT::BUTTON_ON_HOLD, true);
            }
        }
        else if (pButton->state && pButton->pressed) {
            pButton->pressed = false;
        }
    }

    ERaList<Button_t*>::iterator* next = nullptr;
    for (ERaList<Button_t*>::iterator* it = this->button.begin(); it != e; it = next) {
        next = it->getNext();
        Button_t* pButton = it->get();
        if (!this->isValidButton(pButton)) {
            continue;
        }
        if (!pButton->called) {
            continue;
        }
        if (this->getFlag(pButton->called, ButtonFlagT::BUTTON_ON_CHANGE)) {
            if (pButton->callback_p == nullptr) {
                if (pButton->callback != nullptr) {
                    pButton->callback();
                }
            }
            else {
                pButton->callback_p(pButton->param);
            }
        }
        if (this->getFlag(pButton->called, ButtonFlagT::BUTTON_ON_FALLING)) {
            if (pButton->callback_falling_p == nullptr) {
                if (pButton->callback_falling != nullptr) {
                    pButton->callback_falling();
                }
            }
            else {
                pButton->callback_falling_p(pButton->param_falling);
            }
        }
        if (this->getFlag(pButton->called, ButtonFlagT::BUTTON_ON_RISING)) {
            if (pButton->callback_rising_p == nullptr) {
                if (pButton->callback_rising != nullptr) {
                    pButton->callback_rising();
                }
            }
            else {
                pButton->callback_rising_p(pButton->param_rising);
            }
        }
        if (this->getFlag(pButton->called, ButtonFlagT::BUTTON_ON_HOLD)) {
            if (pButton->callback_hold_p == nullptr) {
                if (pButton->callback_hold != nullptr) {
                    pButton->callback_hold();
                }
            }
            else {
                pButton->callback_hold_p(pButton->param_hold);
            }
        }
        if (this->getFlag(pButton->called, ButtonFlagT::BUTTON_ON_DELETE)) {
            delete pButton;
            pButton = nullptr;
            it->get() = nullptr;
            this->button.remove(it);
            this->numButton--;
            continue;
        }
        pButton->called = 0;
    }
}

ERaButton::Button_t* ERaButton::setupButton(uint8_t pin, ERaButton::ReadPinHandler_t readPin, bool invert) {
    if (!this->isButtonFree()) {
        return nullptr;
    }
    if (readPin == nullptr) {
        return nullptr;
    }

    Button_t* pButton = new Button_t();
    if (pButton == nullptr) {
        return nullptr;
    }

    pButton->delay = 0;
    pButton->readPin = readPin;
    pButton->callback = nullptr;
    pButton->callback_p = nullptr;
    pButton->callback_falling = nullptr;
    pButton->callback_falling_p = nullptr;
    pButton->callback_rising = nullptr;
    pButton->callback_rising_p = nullptr;
    pButton->callback_hold = nullptr;
    pButton->callback_hold_p = nullptr;
    pButton->param = nullptr;
    pButton->param_falling = nullptr;
    pButton->param_rising = nullptr;
    pButton->param_hold = nullptr;
    pButton->enable = true;
    pButton->called = 0;
    pButton->prevMillis = ERaMillis();
    pButton->prevTimeout = ERaMillis();
    pButton->pin = pin;
    pButton->invert = invert;
    if (!invert) {
        pButton->state = readPin(pin);
        pButton->prevState = readPin(pin);
    }
    else {
        pButton->state = !readPin(pin);
        pButton->prevState = !readPin(pin);
    }
    pButton->pressed = false;
    this->button.put(pButton);
    this->numButton++;
    return pButton;
}

void ERaButton::onChange(Button_t* pButton, ERaButton::ButtonCallback_t cb) {
    if (pButton == nullptr) {
        return;
    }

    pButton->callback = cb;
    pButton->callback_p = nullptr;
    pButton->param = nullptr;
}

void ERaButton::onChange(Button_t* pButton, ERaButton::ButtonCallback_p_t cb, void* arg) {
    if (pButton == nullptr) {
        return;
    }

    pButton->callback = nullptr;
    pButton->callback_p = cb;
    pButton->param = arg;
}

void ERaButton::onFalling(Button_t* pButton, ERaButton::ButtonCallback_t cb) {
    if (pButton == nullptr) {
        return;
    }

    pButton->callback_falling = cb;
    pButton->callback_falling_p = nullptr;
    pButton->param_falling = nullptr;
}

void ERaButton::onFalling(Button_t* pButton, ERaButton::ButtonCallback_p_t cb, void* arg) {
    if (pButton == nullptr) {
        return;
    }

    pButton->callback_falling = nullptr;
    pButton->callback_falling_p = cb;
    pButton->param_falling = arg;
}

void ERaButton::onRising(Button_t* pButton, ERaButton::ButtonCallback_t cb) {
    if (pButton == nullptr) {
        return;
    }

    pButton->callback_rising = cb;
    pButton->callback_rising_p = nullptr;
    pButton->param_rising = nullptr;
}

void ERaButton::onRising(Button_t* pButton, ERaButton::ButtonCallback_p_t cb, void* arg) {
    if (pButton == nullptr) {
        return;
    }

    pButton->callback_rising = nullptr;
    pButton->callback_rising_p = cb;
    pButton->param_rising = arg;
}

void ERaButton::onHold(Button_t* pButton, unsigned long delay, ERaButton::ButtonCallback_t cb) {
    if (pButton == nullptr) {
        return;
    }

    pButton->delay = delay;
    pButton->callback_hold = cb;
    pButton->callback_hold_p = nullptr;
    pButton->param_hold = nullptr;
}

void ERaButton::onHold(Button_t* pButton, unsigned long delay, ERaButton::ButtonCallback_p_t cb, void* arg) {
    if (pButton == nullptr) {
        return;
    }

    pButton->delay = delay;
    pButton->callback_hold = nullptr;
    pButton->callback_hold_p = cb;
    pButton->param_hold = arg;
}

void ERaButton::deleteButton(Button_t* pButton) {
    if (!this->numButton) {
        return;
    }

    if (this->isValidButton(pButton)) {
        this->setFlag(pButton->called, ButtonFlagT::BUTTON_ON_DELETE, true);
    }
}

bool ERaButton::isEnable(Button_t* pButton) {
    if (this->isValidButton(pButton)) {
        return pButton->enable;
    }
    else {
        return false;
    }
}

void ERaButton::enable(Button_t* pButton) {
    if (this->isValidButton(pButton)) {
        pButton->enable = true;
    }
}

void ERaButton::disable(Button_t* pButton) {
    if (this->isValidButton(pButton)) {
        pButton->enable = false;
    }
}

void ERaButton::enableAll() {
    const ERaList<Button_t*>::iterator* e = this->button.end();
    for (ERaList<Button_t*>::iterator* it = this->button.begin(); it != e; it = it->getNext()) {
        Button_t* pButton = it->get();
        if (this->isValidButton(pButton)) {
            pButton->enable = true;
        }
    }
}

void ERaButton::disableAll() {
    const ERaList<Button_t*>::iterator* e = this->button.end();
    for (ERaList<Button_t*>::iterator* it = this->button.begin(); it != e; it = it->getNext()) {
        Button_t* pButton = it->get();
        if (this->isValidButton(pButton)) {
            pButton->enable = false;
        }
    }
}

bool ERaButton::isButtonFree() {
    if (this->numButton >= MAX_BUTTONS) {
        return false;
    }

    return true;
}
