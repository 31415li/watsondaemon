#ifndef __OPTS_H
#define __OPTS_H

typedef struct cli_args {
    const char *org_id;
    const char *type_id;
    const char *device_id;
    const char *auth_token;
} cli_args;

int parse_cli_args(int argc, const char **argv, cli_args *out_args);

#endif