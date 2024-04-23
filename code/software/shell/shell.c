#include <stdbool.h>
#include <stdio.h>
#include <stdnoreturn.h>
#include <string.h>

#include <kmain.h>
#include <sh_cmd.h>
#include <sh_util.h>
#include <shell.h>

extern IKernel *k_api;
extern uint32_t bytes_free;

void shell_loop(void) {
  char *buffer = (char *)sh_calloc(INPUT_BUFFER_SIZE, sizeof(char));

  while (true) {
    printf("[%d bytes free]\n> ", bytes_free);
    sh_get_inputn(buffer, INPUT_BUFFER_SIZE);

    int status = execute_cmd(buffer);

    printf("%s (%d)\n", parse_err_str(status), status);
    switch (status) {
    case SHELL_STATUS_OK:
      continue;
    case SHELL_STATUS_ERROR:
      continue;
    case SHELL_STATUS_PANIC:
      sh_panic("shell returned panic status\n");
    }
  }
}
