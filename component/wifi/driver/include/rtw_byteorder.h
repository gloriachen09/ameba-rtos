/**
  ******************************************************************************
  * @file    rtw_byteorder.h
  * @author
  * @version
  * @date
  * @brief
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

#ifndef _RTL871X_BYTEORDER_H_
#define _RTL871X_BYTEORDER_H_

#include "basic_types.h"

#if !defined(CONFIG_PLATFORM_MSTAR_TITANIA12)

#define RTW_GET_BE16(a) ((uint16_t) (((a)[0] << 8) | (a)[1]))

#define RTW_GET_LE16(a) ((uint16_t) (((a)[1] << 8) | (a)[0]))
#define RTW_PUT_LE16(a, val)			\
	do {					\
		(a)[1] = ((uint16_t) (val)) >> 8;	\
		(a)[0] = ((uint16_t) (val)) & 0xff;	\
	} while (0)

#define RTW_GET_BE24(a) ((((uint32_t) (a)[0]) << 16) | (((uint32_t) (a)[1]) << 8) | \
			 ((uint32_t) (a)[2]))

__inline static u16  ___swab16(u16 x)
{
	u16 __x = x;
	return
		((u16)(
			 (((u16)(__x) & (u16)0x00ffU) << 8) |
			 (((u16)(__x) & (u16)0xff00U) >> 8)));

}

__inline static u32  ___swab32(u32 x)
{
	u32 __x = (x);
	return ((u32)(
				(((u32)(__x) & (u32)0x000000ffUL) << 24) |
				(((u32)(__x) & (u32)0x0000ff00UL) <<  8) |
				(((u32)(__x) & (u32)0x00ff0000UL) >>  8) |
				(((u32)(__x) & (u32)0xff000000UL) >> 24)));
}

__inline static u64  ___swab64(u64 x)
{
	u64 __x = (x);

	return
		((u64)(\
			   (u64)(((u64)(__x) & (u64)0x00000000000000ffULL) << 56) | \
			   (u64)(((u64)(__x) & (u64)0x000000000000ff00ULL) << 40) | \
			   (u64)(((u64)(__x) & (u64)0x0000000000ff0000ULL) << 24) | \
			   (u64)(((u64)(__x) & (u64)0x00000000ff000000ULL) <<  8) | \
			   (u64)(((u64)(__x) & (u64)0x000000ff00000000ULL) >>  8) | \
			   (u64)(((u64)(__x) & (u64)0x0000ff0000000000ULL) >> 24) | \
			   (u64)(((u64)(__x) & (u64)0x00ff000000000000ULL) >> 40) | \
			   (u64)(((u64)(__x) & (u64)0xff00000000000000ULL) >> 56)));
	\
}
#endif // CONFIG_PLATFORM_MSTAR_TITANIA12

#ifndef __arch__swab16
__inline static u16 __arch__swab16(u16 x)
{
	return ___swab16(x);
}

#endif

#ifndef __arch__swab32
__inline static u32 __arch__swab32(u32 x)
{
	u32 __tmp = (x) ;
	return ___swab32(__tmp);
}
#endif

#ifndef __arch__swab64

__inline static u64 __arch__swab64(u64 x)
{
	u64 __tmp = (x) ;
	return ___swab64(__tmp);
}


#endif

#ifndef __swab16
#define __swab16(x) __fswab16(x)
#define __swab32(x) __fswab32(x)
#define __swab64(x) __fswab64(x)
#endif	// __swab16

__inline static u16 __fswab16(u16 x)
{
	return __arch__swab16(x);
}

__inline static u32 __fswab32(u32 x)
{
	return __arch__swab32(x);
}


/*
 * inside the kernel, we can use nicknames;
 * outside of it, we must avoid POSIX namespace pollution...
 */

