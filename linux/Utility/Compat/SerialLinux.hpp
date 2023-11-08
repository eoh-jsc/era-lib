#ifndef INC_SERIAL_LINUX_HPP_
#define INC_SERIAL_LINUX_HPP_

#ifdef __cplusplus
extern "C" {
#endif

int  serialOpen(const char* device, const int baud);
void serialFlush(const int fd);
void serialClose(const int fd);
void serialPutchar(const int fd, const unsigned char c);
void serialPuts(const int fd, const char* s);
void serialPrintf(const int fd, const char* message, ...);
int  serialDataAvail(const int fd);
int  serialGetchar(const int fd);

#ifdef __cplusplus
}
#endif

#endif /* INC_SERIAL_LINUX_HPP_ */
