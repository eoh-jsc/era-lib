#ifndef INC_ERA_WIDGET_TERMINAL_BOX_HPP_
#define INC_ERA_WIDGET_TERMINAL_BOX_HPP_

#include <ERa/ERaProtocol.hpp>
#include <Widgets/ERaWidgetTerminalDef.hpp>

class ERaWidgetTerminalBox
    : public Stream
{
public:
    ERaWidgetTerminalBox(ERaString& str,
                        uint16_t fromVPin,
                        uint16_t toVPin)
        : estr(str)
        , callback(NULL)
        , fromPin(fromVPin)
        , toPin(toVPin)
        , txBufferCount(0UL)
    {}

    virtual ~ERaWidgetTerminalBox()
    {}

    void setPin(uint16_t fromVPin, uint16_t toVPin) {
        this->fromPin = fromVPin;
        this->toPin = toVPin;
    }

    void begin(WidgetCallback_t cb, uint16_t fromVPin, uint16_t toVPin) {
        this->setPin(fromVPin, toVPin);
        this->begin(cb);
    }

#if defined(ERA_HAS_FUNCTIONAL_H)
    void begin(WidgetCallback_t cb) {
        this->callback = cb;
        ERa.addPropertyVirtual(this->fromPin, this->estr, PermissionT::PERMISSION_READ_WRITE).onUpdate([&, this](void) {
            this->onUpdate();
        });
    }
#else
    void begin(WidgetCallback_t cb) {
        this->callback = cb;
        ERa.addPropertyVirtual(this->fromPin, this->estr, PermissionT::PERMISSION_READ_WRITE).onUpdate([](void* args) {
            ERaWidgetTerminalBox* pTerminal = (ERaWidgetTerminalBox*)args;
            if (pTerminal == nullptr) {
                return;
            }
            pTerminal->onUpdate();
        }, this);
    }
#endif

    void clear() {
        this->txBufferCount = 0;
    }

    virtual size_t write(uint8_t byte) override {
        this->txBuffer[this->txBufferCount++] = byte;
        if (byte == '\n') {
            this->flush();
        }
        if (this->txBufferCount >= (sizeof(this->txBuffer) - 1)) {
            this->flush();
        }
        return 1;
    }

    void trimTxBuffer() {
        for (size_t i = 0; i < this->txBufferCount; ++i) {
            if (!isspace(this->txBuffer[i])) {
                return;
            }
        }
        this->txBufferCount = 0;
    }

    virtual void flush() override {
            this->trimTxBuffer();

        if (this->txBufferCount &&
            ERa.connected()) {
            this->txBuffer[this->txBufferCount] = '\0';
            ERa.virtualWrite(this->toPin, (char*)this->txBuffer, true);
#if !defined(ERA_ABBR)
            ERa.getPropertyReport().run();
#endif
            this->txBufferCount = 0;
        }
    }

    virtual int read() override {
        if (this->rxBuffer.isEmpty()) {
            return -1;
        }
        return this->rxBuffer.get();
    }

    virtual int available() override {
        return this->rxBuffer.size();
    }

    virtual int peek() override {
        if (this->rxBuffer.isEmpty()) {
            return -1;
        }
        return this->rxBuffer.peek();
    }

#if defined(ERA_USE_STREAM)

    using Stream::write;

    virtual size_t write(const void* buf, size_t size) {
        return this->write((char*)buf, size);
    }

#else

    virtual size_t write(const void* buf, size_t size) {
        uint8_t* begin = (uint8_t*)buf;
        size_t left = size;
        while (left--) {
            this->write(*begin++);
        }
        return size;
    }

    virtual size_t write(const char* str) {
        return this->write(str, strlen(str));
    }

#endif

private:
    void process() {
        this->rxBuffer.put((uint8_t*)this->estr.getString(), this->estr.length());
    }

    void onUpdate() {
        this->process();
        ERa.virtualWrite(this->fromPin, this->estr, true);
#if !defined(ERA_ABBR)
        ERa.getPropertyReport().run();
#endif
        if (this->callback != NULL) {
            this->callback();
        }
    }

    ERaString& estr;
    WidgetCallback_t callback;

    uint16_t fromPin;
    uint16_t toPin;

    ERaQueue<uint8_t, ERA_TERMINAL_INPUT_BUFFER_SIZE> rxBuffer;
    uint8_t txBuffer[ERA_TERMINAL_OUTPUT_BUFFER_SIZE + 1];
    size_t txBufferCount;
};

#endif /* INC_ERA_WIDGET_TERMINAL_BOX_HPP_ */
