/*
 * Copyright (c) 2014, ARM Limited and Contributors. All rights reserved.
 * Copyright (c) 2015-2016, Cortina Access. All rights reserved.
 * Copyright (c) 2017, Realtek. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * Neither the name of ARM nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific
 * prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <asm_macros.S>
#include <platform_def.h>


    .local    platform_normal_stacks
    .globl    plat_set_my_stack
    .globl    plat_get_my_stack

    .set      noreorder
    /* -----------------------------------------------------
     * unsigned long plat_get_my_stack ()
     *
     * For cold-boot BL images, only the primary CPU needs a
     * stack. This function returns the stack pointer for a
     * stack allocated in device memory.
     * -----------------------------------------------------
     */
func plat_get_my_stack
    get_up_stack platform_normal_stacks, PLATFORM_STACK_SIZE
    jr $ra
    nop
endfunc plat_get_my_stack

    /* -----------------------------------------------------
     * void plat_set_my_stack ()
     *
     * For cold-boot BL images, only the primary CPU needs a
     * stack. This function sets the stack pointer to a stack
     * allocated in normal memory.
     * -----------------------------------------------------
     */
func plat_set_my_stack
    get_up_stack platform_normal_stacks, PLATFORM_STACK_SIZE
    move $sp, $v0
    jr $ra
    nop
endfunc plat_set_my_stack

    /* -----------------------------------------------------
     * Single cpu stack in normal memory.
     * Used for C code during boot, PLATFORM_STACK_SIZE bytes
     * are allocated
     * -----------------------------------------------------
     */
declare_stack platform_normal_stacks, tzfw_normal_stacks, \
        PLATFORM_STACK_SIZE, 1
