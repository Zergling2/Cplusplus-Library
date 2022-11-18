#pragma once

#define _WINSOCKAPI_
#include <Windows.h>

#define ONE_SECOND_IN_MILLISECONDS ((DWORD)(1000))

inline float ConvertToSeconds(DWORD dwMilliseconds)
{
	return (float)dwMilliseconds / ONE_SECOND_IN_MILLISECONDS;
}

inline DWORD ConvertToMilliseconds(float fSeconds)
{
	return (DWORD)(fSeconds * ONE_SECOND_IN_MILLISECONDS);
}

class CTime
{
public:
	CTime();
	void Update();
	inline DWORD GetDeltaTime();
private:
	DWORD dwDeltaTime;
	DWORD dwOldTime;
};

inline DWORD CTime::GetDeltaTime()
{
	return this->dwDeltaTime;
}

#undef _WINSOCKAPI_