
#include "opts.h"

#include <popt.h>
#include <syslog.h>

static const char args_doc[] = "--orgid=<org ID> --typeid=<type ID> "
                               "--deviceid=<device ID> --token=<auth token>";

#define OPT_ORGID 1
#define OPT_TYPEID 2
#define OPT_DEVICEID 3
#define OPT_TOKEN 4

static const struct poptOption options[] = {
    {"orgid", '\0', POPT_ARG_STRING, NULL, OPT_ORGID, "IoTP organization ID",
     "<org ID>"},
    {"typeid", '\0', POPT_ARG_STRING, NULL, OPT_TYPEID, "IoTP device type ID",
     "<type ID>"},
    {"deviceid", '\0', POPT_ARG_STRING, NULL, OPT_DEVICEID, "IoTP device ID",
     "<device ID>"},
    {"token", '\0', POPT_ARG_STRING, NULL, OPT_TOKEN, "IoTP auth token",
     "<auth token>"},
    POPT_AUTOHELP POPT_TABLEEND};

static void apply_opt(poptContext ctx, int key, cli_args *out_args) {
    switch (key) {
    case OPT_ORGID:
        out_args->org_id = poptGetOptArg(ctx);
        break;

    case OPT_TYPEID:
        out_args->type_id = poptGetOptArg(ctx);
        break;

    case OPT_DEVICEID:
        out_args->device_id = poptGetOptArg(ctx);
        break;

    case OPT_TOKEN:
        out_args->auth_token = poptGetOptArg(ctx);
        break;
    }
}

int parse_cli_args(int argc, const char **argv, cli_args *out_args) {
    poptContext ctx;

    out_args->org_id = NULL;
    out_args->type_id = NULL;
    out_args->device_id = NULL;
    out_args->auth_token = NULL;

    ctx = poptGetContext("watson-daemon", argc, argv, options, 0);
    poptSetOtherOptionHelp(ctx, args_doc);

    int key;
    while ((key = poptGetNextOpt(ctx)) >= 0) {
        apply_opt(ctx, key, out_args);
    }
    if (key < -1) {
        syslog(LOG_CRIT, "Error parsing coomand line option '%s': %s",
               poptBadOption(ctx, POPT_BADOPTION_NOALIAS), poptStrerror(key));
        return 1;
    }
    return 0;
}
