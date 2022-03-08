#include <stdio.h>
#include <iotp_device.h>
#include <uci.h>
#include <syslog.h>

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
    
    char *data = "{\"d\" : {\"SensorID\": \"Test\", \"Reading\": 7 }}";
    IOTPRC err = IoTPDevice_sendEvent(device, "status", data, "json", QoS1, NULL);
    
    disconnect_device(device);
err2:
    clear_config(config);
err1:
    syslog(LOG_INFO, "Stopping watson-daemon");
    closelog();
    return rc;
}