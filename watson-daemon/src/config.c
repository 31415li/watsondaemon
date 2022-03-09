#include "config.h"

#include <syslog.h>

static int tranfer_config_fields(struct uci_context *context, struct uci_section *section, IoTPConfig *config) {
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

void clear_config(IoTPConfig *config) {
    IOTPRC rc;
    rc = IoTPConfig_clear(config);
    if (rc != IOTPRC_SUCCESS) {
        syslog(LOG_ERR, "Could not clean up IoTP config object: %s",
               IOTPRC_toString(rc));
    }
}