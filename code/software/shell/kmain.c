#include <kernelapi.h>
#include <machine.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <kmain.h>
#include <sh_malloc.h>
#include <sh_util.h>

IKernel *k_api;

extern void *_end;

extern void shell_loop(void);

static uint32_t tinit_stack;
static uint32_t tidle_stack;
static Task *shell_task;
static uint32_t shell_stack;

void debug_dump_task(Task *t) {
  syncprintf("task      : 0x%08lx\n", (uint32_t)t);
  syncprintf(" -> stack : 0x%08lx\n", (uint32_t)t->stack_addr);
  syncprintf(" -> tid   : 0x%08lx\n", (uint32_t)t->tid);
  syncprintf(" -> sig_w : 0x%08lx\n", (uint32_t)t->sig_wait);
  syncprintf(" -> ssize : 0x%08lx\n", (uint32_t)t->stack_size);
  syncprintf(" -> sbott : 0x%08lx\n", (uint32_t)t->stack_bottom);
  syncprintf(" -> prio  : 0x%08lx\n", (uint32_t)t->priority);
}

static void idle(void) {
  while (1) {
#ifndef INSTANT_IDLE
    mcBusywait(BUSYWAIT_NOPS);
#endif
  }
}

static void init(void) {
  syncprintf("init task: scheduling initial task\n");
  k_api->task_schedule(shell_task, shell_stack, USER_STACK_SIZE, shell_loop);
}

static void set_up_tasks(void) {
  tinit_stack = k_api->mem_alloc(SYS_STACK_SIZE);
  tidle_stack = k_api->mem_alloc(SYS_STACK_SIZE);

  syncprintf("allocating task objects...\n");
  shell_task = (Task *)k_api->alloc_sys_object(TASK_SLAB_BLOCKS);
  shell_stack = k_api->mem_alloc(USER_STACK_SIZE);

  if (shell_task == NULL || shell_stack == 0) {
    syncprintf("failed to allocate memory for shell_task\n");
    ASM_HALT;
  }

  k_api->task_init(shell_task, 0x01, 0x02);
  syncprintf("tasks are initialized...\n");
}

static void init_pmm() {
  uint32_t free_start = (uint32_t)&_end;
  uint32_t free_aligned = free_start & 0xfffff000;

  syncprintf("initializing memory...\n");
  if (free_aligned < free_start) {
    free_aligned = free_aligned + 1024;
  }

  if (free_aligned >= 0xefc00) {
    sh_panic("out of memory");
    abort();
  }
  k_api->mem_free(free_aligned, 0xf0000);
}

void kmain() {
  ensure_kernel_api();
  k_api = get_kernel_api();

  init_pmm();
  set_up_tasks();

#ifdef DEBUG
  debug_dump_task(shell_task);
#endif

  syncprintf("init task stack   : 0x%p - 0x%p (%s)\n", (uint32_t)tinit_stack,
             (uint32_t)tinit_stack + SYS_STACK_SIZE,
             friendly_size(SYS_STACK_SIZE));
  syncprintf("idle task stack   : 0x%p - 0x%p (%s)\n", (uint32_t)tidle_stack,
             (uint32_t)tidle_stack + SYS_STACK_SIZE,
             friendly_size(SYS_STACK_SIZE));
  syncprintf("shell task stack  : 0x%p - 0x%p (%s)\n", (uint32_t)shell_stack,
             (uint32_t)shell_stack + USER_STACK_SIZE,
             friendly_size(USER_STACK_SIZE));

  sh_init_malloc_heap();
  dump_task(shell_task);
  while (true) {
    k_api->start(tinit_stack, SYS_STACK_SIZE, init, tidle_stack, SYS_STACK_SIZE,
                 idle);
  }
  sh_panic("after k_api->start()\n\n");
}
