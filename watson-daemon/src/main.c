#include <signal.h>
#include <stdlib.h>
#include <syslog.h>
#include <unistd.h>

#include "config.h"
#include "device.h"
#include "opts.h"
#include "system.h"

volatile sig_atomic_t terminated = 0;

void sigterm_handler() { terminated = 1; }

void init_sighandler() {
    struct sigaction action;
    action.sa_handler = sigterm_handler;
    sigemptyset(&action.sa_mask);
    sigaction(SIGTERM, &action, NULL);
}

int main(int argc, const char **argv) {
    cli_args args;
    IoTPConfig *config = NULL;
    IoTPDevice *device = NULL;
    struct ubus_context *ctx;
    int rc = EXIT_SUCCESS;

    init_sighandler();
    openlog("watson-daemon", LOG_PID, LOG_DAEMON);

    if (parse_cli_args(argc, argv, &args)) {
        rc = EXIT_FAILURE;
        goto err1;
    }

    syslog(LOG_INFO, "Starting watson-daemon");

    config = load_iotp_config(&args);
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

    while (!terminated) {
        system_data data;
        IOTPRC err;
        fetch_system_data(ctx, &data);

        char buf[256];
        sprintf(buf, "{\"total\": %lld, \"free\": %lld }", data.total_memory,
                data.free_memory);

        err = IoTPDevice_sendEvent(device, "status", buf, "json", QoS0, NULL);
        if (err) {
            syslog(LOG_ERR, "Could not send data to server: %s",
                   IOTPRC_toString(rc));
        }

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