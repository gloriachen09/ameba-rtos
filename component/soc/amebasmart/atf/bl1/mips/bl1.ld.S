/*
 * Copyright (c) 2013-2017, ARM Limited and Contributors. All rights reserved.
 * Copyright (c) 2017, Realtek. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <platform_def.h>

OUTPUT_FORMAT(PLATFORM_LINKER_FORMAT)
OUTPUT_ARCH(PLATFORM_LINKER_ARCH)
ENTRY(bl1_entrypoint)

MEMORY {
    ROM (rx): ORIGIN = BL1_RO_BASE, LENGTH = BL1_RO_LIMIT - BL1_RO_BASE
    RAM (rwx): ORIGIN = BL1_RW_BASE, LENGTH = BL1_RW_LIMIT - BL1_RW_BASE
}

SECTIONS
{
    . = BL1_RO_BASE;
    ASSERT(. == ALIGN(4096),
           "BL1_RO_BASE address is not aligned on a page boundary.")

#if SEPARATE_CODE_AND_RODATA
    .text . : {
        __TEXT_START__ = .;
        *bl1_entrypoint.o(.text*)
        *(.text*)
        *(.vectors)
        . = NEXT(4096);
        __TEXT_END__ = .;
     } >ROM

    .rodata . : {
        __RODATA_START__ = .;
        *(.rodata*)

        /* Ensure 8-byte alignment for descriptors and ensure inclusion */
        . = ALIGN(8);
        __PARSER_LIB_DESCS_START__ = .;
        KEEP(*(.img_parser_lib_descs))
        __PARSER_LIB_DESCS_END__ = .;

        /*
         * No need to pad out the .rodata section to a page boundary. Next is
         * the .data section, which can mapped in ROM with the same memory
         * attributes as the .rodata section.
         */
        __RODATA_END__ = .;
    } >ROM
#else
    ro . : {
        __RO_START__ = .;
        *bl1_entrypoint.o(.text*)
        *(.text*)
        *(.rodata*)

        /* Ensure 8-byte alignment for descriptors and ensure inclusion */
        . = ALIGN(8);
        __PARSER_LIB_DESCS_START__ = .;
        KEEP(*(.img_parser_lib_descs))
        __PARSER_LIB_DESCS_END__ = .;

        *(.vectors)
        __RO_END__ = .;
    } >ROM
#endif

    . = BL1_RW_BASE;
    ASSERT(BL1_RW_BASE == ALIGN(4096),
           "BL1_RW_BASE address is not aligned on a page boundary.")

    /*
     * The .data section gets copied from ROM to RAM at runtime.
     * Its LMA should be 16-byte aligned to allow efficient copying of 16-bytes
     * aligned regions in it.
     * Its VMA must be page-aligned as it marks the first read/write page.
     *
     * It must be placed at a lower address than the stacks if the stack
     * protector is enabled. Alternatively, the .data.stack_protector_canary
     * section can be placed independently of the main .data section.
     */
    .data . : ALIGN(16) {
        __DATA_RAM_START__ = .;
        *(.data*)
        __DATA_RAM_END__ = .;
    } >RAM AT>ROM

    stacks . (NOLOAD) : {
        __STACKS_START__ = .;
        *(tzfw_normal_stacks)
        __STACKS_END__ = .;
    } >RAM

    /*
     * The .bss section gets initialised to 0 at runtime.
     * Its base address should be 16-byte aligned for better performance of the
     * zero-initialization code.
     */
    .bss : ALIGN(16) {
        __BSS_START__ = .;
        *(.bss*)
        *(COMMON)
        __BSS_END__ = .;
    } >RAM

#if USE_COHERENT_MEM
    /*
     * The base address of the coherent memory section must be page-aligned (4K)
     * to guarantee that the coherent data are stored on their own pages and
     * are not mixed with normal data.  This is required to set up the correct
     * memory attributes for the coherent data page tables.
     */
    coherent_ram (NOLOAD) : ALIGN(4096) {
        __COHERENT_RAM_START__ = .;
        *(tzfw_coherent_mem)
        __COHERENT_RAM_END_UNALIGNED__ = .;
        /*
         * Memory page(s) mapped to this section will be marked
         * as device memory.  No other unexpected data must creep in.
         * Ensure the rest of the current memory page is unused.
         */
        . = NEXT(4096);
        __COHERENT_RAM_END__ = .;
    } >RAM
#endif

    __BL1_RAM_START__ = ADDR(.data);
    __BL1_RAM_END__ = .;

    __DATA_ROM_START__ = LOADADDR(.data);
    __DATA_SIZE__ = SIZEOF(.data);

    /*
     * The .data section is the last PROGBITS section so its end marks the end
     * of BL1's actual content in Trusted ROM.
     */
    __BL1_ROM_END__ =  __DATA_ROM_START__ + __DATA_SIZE__;
    ASSERT(__BL1_ROM_END__ <= BL1_RO_LIMIT,
           "BL1's ROM content has exceeded its limit.")

    __BSS_SIZE__ = SIZEOF(.bss);

#if USE_COHERENT_MEM
    __COHERENT_RAM_UNALIGNED_SIZE__ =
        __COHERENT_RAM_END_UNALIGNED__ - __COHERENT_RAM_START__;
#endif

    ASSERT(. <= BL1_RW_LIMIT, "BL1's RW section has exceeded its limit.")
}
