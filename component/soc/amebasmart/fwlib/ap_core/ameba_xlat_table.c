/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "xlat_tables_v2.h"
#include "basic_types.h"

extern unsigned char __ram_nocache_start__[];
extern unsigned char __ram_nocache_end__[];
extern unsigned char __bss_start__[];
extern unsigned char __bss_end__[];
extern unsigned char __dram_text_start__[];
extern unsigned char __dram_text_end__[];
extern unsigned char __data_start__[];
extern unsigned char __dram_dynamic_app_text_start__[];
extern unsigned char __dram_dynamic_app_text_end__[];
extern unsigned char __dram_dynamic_app_text_size__[];

void mmap_add_region(unsigned long long base_pa, uintptr_t base_va, size_t size,
					 unsigned int attr);

void *_memset(void *dst, int val, size_t count);

/*|                       		| Memory Range            	| Definition in Translation Table   |
*|----------------------|-------------------------|------------------------------------------------|
*|Boot ROM				| 0x00000000 - 0x00100000-1 | Cacheable, write-back, write-allocate, RO, EXECUTE  	|
*|Flash					| 0x08000000 - 0x10000000-1 | Cacheable, write-back, write-allocate, RO, EXECUTE		|
*|SRAM					| 0x20000000 - 0x40000000-1 | Cacheable, write-back, write-allocate, RW, XN			|
*|Memory mapped device	| 0x40000000 - 0x60000000-1 | Device Memory									|
*|NS code				| __text_start__ - __text_end__ | Cacheable, write-back, write-allocate, RO, EXECUTE	|
*|Non-cache memory		| __ram_nocache_start__ - __ram_nocache_end__ | Non-cacheable, RW				|
*|Other DRAM			| 0x60000000 - 0x80000000-1 | Cacheable, write-back, write-allocate, RW, XN			|
*|Memory mapped device	| 0x80000000 - 0xFFFFFFFF   | Device Memory										|
*
******************************************************************************/

void setupMMUTable(int coreID)
{
	if (coreID == 1) {
		/* core0: clear bss, create MMU table */
		_memset((void *)__bss_start__, 0, (size_t)__bss_end__ - (size_t)__bss_start__);

		/* NOTE: For RW Normal Memory, default XN. For RO Normal Memory, it can be EXC/XN */
//		mmap_add_region(0x00000000, 0x00000000, 0x00100000, MT_CODE | MT_NS); // Non-Secure CPU do not need it
		mmap_add_region(0x08000000, 0x08000000, 0x10000000 - 0x08000000, MT_CODE | MT_NS);
		mmap_add_region(0x20000000, 0x20000000, 0x40000000 - 0x20000000, MT_MEMORY | MT_RW | MT_NS);
		mmap_add_region(0x40000000, 0x40000000,	0x60000000 - 0x40000000, MT_DEVICE | MT_RW | MT_NS);

		mmap_add_region((uint64_t)((int)__dram_text_start__), (uintptr_t)__dram_text_start__, (size_t)__dram_text_end__ - (size_t)__dram_text_start__,  \
						MT_CODE | MT_NS);
		mmap_add_region((uint64_t)((int)__ram_nocache_start__), (uintptr_t)__ram_nocache_start__, (size_t)__ram_nocache_end__ - (size_t)__ram_nocache_start__,  \
						MT_NON_CACHEABLE | MT_RW | MT_NS);
		if ((size_t)__dram_dynamic_app_text_size__ > 0) {
			mmap_add_region((uint64_t)((int)__dram_dynamic_app_text_start__ - 0x20), (uintptr_t)__dram_dynamic_app_text_start__ - 0x20,
							(size_t)__dram_dynamic_app_text_end__ - (size_t)__dram_dynamic_app_text_start__ + 0x20,  \
							MT_CODE | MT_NS);
		}
		mmap_add_region(0x60000000, 0x60000000,	0x80000000 - 0x60000000, MT_MEMORY | MT_RW | MT_NS);

		mmap_add_region(0x80000000, 0x80000000,	0x80000000, MT_DEVICE | MT_RW | MT_NS);

		init_xlat_tables();

	}
	/* core0 & 1: set BBT0 */
	enable_mmu_svc_mon(0);
}
