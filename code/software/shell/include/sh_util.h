#ifndef SH_UTIL_H
#define SH_UTIL_H

#include <stdbool.h>
#include <stdlib.h>

#include "kmain.h"

#define sh_prompt_input(prompt, buf, n) printf(prompt);         \
                                        sh_get_inputn(buf, n);  \
                                        printf("\n");           \

#define SHELL_STATUS_ARGS_REQ   -5 // Required arguments missing
#define SHELL_STATUS_OOM        -4 // Out of memory
#define SHELL_STATUS_NOT_FOUND  -3 // Command not found
#define SHELL_STATUS_PANIC      -2 // Something is wrong, panic (message/halt)
#define SHELL_STATUS_ERROR      -1 // Error, but not fatal
#define SHELL_STATUS_OK          0 // Ok, nothing went wrong
#define SHELL_STATUS_EXIT        1 // Ok, user exited

void sh_get_inputn(char *out, size_t n);
char *friendly_size(uint32_t v);
unsigned int next_token_index(char *in, size_t off, char token);
char *strncpy_slice(char *src, size_t off, size_t n);
unsigned int count_tokens(char *in, char token);
bool sh_strncmp(const char *cmp, const char *buf, int n);
const char *parse_err_str(int err);
void strip_newline(char *str);

#endif
