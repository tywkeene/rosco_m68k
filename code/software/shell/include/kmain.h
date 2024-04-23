#ifndef KMAIN_H
#define KMAIN_H

#include <kernelapi.h>

#define SYS_STACK_SIZE 0x800
#define USER_STACK_SIZE 0x10000

#define BUSYWAIT_NOPS 50000

#ifdef DEBUG
#define dump_task(t) debug_dump_task(t)
#else
#define dump_task(t)
#endif

#define syncprintf(...)          \
  do {                           \
    k_api->disable_interrupts(); \
    printf(__VA_ARGS__);         \
    k_api->enable_interrupts();  \
  } while (0)

#define ASM_HALT                                        \
                 printf("%s:%d\n", __FILE__, __LINE__); \
                 printf("\thalting\n");                 \
                 mcDisableInterrupts();                 \
                 mcHalt();                              \

#define sh_panic(...)                      \
    printf("!! PANIC: " __VA_ARGS__ "\n"); \
    printf("\n");                          \
    ASM_HALT;                              \
    __builtin_unreachable();               \

#endif
