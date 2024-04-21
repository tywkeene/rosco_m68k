#include <kernelapi.h>
#include <machine.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kmain.h"

#define syncprintf(...)                                                        \
  do {                                                                         \
    Kernel->disable_interrupts();                                              \
    printf(__VA_ARGS__);                                                       \
    Kernel->enable_interrupts();                                               \
  } while (0)

IKernel *Kernel;

extern void *_end;

extern void shell_loop(void);

static uint32_t tinit_stack;
static uint32_t tidle_stack;
static Task *shell_task;
static uint32_t shell_stack;

void debug_dump_task(Task *t) {
  syncprintf("t           @ 0x%08lx\n", (uint32_t)t);
  syncprintf(" -> stack   @ 0x%08lx\n", (uint32_t)t->stack_addr);
  syncprintf(" -> tid     @ 0x%08lx\n", (uint32_t)t->tid);
  syncprintf(" -> sig_w   @ 0x%08lx\n", (uint32_t)t->sig_wait);
  syncprintf(" -> ssize   @ 0x%08lx\n", (uint32_t)t->stack_size);
  syncprintf(" -> sbott   @ 0x%08lx\n", (uint32_t)t->stack_bottom);
  syncprintf(" -> prio    @ 0x%08lx\n", (uint32_t)t->priority);
}

static void shell_task_loop(void) { shell_loop(); }

static void idle(void) {
  while (1) {
#ifndef INSTANT_IDLE
    mcBusywait(BUSYWAIT_NOPS);
#endif
  }
}

static void init(void) {
  syncprintf("init task: scheduling initial task\n");
  Kernel->task_schedule(shell_task, shell_stack, USER_STACK_SIZE,
                        shell_task_loop);
}

static void set_up_tasks(void) {
  tinit_stack = Kernel->mem_alloc(SYS_STACK_SIZE);
  tidle_stack = Kernel->mem_alloc(SYS_STACK_SIZE);

  syncprintf("  ==> allocating task objects...\n");
  shell_task = (Task *)Kernel->alloc_sys_object(TASK_SLAB_BLOCKS);
  shell_stack = Kernel->mem_alloc(USER_STACK_SIZE);

  if (shell_task == NULL || shell_stack == 0) {
    syncprintf("  ==! failed to allocate memory for shell_task\n");
    mcHalt();
  }

  syncprintf("  ==> shell_task is at 0x%08lx\n", (uint32_t)shell_task);

  Kernel->task_init(shell_task, 0x01, TASK_1_PRIORITY);
  syncprintf("  <== tasks are initialized...\n");
}

static void init_pmm() {
  uint32_t free_start = (uint32_t)&_end;
  uint32_t free_aligned = free_start & 0xfffff000;

  syncprintf("initializing memory...\n");
  if (free_aligned < free_start) {
    free_aligned = free_aligned + 1024;
  }

  if (free_aligned >= 0xefc00) {
    printf("Out of memory\n");
    abort();
  }
  Kernel->mem_free(free_aligned, 0xf0000);
}

void kmain() {
  Kernel = get_kernel_api();

  init_pmm();
  set_up_tasks();

  printf("tinit_stack @ 0x%08lx\n", (uint32_t)tinit_stack);
  printf("tidle_stack @ 0x%08lx\n", (uint32_t)tidle_stack);

  dump_task(shell_task);

  while (true) {
    Kernel->start(tinit_stack, SYS_STACK_SIZE, init, tidle_stack,
                  SYS_STACK_SIZE, idle);
    printf("after Kernel->start()\n\n");
  }
}
