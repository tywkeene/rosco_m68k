# Make rosco_m68k example program for C
#
# Copyright (c) 2020-2022 Xark and contributors
# MIT LICENSE

ROSCO_M68K_DIR=../../..

SERIAL?=/dev/tty.usbserial-ABSCDIGP

ifndef ROSCO_M68K_DIR
$(info NOTE: ROSCO_M68K_DIR not set, using libs: ../../../code/software/libs)
ROSCO_M68K_DIR=$(ROSCO_M68K_DEFAULT_DIR)
else
$(info NOTE: Using ROSCO_M68K_DIR libs in: $(ROSCO_M68K_DIR))
endif

-include $(ROSCO_M68K_DIR)/code/software/software.mk

compile_commands:
	compiledb make

KERNEL_INCLUDE=$(ROSCO_M68K_DIR)/code/firmware/rosco_m68k_firmware/kernel/include/
KERNEL_LIBS=$(ROSCO_M68K_DIR)/code/firmware/rosco_m68k_firmware/kernel

EXTRA_LDFLAGS=--strip-debug
EXTRA_CFLAGS=-Wno-format-zero-length -Wno-format \
			 -Wno-implicit-function-declaration  \
			 -Wunused-variable -Wno-pedantic     \
			 -I./include -I./$(KERNEL_INCLUDE)   \
			 -L./$(KERNEL_LIBS)                  \
#			 -DDEBUG             				 \
			 -O3

EXTRA_LIBS=-lmachine
