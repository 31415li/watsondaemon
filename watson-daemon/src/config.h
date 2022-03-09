#ifndef __CONFIG_H
#define __CONFIG_H

#include <iotp_device.h>
#include <uci.h>

IoTPConfig *load_iotp_config(void);
void clear_config(IoTPConfig *config);

#endif