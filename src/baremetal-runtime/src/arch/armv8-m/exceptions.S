/**
 * SPDX-License-Identifier: Apache-2.0U
 * Copyright (c) Bao Project and Contributors. All rights reserved.
 */

#include <sysregs.h>
#include <arch/irq.h>

.syntax unified
.text

.macro define_handler name
    .thumb
    .thumb_func
    .align  2
    .global \name
    .type   \name, %function
    \name:
.endm

.balign 0x1000
.global _exception_vector
_exception_vector:
    .word _stack_top
    .word _start
    .word _exception_handler  /* NMI_Handler */
    .word _hf_exception_handler  /* HardFault_Handler */
    .word _mem_exception_handler  /* MemManage_Handler */
    .word _bus_exception_handler  /* BusFault_Handler */
    .word _exception_handler  /* UsageFault_Handler */
    .word _exception_handler  /* Reserved */
    .word _exception_handler  /* Reserved */
    .word _exception_handler  /* Reserved */
    .word _try1_exception_handler  /* Reserved */
    .word SVC_Handler  /* SVCall_Handler */
    .word _try2_exception_handler  /* DebugMon_Handler */
    .word _exception_handler  /* Reserved */
    .word PendSV_Handler  /* PendSV_Handler */
    .word _systick_handler
    /* Interrupts */
    .rept IRQ_NUM
    .word _irq_handler
    .endr

/* Handlers */
define_handler _exception_handler
    b   .

define_handler _mem_exception_handler
    b   .

define_handler _hf_exception_handler
    b   .

define_handler _bus_exception_handler
    b   .

define_handler _try1_exception_handler
    b   .

define_handler _try2_exception_handler
    b   .

define_handler _irq_handler
    // Get the IRQ number
    mrs r0, IPSR
    // Call the IRQ handler
    b   irq_handle
    // Should never reach here
    b   .

define_handler _systick_handler
    // Call the SysTick handler
    b SysTick_Handler
    // mov r0, #TIMER_IRQ_ID
    // b   irq_handle
    // // Should never reach here
    // b   .
