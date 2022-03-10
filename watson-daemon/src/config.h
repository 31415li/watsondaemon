#ifndef __CONFIG_H
#define __CONFIG_H

#include <iotp_device.h>
#include "opts.h"

IoTPConfig *load_iotp_config(cli_args *args);
void clear_config(IoTPConfig *config);

#endif