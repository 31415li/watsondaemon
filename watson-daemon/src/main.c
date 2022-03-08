#include <stdio.h>
#include <iotp_device.h>
#include <uci.h>
#include <syslog.h>
#include <libubox/blobmsg_json.h>
#include <libubus.h>

int tranfer_config_fields(struct uci_context *context, struct uci_section *section, IoTPConfig *config) {
    const int field_count = 4;
    const char *uci_fields[] = {
        "orgId", "typeId", "deviceId", "token"
    };
    const char *iotp_fields[] = {
        "identity.orgId", "identity.typeId", "identity.deviceId", "auth.token"
    };

    for (int i = 0; i < field_count; i++) {
        IOTPRC rc;
        const char *value = uci_lookup_option_string(context, section, 
                                                     uci_fields[i]);
        if (!value) {
            syslog(LOG_CRIT, "Could not find field '%s' in config",
                   uci_fields[i]);
            return 1;
        }

        rc = IoTPConfig_setProperty(config, iotp_fields[i], value);
        if (rc != IOTPRC_SUCCESS) {
            syslog(LOG_CRIT, "Failed to set IoTP property '%s': %s",
                   iotp_fields[i], IOTPRC_toString(rc));
            return 1;
        }
    }
    return 0;
}

void clear_config(IoTPConfig *config) {
    IOTPRC rc;
    rc = IoTPConfig_clear(config);
    if (rc != IOTPRC_SUCCESS) {
        syslog(LOG_ERR, "Could not clean up IoTP config object: %s",
               IOTPRC_toString(rc));
    }
}

IoTPConfig *load_iotp_config(void) {
    const char *const config_name = "watson-daemon";
    const char *const section_name = "global";
    struct uci_context *context;
    struct uci_package *package;
    struct uci_section *section;
    IoTPConfig *config = NULL;
    IOTPRC rc;

    context = uci_alloc_context();
    if (!context) {
        syslog(LOG_CRIT, "Failed to allocate UCI context");
        return NULL;
    }

    uci_load(context, config_name, &package);
    if (!package) {
        syslog(LOG_CRIT, "Failed to load config file");
        goto err1;
    }

    section = uci_lookup_section(context, package, section_name);
    if (!section) {
        syslog(LOG_CRIT, "Could not find section '%s'", section_name);
        goto err1;
    }

    rc = IoTPConfig_create(&config, NULL);
    if (rc != IOTPRC_SUCCESS) {
        syslog(LOG_CRIT, "Could not create IoTP config object: %s",
               IOTPRC_toString(rc));
        goto err1;
    }

    int err = tranfer_config_fields(context, section, config);
    if (err) {
        goto err2;
    }

    uci_free_context(context);
    return config;

err2:
    clear_config(config);
err1:
    uci_free_context(context);
    return NULL;
}

void disconnect_device(IoTPDevice *device) {
    IOTPRC rc;
    
    rc = IoTPDevice_disconnect(device);
    if (rc != IOTPRC_SUCCESS) {
        syslog(LOG_ERR, "Could not disconnect IoTP device: %s",
               IOTPRC_toString(rc));
    }

    rc = IoTPDevice_destroy(device);
    if (rc != IOTPRC_SUCCESS) {
        syslog(LOG_ERR, "Could not clean up IoTP device object: %s",
               IOTPRC_toString(rc));
    }
}

IoTPDevice *connect_device(IoTPConfig *config) {
    IoTPDevice *device = NULL;
    IOTPRC rc;
    
    rc = IoTPDevice_create(&device, config);
    if (rc != IOTPRC_SUCCESS) {
        syslog(LOG_CRIT, "Could not create IoTP device: %s",
               IOTPRC_toString(rc));
        return NULL;
    }
    
    rc = IoTPDevice_connect(device);
    if (rc != IOTPRC_SUCCESS) {
        syslog(LOG_CRIT, "Could not connect to IoTP server: %s",
               IOTPRC_toString(rc));
        goto err1;
    }

    return device;

err1:
    IoTPDevice_destroy(device);
    return NULL;
}


typedef struct system_data {
    bool valid;
    int64_t total_memory;
    int64_t free_memory;
} system_data;

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

void parse_system_data(struct ubus_request *req, int type, struct blob_attr *msg) {
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

int main(void) {
    IoTPConfig *config = NULL;
    IoTPDevice *device = NULL;
    int rc = EXIT_SUCCESS;

    openlog("watson-daemon", LOG_PID, LOG_DAEMON);
    syslog(LOG_INFO, "Starting watson-daemon");

    config = load_iotp_config();
    if (!config) {
        rc = EXIT_FAILURE;
        goto err1;
    }

    device = connect_device(config);
    if (!device) {
        rc = EXIT_FAILURE;
        goto err2;
    }

    struct ubus_context *ctx = ubus_connect(NULL);

    while (true) {
        system_data data;
        fetch_system_data(ctx, &data);

        char buf[256];
        sprintf(buf, "{\"total\": %lld, \"free\": %lld }", data.total_memory, data.free_memory);
    
        IOTPRC err = IoTPDevice_sendEvent(device, "status", buf, "json", QoS0, NULL);

        sleep(2);
    }

    ubus_free(ctx);

    disconnect_device(device);
err2:
    clear_config(config);
err1:
    syslog(LOG_INFO, "Stopping watson-daemon");
    closelog();
    return rc;
}