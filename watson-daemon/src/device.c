#include "device.h"

#include <syslog.h>

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