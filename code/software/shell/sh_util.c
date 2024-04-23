#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <sh_util.h>
#include <shell.h>

void sh_get_inputn(char *out, size_t n) {
  memset(out, 0, n);
  char *res = fgets(out, n, 0);
  strip_newline(res);
  if (res == NULL)
    return;
}

// format size to "friendly" 4 char string (e.g, 321B, 4.2K, 42M or 3.1G)
char *friendly_size(uint32_t v) {
    static char size_str[8]; // friendly string buffer
    uint32_t units = 1;
    char unitlabel = 'B';

    if (v > 999) {
        if (v < (999 * 1024)) {
            units = 1024;
            unitlabel = 'K';
        } else if (v < (999 * 1024 * 1024)) {
            units = 1024 * 1024;
            unitlabel = 'M';
        } else {
            units = 1024 * 1024 * 1024;
            unitlabel = 'G';
        }
    }

    // if single digit, also give tenths
    uint32_t round = (units / 10) / 2;
    uint32_t iv = (v + round) / units;
    if (iv < 10 && units > 1) {
        uint32_t tenth_units = units / 10;
        uint32_t tv = (v + round - (iv * units)) / (tenth_units > 0 ? tenth_units : 1);
        snprintf(size_str, sizeof(size_str), "%u.%u%c", iv, tv, unitlabel);
    } else {
        round = (units / 1000) / 2;
        iv = (v + round) / units;
        snprintf(size_str, sizeof(size_str), "%u%c", iv, unitlabel);
    }
    return size_str;
}

unsigned int next_token_index(char *in, size_t off, char token){
    unsigned int i = off;
    if (in[i] == token)
        i++;
    for (; i < strlen(in); i++)
        if (in[i] == token)
            break;
    return i;
}

inline char *strncpy_slice(char *src, size_t off, size_t n){
    char *dst = (char *)sh_calloc(n+1, sizeof(char));
    size_t max = off + n;
    size_t i = off;
    size_t j = 0;

    for (; i < max && j < n; i++, j++)
        dst[j] = src[i];
    dst[j+1] = '\0';
    return dst;
}

inline unsigned int count_tokens(char *in, char token){
    unsigned int count = 0;
    for (unsigned int i = 0; i < strlen(in); i++)
        if (in[i] == token)
            count++;
    return count;
}

inline bool sh_strncmp(const char *cmp, const char *buf, int n) {
    for (int i = 0; i < n; i++) {
        if (cmp[i] != buf[i])
            return false;
    }
    return true;
}

inline void strip_newline(char *str) {
    for (unsigned int i = 0; i < strlen(str); i++)
        if (str[i] == '\n')
            str[i] = '\0';
}

inline const char *parse_err_str(int err) {
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
