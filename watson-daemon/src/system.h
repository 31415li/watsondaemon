#ifndef __SYSTEM_H
#define __SYSTEM_H

#include <libubus.h>

typedef struct system_data {
    bool valid;
    int64_t total_memory;
    int64_t free_memory;
} system_data;

int fetch_system_data(struct ubus_context *ctx, system_data *out_data);

#endif