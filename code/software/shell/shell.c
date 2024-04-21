#include <stdbool.h>
#include <stdio.h>
#include <stdnoreturn.h>
#include <string.h>

#include "cmd.h"
#include "kmain.h"
#include "shell.h"

static const char *parse_err_str(int err) {
  const char *str;
  switch (err) {
  case SHELL_STATUS_NOT_FOUND:
    str = "command not found";
    break;
  case SHELL_STATUS_PANIC:
    str = "panic";
    break;
  case SHELL_STATUS_ERROR:
    str = "error";
    break;
  case SHELL_STATUS_OK:
    str = "ok";
    break;
  case SHELL_STATUS_EXIT:
    str = "exit";
    break;
  default:
    str = "unknown";
  }
  return str;
}

static inline void strip_newline(char *str) {
  for (unsigned int i = 0; i < strlen(str); i++)
    if (str[i] == '\n')
      str[i] = 0;
}

static void *sh_calloc(int n, size_t size) {
  void **p = (void **)Kernel->mem_alloc(size * n);
  for (int i = 0; i < n; i++) {
    p[i] = (void *)Kernel->mem_alloc(size);
    p[i] = 0;
  }
  return (void *)p;
}

static void *sh_malloc(size_t size) { return (void *)Kernel->mem_alloc(size); }

static inline void get_input(char *out) {
  memset(out, 0, INPUT_BUFFER_SIZE);
  char *res = fgets(out, INPUT_BUFFER_SIZE, 0);
  strip_newline(res);
  if (res == NULL)
    return;
}

void shell_loop(void) {
  char *buffer = (char *)sh_calloc(INPUT_BUFFER_SIZE, sizeof(char));

  while (true) {
    printf("> ");
    get_input(buffer);

    int status = execute_cmd(buffer);

    printf("status: (%d) %s\n", status, parse_err_str(status));
    if (status != SHELL_STATUS_OK) {
      continue;
    }
    if (status == SHELL_STATUS_PANIC)
      break;
  }

  printf("halted\n");
  __asm__ volatile("stop   #0x2700\n\t");
}
