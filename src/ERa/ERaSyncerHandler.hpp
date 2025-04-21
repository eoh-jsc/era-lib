#ifndef INC_ERA_SYNCER_HANDLER_HPP_
#define INC_ERA_SYNCER_HANDLER_HPP_

#include <ERa/ERaSyncer.hpp>

#define ERA_TEMPLATABLE_SYNCER_LOOP(method, ...) \
    ERaSyncer* syncer = this->mFirstSyncer;      \
    while (syncer != NULL) {                     \
        ERaSyncer* next = syncer->getNext();     \
        syncer->method(__VA_ARGS__);             \
        syncer = next;                           \
    }

class ERaSyncerHandler
{
#if defined(ERA_HAS_FUNCTIONAL_H)
    typedef std::function<void(const char*, const char*)> MessageCallback_t;
#else
    typedef void (*MessageCallback_t)(const char*, const char*);
#endif

public:
    ERaSyncerHandler()
        : mFirstSyncer(NULL)
        , mLastSyncer(NULL)
    {}
    ~ERaSyncerHandler()
    {}

protected:
    void message(const ERaDataBuff& topic, const char* payload) {
        ERA_TEMPLATABLE_SYNCER_LOOP(message, topic, payload)
    }

    void save(const void* data, size_t size) {
        ERA_TEMPLATABLE_SYNCER_LOOP(save, data, size)
    }

    void load(void* data, size_t size) {
        ERA_TEMPLATABLE_SYNCER_LOOP(load, data, size)
    }

    void addSyncer(ERaSyncer& rSyncer) {
        this->addSyncer(&rSyncer);
    }

    void addSyncer(ERaSyncer* pSyncer) {
        if (this->mLastSyncer == NULL) {
            this->mFirstSyncer = pSyncer;
        }
        else {
            this->mLastSyncer->setNext(pSyncer);
        }
        this->mLastSyncer = pSyncer;
    }

    void setAuth(const char* auth) {
        ERA_TEMPLATABLE_SYNCER_LOOP(setAuth, auth)
    }

    void setTopic(const char* topic) {
        ERA_TEMPLATABLE_SYNCER_LOOP(setTopic, topic)
    }

    void onMessage(MessageCallback_t cb) {
        ERA_TEMPLATABLE_SYNCER_LOOP(onMessage, cb)
    }

private:
    ERaSyncer* mFirstSyncer;
    ERaSyncer* mLastSyncer;
};

#endif /* INC_ERA_SYNCER_HANDLER_HPP_ */
