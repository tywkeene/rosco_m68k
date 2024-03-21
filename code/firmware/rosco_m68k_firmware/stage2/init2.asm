;------------------------------------------------------------
;                                  ___ ___ _   
;  ___ ___ ___ ___ ___       _____|  _| . | |_ 
; |  _| . |_ -|  _| . |     |     | . | . | '_|
; |_| |___|___|___|___|_____|_|_|_|___|___|_,_| 
;                     |_____|       firmware v2
;------------------------------------------------------------
; Copyright (c)2020-2021 Ross Bamford and contributors
; See top-level LICENSE.md for licence information.
;
; This is the initialization code for stage 2. 
;------------------------------------------------------------
    include "../../../shared/rosco_m68k_public.asm"
    include "../rosco_m68k_private.asm"

    section .text.init

START::
    movea.l VECTORS_LOAD,A7             ; Stack to the default stack pointer vector
    lea.l   linit,A0
    jsr     (A0)
    lea.l   lmain,A0
    jmp     (A0)                        ; Stage 2 is go

red_led_off::
    ifd REVISION1X
    move.b  SDB_SYSFLAGS,D0             ; Get sysflags (high byte)
    and.b   #2,D0                       ; Turn off I1
    or.b    D0,MFP_GPDR                 ; Update GPDR
    else
    move.l  SDB_UARTBASE,A0
    move.b  #$08,W_OPR_RESETCMD(A0)     ; Turn off red LED on r2.x boards
    endif
    rts