#define cpu_to_le64 __cpu_to_le64
#define le64_to_cpu __le64_to_cpu
#define cpu_to_le32 __cpu_to_le32
#define le32_to_cpu __le32_to_cpu
#define cpu_to_le16 __cpu_to_le16
#define le16_to_cpu __le16_to_cpu
#define cpu_to_be64 __cpu_to_be64
#define be64_to_cpu __be64_to_cpu
#define cpu_to_be32 __cpu_to_be32
#define be32_to_cpu __be32_to_cpu
#define cpu_to_be16 __cpu_to_be16
#define be16_to_cpu __be16_to_cpu
#define cpu_to_le64p __cpu_to_le64p
#define le64_to_cpup __le64_to_cpup
#define cpu_to_le32p __cpu_to_le32p
#define le32_to_cpup __le32_to_cpup
#define cpu_to_le16p __cpu_to_le16p
#define le16_to_cpup __le16_to_cpup
#define cpu_to_be64p __cpu_to_be64p
#define be64_to_cpup __be64_to_cpup
#define cpu_to_be32p __cpu_to_be32p
#define be32_to_cpup __be32_to_cpup
#define cpu_to_be16p __cpu_to_be16p
#define be16_to_cpup __be16_to_cpup
#define cpu_to_le64s __cpu_to_le64s
#define le64_to_cpus __le64_to_cpus
#define cpu_to_le32s __cpu_to_le32s
#define le32_to_cpus __le32_to_cpus
#define cpu_to_le16s __cpu_to_le16s
#define le16_to_cpus __le16_to_cpus
#define cpu_to_be64s __cpu_to_be64s
#define be64_to_cpus __be64_to_cpus
#define cpu_to_be32s __cpu_to_be32s
#define be32_to_cpus __be32_to_cpus
#define cpu_to_be16s __cpu_to_be16s
#define be16_to_cpus __be16_to_cpus

#ifndef __LITTLE_ENDIAN
#define __LITTLE_ENDIAN 1234
#endif
#ifndef __LITTLE_ENDIAN_BITFIELD
#define __LITTLE_ENDIAN_BITFIELD
#endif

#ifndef __constant_htonl

#define __cpu_to_le64(x) ((u64)(x))
#define __le64_to_cpu(x) ((u64)(x))
#define __cpu_to_le32(x) ((u32)(x))
#define __le32_to_cpu(x) ((u32)(x))
#define __cpu_to_le16(x) ((u16)(x))
#define __le16_to_cpu(x) ((u16)(x))
#define __cpu_to_be64(x) __swab64((x))
#define __be64_to_cpu(x) __swab64((x))
#define __cpu_to_be32(x) __swab32((x))
#define __be32_to_cpu(x) __swab32((x))
#define __cpu_to_be16(x) __swab16((x))
#define __be16_to_cpu(x) __swab16((x))
#define __cpu_to_le64p(x) (*(u64*)(x))
#define __le64_to_cpup(x) (*(u64*)(x))
#define __cpu_to_le32p(x) (*(u32*)(x))
#define __le32_to_cpup(x) (*(u32*)(x))
#define __cpu_to_le16p(x) (*(u16*)(x))
#define __le16_to_cpup(x) (*(u16*)(x))
#define __cpu_to_be64p(x) __swab64p((x))
#define __be64_to_cpup(x) __swab64p((x))
#define __cpu_to_be32p(x) __swab32p((x))
#define __be32_to_cpup(x) __swab32p((x))
#define __cpu_to_be16p(x) __swab16p((x))
#define __be16_to_cpup(x) __swab16p((x))
#define __cpu_to_le64s(x) do {} while (0)
#define __le64_to_cpus(x) do {} while (0)
#define __cpu_to_le32s(x) do {} while (0)
#define __le32_to_cpus(x) do {} while (0)
#define __cpu_to_le16s(x) do {} while (0)
#define __le16_to_cpus(x) do {} while (0)
#define __cpu_to_be64s(x) __swab64s((x))
#define __be64_to_cpus(x) __swab64s((x))
#define __cpu_to_be32s(x) __swab32s((x))
#define __be32_to_cpus(x) __swab32s((x))
#define __cpu_to_be16s(x) __swab16s((x))
#define __be16_to_cpus(x) __swab16s((x))
#endif	// __constant_htonl

#endif /* _RTL871X_BYTEORDER_H_ */

