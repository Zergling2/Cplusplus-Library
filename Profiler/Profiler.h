#ifndef __PROFILER_H__
#define __PROFILER_H__

#include <stdio.h>
#include <string>
#include <map>
#include <locale.h>
#include <Windows.h>
#include <strsafe.h>
#include <time.h>

class Profiler
{
private:
    struct ProfileSample;
public:
    Profiler();
    ~Profiler();
    void BeginRecord(const wchar_t* szTag);
    inline void EndRecord(const wchar_t* szTag);
    void SaveProfileToFile(const wchar_t* szFileName);
private:
    void RecordEndRequestHandler(const wchar_t* szTag, LARGE_INTEGER endTime);
    std::map<std::wstring, ProfileSample*> _DataMap;
    LONGLONG _Frequency;
    inline static const wchar_t szLine[] = L"----------------------------------------------------------------------------------------------------------\n";
    inline static const wchar_t szItems[] = L"                   Name                  |     Average    |       Min      |       Max      |    Call    \n";
private:
    struct ProfileSample
    {
        friend Profiler;
    public:
        ProfileSample(std::wstring tag);
    private:
        std::wstring tag; // 프로파일 샘플 이름
        ULONGLONG lastStartTime; // 프로파일 샘플 실행 시간.
        ULONGLONG totalTime; // 전체 사용시간 카운터 Time. (출력시 호출회수로 나누어 평균 구함)
        ULONGLONG minimumTime; // 최소 사용시간 카운터 Time. (초단위로 계산하여 저장 / [0] 가장최소 [1] 다음 최소 [2])
        ULONGLONG maximumTime; // 최대 사용시간 카운터 Time. (초단위로 계산하여 저장 / [0] 가장최대 [1] 다음 최대 [2])
        int numOfCalls; // 누적 호출 횟수
    };
};

inline void Profiler::EndRecord(const wchar_t* szTag)
{
    LARGE_INTEGER endTime;
    QueryPerformanceCounter(&endTime);

    RecordEndRequestHandler(szTag, endTime);
}

#endif // !__PROFILER_H__