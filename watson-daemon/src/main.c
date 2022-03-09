#include <syslog.h>
#include "config.h"
#include "device.h"
#include "system.h"

int main(int argc, char** argv) {
    IoTPConfig *config = NULL;
    IoTPDevice *device = NULL;
    struct ubus_context *ctx;
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

    ctx = ubus_connect(NULL);
    if (!ctx) {
        rc = EXIT_FAILURE;
        syslog(LOG_CRIT, "Could not connect to UBUS");
        goto err3;
    }

    while (true) {
        system_data data;
        fetch_system_data(ctx, &data);

        char buf[256];
        sprintf(buf, "{\"total\": %lld, \"free\": %lld }", data.total_memory, data.free_memory);

        IOTPRC err = IoTPDevice_sendEvent(device, "status", buf, "json", QoS0, NULL);

        sleep(2);
    }

    ubus_free(ctx);
err3:
    disconnect_device(device);
err2:
    clear_config(config);
err1:
    syslog(LOG_INFO, "Stopping watson-daemon");
    closelog();
    return rc;
}