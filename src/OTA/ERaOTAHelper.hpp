#ifndef INC_ERA_OTA_HELPER_HPP_
#define INC_ERA_OTA_HELPER_HPP_

#include <Utility/ERaUtility.hpp>

class ERaOTAHelper
{
public:
    ERaOTAHelper()
    {}
    virtual ~ERaOTAHelper()
    {}

protected:
    const char* createUrl(const char* auth) {
        static char url[128] {0};
        if (!strlen(url)) {
#if defined(ERA_OTA_SSL)
            FormatString(url, "https://" ERA_DEFAULT_DOMAIN_SSL);
#else
            FormatString(url, "http://" ERA_DEFAULT_DOMAIN);
#endif
            FormatString(url, "/api/chip_manager");
            FormatString(url, "/firmware?code=%s", auth);
            FormatString(url, "&firm_version=%d.%d", ERA_MAJOR, ERA_MINOR);
            FormatString(url, "&board=%s", ERA_MODEL_TYPE);
        }
        return url;
    }
};

#endif /* INC_ERA_OTA_HELPER_HPP_ */
