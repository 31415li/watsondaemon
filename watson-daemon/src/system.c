#include "system.h"

#include <libubox/blobmsg_json.h>
#include <syslog.h>

enum {
    INFO_MEMORY,
    __INFO_MAX
};

enum {
    MEMORY_TOTAL,
    MEMORY_FREE,
    __MEMORY_MAX
};

static struct blobmsg_policy info_policy[__INFO_MAX] = {
    [INFO_MEMORY] = { .name = "memory", .type = BLOBMSG_TYPE_TABLE }
};

static struct blobmsg_policy memory_policy[__MEMORY_MAX] = {
    [MEMORY_TOTAL] = { .name = "total", .type = BLOBMSG_TYPE_INT64 },
    [MEMORY_FREE]  = { .name = "free",  .type = BLOBMSG_TYPE_INT64 }
};

static void parse_system_data(struct ubus_request *req, int type, struct blob_attr *msg) {
    system_data *data = (system_data*) req->priv;
    struct blob_attr *info_table[__INFO_MAX];
    struct blob_attr *memory_table[__MEMORY_MAX];

    blobmsg_parse(info_policy, __INFO_MAX, info_table, blob_data(msg), blob_len(msg));

    if (!info_table[INFO_MEMORY]) {
        goto err;
    }

    blobmsg_parse(memory_policy, __MEMORY_MAX, memory_table, 
                  blobmsg_data(info_table[INFO_MEMORY]),
                  blobmsg_len(info_table[INFO_MEMORY]));

    if (!memory_table[MEMORY_TOTAL] || !memory_table[MEMORY_FREE]) {
        goto err;
    }

    data->valid = true;
    data->total_memory = blobmsg_get_u64(memory_table[MEMORY_TOTAL]);
    data->free_memory  = blobmsg_get_u64(memory_table[MEMORY_FREE]);
    return;

err:
    syslog(LOG_ERR, "Could not fetch system data from ubus");

    data->valid = false;
    data->total_memory = 0;
    data->free_memory = 0;
}

int fetch_system_data(struct ubus_context *ctx, system_data *out_data) {
    const char * const service_name = "system";
    const char * const method_name = "info";
    uint32_t id;
    int err;

    err = ubus_lookup_id(ctx, service_name, &id);
    if (err) {
        syslog(LOG_ERR, "Could not find '%s' ubus service", service_name);
        return 1;
    }

    err = ubus_invoke(ctx, id, method_name, NULL, (void*) parse_system_data, out_data, 3000);
    if (err) {
        syslog(LOG_ERR, "Could not invoke ubus method");
        return 1;
    }

    return 0;
}