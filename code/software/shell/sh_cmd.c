#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <machine.h>

#include <shell.h>
#include <sh_cmd.h>
#include <sh_util.h>
#include <sh_malloc.h>

#define sizeof_array(x) (sizeof(x) / sizeof(*x))

#define check_null_args(req,argc,argv)  \
    if (argc < req || argv == NULL)     \
        return SHELL_STATUS_ARGS_REQ;

#define args_not_used()      \
                (void) argc; \
                (void) argv;

enum {
    CMD_HALT,
    CMD_HELP,
    CMD_MEMD,
    CMD_REBOOT,
    CMD_PANIC,
    CMD_MALLOC,
    NUM_CMD,
};

static struct {
    const char *str;
    const char *help;
    cmd_fn_t fn;
}cmd_lut[NUM_CMD] = {
    [CMD_HALT] = { "halt", "halt the machine", cmd_fn_halt },
    [CMD_HELP] = { "help", "print help messages of all commands", cmd_fn_help },
    [CMD_MEMD] = { "memdump", "print a hexdump of a region in memory", cmd_fn_memdump },
    [CMD_REBOOT] = { "reboot", "perform a warm reboot of the machine", cmd_fn_reboot },
    [CMD_PANIC] = { "panic", "force a panic and halt the machine", cmd_fn_panic },
    [CMD_MALLOC] = { "malloc", "allocate some memory and print pointer information", cmd_fn_malloc},
};

static inline bool isascii(int in) {
  if (in >= 0x20 && in <= 0x7E)
    return true;
  return false;
}

static inline uint8_t ascii_or_dot(uint8_t val) {
  if (isascii(val) && val != (char)0x00)
    return val;
  return '.';
}

static int cmd_fn_panic(int argc, char **argv) {
    args_not_used();
    return SHELL_STATUS_PANIC;
}

static int cmd_fn_malloc(int argc, char **argv) {
    args_not_used();

    char input[4];
    sh_prompt_input("num bytes: ", input, 4);
    uint32_t size = strtol(input, NULL, 10);

    char *data = (char *)sh_malloc(size);
    if (data == NULL)
        return SHELL_STATUS_PANIC;

    printf("sh_malloc(%d) returned pointer: 0x%p\n", size, (uint32_t)data);
    memset((void *)data, 'X', size);

    uint32_t *p = (uint32_t *)data;
    for (unsigned int i = 0; i < (size/4); i++) {
        printf("[0x%p]: %#04x %#04x %#04x %#04x ", (void *)p, (uint8_t)(*p >> 24),
                (uint8_t)(*p >> 16), (uint8_t)(*p >> 8), (uint8_t)(*p));
        printf("| %c %c %c %c |\n", ascii_or_dot((uint8_t)(*p >> 24)),
                ascii_or_dot((uint8_t)(*p >> 16)), ascii_or_dot((uint8_t)(*p >> 8)),
                ascii_or_dot((uint8_t)(*p)));
        p++;
    }

    return SHELL_STATUS_OK;
}

static int cmd_fn_memdump(int argc, char **argv) {
    args_not_used();

    const uint32_t USER_RAM_START = 0x00001000;
    const uint32_t USER_RAM_END = 0x000FFFFF;
    char start_str[9];
    char end_str[9];

_get_input:
    memset(start_str, 0, 9);
    memset(end_str, 0, 9);

    printf("user ram is 0x%p - 0x%p\n", (void *)USER_RAM_START, (void *)USER_RAM_END);
    printf("enter start and end addr, or q<ENTER> to exit\n");

    printf("start addr: 0x");
    sh_get_inputn(start_str, 9);
    if (start_str[0] == 'q') {
        return SHELL_STATUS_EXIT;
    }
    printf("\n");

    printf("  end addr: 0x");
    sh_get_inputn(end_str, 9);
    if (end_str[0] == 'q') {
        return SHELL_STATUS_EXIT;
    }
    printf("\n");

    uint32_t *start = (uint32_t *)strtol(start_str, NULL, 16);
    uint32_t *end = (uint32_t *)strtol(end_str, NULL, 16);

    if (*start >= *end) {
        printf("start addr must be less than end addr (0x%p > 0x%p)\n",
                (void *)start, (void *)end);
        goto _get_input;
    }
    printf("--- dumping 0x%p to 0x%p ---\n", (void *)start, (void *)end);
    for (uint32_t *p = (uint32_t *)start; p < (uint32_t *)end; (void)p++) {
        printf("[0x%p]: %#04x %#04x %#04x %#04x ", (void *)p, (uint8_t)(*p >> 24),
                (uint8_t)(*p >> 16), (uint8_t)(*p >> 8), (uint8_t)(*p));
        printf("| %c %c %c %c |\n", ascii_or_dot((uint8_t)(*p >> 24)),
                ascii_or_dot((uint8_t)(*p >> 16)), ascii_or_dot((uint8_t)(*p >> 8)),
                ascii_or_dot((uint8_t)(*p)));
    }
    printf("-------------------  END  ------------------\n");
    return SHELL_STATUS_OK;
}

static noreturn int cmd_fn_reboot(int argc, char **argv) {
    args_not_used();
    printf("cmd_fn_reboot()");

    _WARM_BOOT();
    __builtin_unreachable();
}

static int cmd_fn_halt(int argc, char **argv) {
    args_not_used();
    printf("cmd_fn_halt()\n");
    ASM_HALT;
    __builtin_unreachable();
}

static int cmd_fn_help(int argc, char **argv) {
    args_not_used();

    printf("supported commands:\n");
    for (int i = 0; i < NUM_CMD; i++){
        printf("\t%s:  %s", cmd_lut[i].str, cmd_lut[i].help);
#ifdef DEBUG
        printf("\taddr: %#08x", cmd_lut[i].fn);
#endif
        printf("\n");
    }
    return SHELL_STATUS_OK;
}

int execute_cmd(char *in) {
    int status = SHELL_STATUS_NOT_FOUND;
    unsigned int sp = next_token_index(in, 0, ' ');
    char *cmd_str = strncpy_slice(in, 0, sp);

    for (unsigned int i = 0; i < NUM_CMD; i++) {
        if (sh_strncmp(cmd_str, cmd_lut[i].str, strlen(cmd_str))) {

            cmd_fn_t fn = cmd_lut[i].fn;
            if (fn == NULL){
                status = SHELL_STATUS_PANIC;
                break;
            }

            //            unsigned int argc = count_tokens(in, ' ');
            status = fn(0, NULL);
            break;
        }
    }
    return status;
}
