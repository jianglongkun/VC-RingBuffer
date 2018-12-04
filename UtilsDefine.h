#pragma once
#ifndef __UTILSDEFINE__H_
#define __UTILSDEFINE__H_

#include <stdint.h>
#include <stdio.h>
#include <string.h>

namespace Utils
{
#if (_MSC_VER >= 1400) && !defined(_WIN32_WCE)
#   include <intrin.h>
#   pragma intrinsic(_ReadWriteBarrier)
#   pragma intrinsic(_ReadBarrier)
#   pragma intrinsic(_WriteBarrier)
	/* note that MSVC intrinsics _ReadWriteBarrier(), _ReadBarrier(), _WriteBarrier() are just compiler barriers *not* memory barriers */
#   define FullMemoryBarrier()  _ReadWriteBarrier()
#   define ReadMemoryBarrier()  _ReadBarrier()
#   define WriteMemoryBarrier() _WriteBarrier()
#endif

#ifdef __APPLE__
#include <sys/types.h>
#include <libkern/OSAtomic.h>
#define FullMemoryBarrier() OSMemoryBarrier()
#define ReadMemoryBarrier() OSMemoryBarrier()
#define WriteMemoryBarrier() OSMemoryBarrier()
#endif
}

#endif