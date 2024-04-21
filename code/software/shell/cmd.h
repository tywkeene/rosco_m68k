#ifndef CMD_H
#define CMD_H

#include <stdlib.h>
#include <stdint.h>

typedef void (*cmd_fn_t)(void *);

static void cmd_fn_reboot(void *args);
static void cmd_fn_halt(void *args);
static void cmd_fn_help(void *args);

int execute_cmd(char *in);

#endif
