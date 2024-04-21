#ifndef KMAIN_H
#define KMAIN_H

#include <kernelapi.h>

#define SYS_STACK_SIZE 0x800
#define USER_STACK_SIZE 0x10000
// 0x100000 = 1mb

#define TASK_1_PRIORITY 0x02

#define BUSYWAIT_NOPS 50000

#ifdef DEBUG
#define dump_task(t) debug_dump_task(t)
#else
#define dump_task(t)
#endif

extern IKernel *Kernel;

#endif
