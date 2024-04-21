#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <machine.h>

#include "shell.h"
#include "cmd.h"

#define sizeof_array(x) (sizeof(x) / sizeof(*x))

enum {
    CMD_REBOOT,
    CMD_HALT,
    CMD_HELP,
    NUM_CMD,
};

static struct {
    const char *str;
    const char *help;
    cmd_fn_t fn;
}cmd_lut[NUM_CMD] = {
    [CMD_REBOOT] = { "reboot", "perform a warm reboot of the machine", cmd_fn_reboot },
    [CMD_HALT] = { "halt", "halt the machine", cmd_fn_halt },
    [CMD_HELP] = { "help", "print help messages of all commands", cmd_fn_help },
};

static inline bool sh_strncmp(const char *cmp, const char *buf, int n) {
  for (int i = 0; i < n; i++) {
    if (cmp[i] != buf[i])
      return false;
  }
  return true;
}

static void cmd_fn_halt(void *args) {
    (void) args;
    printf("halted\n");
    __asm__ volatile ("stop   #0x2700\n\t");
}

static void cmd_fn_help(void *args) {
    (void) args;
    printf("supported commands:\n");
    for (int i = 0; i < NUM_CMD; i++)
        printf("\t%s:\t%s\n", cmd_lut[i].str, cmd_lut[i].help);
}

static noreturn void cmd_fn_reboot(void *args) {
    (void) args;
    _WARM_BOOT();
    __builtin_unreachable();
}

int execute_cmd(char *in) {
    for (unsigned int i = 0; i < NUM_CMD; i++) {
        if (sh_strncmp(in, cmd_lut[i].str, strlen(in))) {
            cmd_fn_t fn = cmd_lut[i].fn;
            if (fn == NULL)
                return SHELL_STATUS_NOT_FOUND;
            (void) fn(NULL);
        }
    }
    return SHELL_STATUS_OK;
}
