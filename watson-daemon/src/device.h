#ifndef __DEVICE_H
#define __DEVICE_H

#include <iotp_device.h>

IoTPDevice *connect_device(IoTPConfig *config);
void disconnect_device(IoTPDevice *device);

#endif