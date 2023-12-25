#ifndef INC_ERA_NETWORK_INFO_HPP_
#define INC_ERA_NETWORK_INFO_HPP_

#include <stdio.h>
#include <stddef.h>
#include <unistd.h>
#include <string.h>

void GetNetworkInfo(void);
const char* GetLocalIP(const char* interface);
const char* GetMACAddress(const char* interface);
const char* GetNetworkProtocol(void);
const char* GetSSIDNetwork(void);
int16_t GetRSSINetwork(void);

#endif /* INC_ERA_NETWORK_INFO_HPP_ */
