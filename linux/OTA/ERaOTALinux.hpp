#ifndef INC_ERA_OTA_LINUX_HPP_
#define INC_ERA_OTA_LINUX_HPP_

#include <Utility/ERaUtility.hpp>

template <class Proto, class Flash>
class ERaOTA
{
public:
    ERaOTA(Flash& _flash)
        : flash(_flash)
    {}
    ~ERaOTA()
    {}

    void begin(const char* url = nullptr) {
        ERA_FORCE_UNUSED(url);
        exit(27);
    }

protected:
private:
	inline
	const Proto& thisProto() const {
		return static_cast<const Proto&>(*this);
	}

	inline
	Proto& thisProto() {
		return static_cast<Proto&>(*this);
	}

    Flash& flash;
};

#endif /* INC_ERA_OTA_LINUX_HPP_ */
