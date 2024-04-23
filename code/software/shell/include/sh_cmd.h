#ifndef CMD_H
#define CMD_H

#include <stdlib.h>
#include <stdint.h>

typedef int (*cmd_fn_t)(int argc, char **argv);

static int cmd_fn_reboot(int argc, char **argv);
static int cmd_fn_halt(int argc, char **argv);
static int cmd_fn_help(int argc, char **argv);
static int cmd_fn_jump(int argc, char **argv);
static int cmd_fn_memdump(int argc, char **argv);
static int cmd_fn_malloc(int argc, char **argv);
static int cmd_fn_panic(int argc, char **argv);

int execute_cmd(char *in);

#endif
