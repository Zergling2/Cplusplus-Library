#ifndef __FORCE_CRASH_H__
#define __FORCE_CRASH_H__

#if defined(_WIN64)
typedef unsigned long long MEMADDR;
#else
typedef unsigned int MEMADDR;
#endif

inline void ForceCrash(int crashValue, MEMADDR address = 0x00000001)
{
	*(int*)address = crashValue;
}

#endif // !__FORCE_CRASH_H__
