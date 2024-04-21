#ifndef SHELL_H
#define SHELL_H

#define INPUT_BUFFER_SIZE 64

#define SHELL_STATUS_NOT_FOUND -3
#define SHELL_STATUS_PANIC -2
#define SHELL_STATUS_ERROR -1
#define SHELL_STATUS_OK 0
#define SHELL_STATUS_EXIT 1

#define ASM_HALT __asm__ volatile("stop   #0x2700\n\t")

#endif
