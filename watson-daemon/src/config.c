#include "config.h"

#include <stdio.h>
#include <syslog.h>

static int tranfer_config_fields(cli_args *args, IoTPConfig *config) {
    const int field_count = 4;
    const char *cli_names[] = {"orgid", "typeid", "deviceid", "token"};
    const char *cli_fields[] = {args->org_id, args->type_id, args->device_id,
                                args->auth_token};
    const char *iotp_fields[] = {"identity.orgId", "identity.typeId",
                                 "identity.deviceId", "auth.token"};

    for (int i = 0; i < field_count; i++) {
        IOTPRC rc;
        const char *value = cli_fields[i];
        const char *cli_name = cli_names[i];
        const char *iotp_field = iotp_fields[i];

        if (!value) {
            syslog(LOG_CRIT, "Missing command line argument '%s'", cli_name);
            return 1;
        }

        rc = IoTPConfig_setProperty(config, iotp_field, value);
        if (rc != IOTPRC_SUCCESS) {
            syslog(LOG_CRIT, "Failed to set IoTP property '%s': %s", iotp_field,
                   IOTPRC_toString(rc));
            return 1;
        }
    }
    return 0;
}

IoTPConfig *load_iotp_config(cli_args *args) {
    IoTPConfig *config = NULL;
    IOTPRC rc;

    rc = IoTPConfig_create(&config, NULL);
    if (rc != IOTPRC_SUCCESS) {
        syslog(LOG_CRIT, "Could not create IoTP config object: %s",
               IOTPRC_toString(rc));
        return NULL;
    }

    int err = tranfer_config_fields(args, config);
    if (err) {
        goto err1;
    }

    return config;

err1:
    clear_config(config);
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