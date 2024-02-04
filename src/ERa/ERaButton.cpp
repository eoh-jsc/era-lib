#include <Utility/ERaUtility.hpp>
#include <ERa/ERaButton.hpp>

ERaButton::ERaButton()
    : numButton(0)
    , timeout(50UL)
{}

void ERaButton::run() {
    unsigned long currentMillis = ERaMillis();
    const ERaList<Button_t*>::iterator* e = this->button.end();
    for (ERaList<Button_t*>::iterator* it = this->button.begin(); it != e; it = it->getNext()) {
        Button_t* pButton = it->get();
        if (!this->isValidButton(pButton)) {
            continue;
        }
        if ((currentMillis - pButton->prevTimeout) < this->timeout) {
            continue;
        }
        unsigned long skipTimes = ((currentMillis - pButton->prevTimeout) / this->timeout);
        // update time
        pButton->prevTimeout += (this->timeout * skipTimes);
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
            this->setFlag(pButton->called, ButtonEventT::BUTTON_ON_CHANGE, true);
            if (pButton->state) {
                this->setFlag(pButton->called, ButtonEventT::BUTTON_ON_RISING, true);
            }
            else {
                this->setFlag(pButton->called, ButtonEventT::BUTTON_ON_FALLING, true);
                if ((currentMillis - pButton->prevMulti) > pButton->delayMulti) {
                    pButton->countMulti = 0;
                    pButton->prevMulti = currentMillis;
                }
                if (++pButton->countMulti >= pButton->numberMulti) {
                    pButton->countMulti = 0;
                    pButton->prevMulti = currentMillis;
                    this->setFlag(pButton->called, ButtonEventT::BUTTON_ON_MULTI, true);
                }
            }
        }
        if (!pButton->state && !pButton->pressed) {
            pButton->onHold = false;
            pButton->pressed = true;
            pButton->prevMillis = currentMillis;
        }
        else if (!pButton->state && pButton->pressed) {
            if (!pButton->onHold && ((currentMillis - pButton->prevMillis) >= pButton->delay)) {
                pButton->onHold = true;
                pButton->prevMillis = currentMillis;
                this->setFlag(pButton->called, ButtonEventT::BUTTON_ON_HOLD, true);
            }
        }
        else if (pButton->state && pButton->pressed) {
            pButton->onHold = false;
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
        if (this->getFlag(pButton->called, ButtonEventT::BUTTON_ON_CHANGE) &&
            this->getFlag(pButton->flag, ButtonEventT::BUTTON_ON_CHANGE)) {
            if (pButton->callback_p == nullptr) {
                if (pButton->callback != nullptr) {
                    pButton->callback(pButton->pin, ButtonEventT::BUTTON_ON_CHANGE);
                }
            }
            else {
                pButton->callback_p(pButton->pin, ButtonEventT::BUTTON_ON_CHANGE, pButton->param);
            }
        }
        if (this->getFlag(pButton->called, ButtonEventT::BUTTON_ON_FALLING) &&
            this->getFlag(pButton->flag, ButtonEventT::BUTTON_ON_FALLING)) {
            if (pButton->callback_p == nullptr) {
                if (pButton->callback != nullptr) {
                    pButton->callback(pButton->pin, ButtonEventT::BUTTON_ON_FALLING);
                }
            }
            else {
                pButton->callback_p(pButton->pin, ButtonEventT::BUTTON_ON_FALLING, pButton->param);
            }
        }
        if (this->getFlag(pButton->called, ButtonEventT::BUTTON_ON_RISING) &&
            this->getFlag(pButton->flag, ButtonEventT::BUTTON_ON_RISING)) {
            if (pButton->callback_p == nullptr) {
                if (pButton->callback != nullptr) {
                    pButton->callback(pButton->pin, ButtonEventT::BUTTON_ON_RISING);
                }
            }
            else {
                pButton->callback_p(pButton->pin, ButtonEventT::BUTTON_ON_RISING, pButton->param);
            }
        }
        if (this->getFlag(pButton->called, ButtonEventT::BUTTON_ON_HOLD) &&
            this->getFlag(pButton->flag, ButtonEventT::BUTTON_ON_HOLD)) {
            if (pButton->callback_p == nullptr) {
                if (pButton->callback != nullptr) {
                    pButton->callback(pButton->pin, ButtonEventT::BUTTON_ON_HOLD);
                }
            }
            else {
                pButton->callback_p(pButton->pin, ButtonEventT::BUTTON_ON_HOLD, pButton->param);
            }
        }
        if (this->getFlag(pButton->called, ButtonEventT::BUTTON_ON_MULTI) &&
            this->getFlag(pButton->flag, ButtonEventT::BUTTON_ON_MULTI)) {
            if (pButton->callback_p == nullptr) {
                if (pButton->callback != nullptr) {
                    pButton->callback(pButton->pin, ButtonEventT::BUTTON_ON_MULTI);
                }
            }
            else {
                pButton->callback_p(pButton->pin, ButtonEventT::BUTTON_ON_MULTI, pButton->param);
            }
        }
        if (this->getFlag(pButton->called, ButtonEventT::BUTTON_ON_DELETE)) {
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

ERaButton::Button_t* ERaButton::setupButton(uint8_t pin, ERaButton::ReadPinHandler_t readPin,
                                            ERaButton::ButtonCallback_t cb, bool invert) {
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
    pButton->callback = cb;
    pButton->callback_p = nullptr;
    pButton->param = nullptr;
    pButton->enable = true;
    pButton->onHold = false;
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

ERaButton::Button_t* ERaButton::setupButton(uint8_t pin, ERaButton::ReadPinHandler_t readPin,
                                            ERaButton::ButtonCallback_p_t cb, void* args, bool invert) {
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
    pButton->delayMulti = 0;
    pButton->readPin = readPin;
    pButton->callback = nullptr;
    pButton->callback_p = cb;
    pButton->param = args;
    pButton->enable = true;
    pButton->onHold = false;
    pButton->called = 0;
    pButton->countMulti = 0;
    pButton->numberMulti = 0;
    pButton->prevMillis = ERaMillis();
    pButton->prevTimeout = ERaMillis();
    pButton->prevMulti = ERaMillis();
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

void ERaButton::onChange(Button_t* pButton) {
    if (pButton == nullptr) {
        return;
    }

    this->setFlag(pButton->flag, ButtonEventT::BUTTON_ON_CHANGE, true);
}

void ERaButton::onFalling(Button_t* pButton) {
    if (pButton == nullptr) {
        return;
    }

    this->setFlag(pButton->flag, ButtonEventT::BUTTON_ON_FALLING, true);
}

void ERaButton::onRising(Button_t* pButton) {
    if (pButton == nullptr) {
        return;
    }

    this->setFlag(pButton->flag, ButtonEventT::BUTTON_ON_RISING, true);
}

void ERaButton::onHold(Button_t* pButton, unsigned long delay) {
    if (pButton == nullptr) {
        return;
    }

    pButton->delay = delay;
    this->setFlag(pButton->flag, ButtonEventT::BUTTON_ON_HOLD, true);
}

void ERaButton::onMulti(Button_t* pButton, uint8_t num, unsigned long delay) {
    if (pButton == nullptr) {
        return;
    }

    pButton->numberMulti = num;
    pButton->delayMulti = delay;
    this->setFlag(pButton->flag, ButtonEventT::BUTTON_ON_MULTI, true);
}

void ERaButton::deleteButton(Button_t* pButton) {
    if (!this->numButton) {
        return;
    }

    if (this->isValidButton(pButton)) {
        this->setFlag(pButton->called, ButtonEventT::BUTTON_ON_DELETE, true);
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
