#pragma warning (disable:6387)
#include "Profiler.h"

#include <strsafe.h>
#include <time.h>
#include <locale.h>
#include <stdio.h>

const wchar_t* Profiler::szItems = L"Name,Average,Min,Max,Call\n";

Profiler::Profiler()
{
    _wsetlocale(LC_ALL, L"ko-kr");
    LARGE_INTEGER f;
    QueryPerformanceFrequency(&f);
    this->_Frequency = f.QuadPart;
}

Profiler::~Profiler()
{
    std::map<CWideString<>, ProfileSample*>::iterator i = _DataMap.begin();
    std::map<CWideString<>, ProfileSample*>::iterator end = _DataMap.end();

    while (i != end)
    {
        ProfileSample* pDel = i->second;
        i = _DataMap.erase(i);
        delete pDel;
    }
}

void Profiler::BeginRecord(const wchar_t* szTag)
{
    auto find = _DataMap.find(szTag);
    ProfileSample* pSample;

    if (find == _DataMap.end())
    {
        pSample = new ProfileSample(szTag);
        _DataMap.emplace(szTag, pSample);
    }
    else
    {
        pSample = find->second;
    }

    LARGE_INTEGER currentTime;
    QueryPerformanceCounter(&currentTime);
    pSample->lastStartTime = currentTime.QuadPart;
}

void Profiler::RecordEndRequestHandler(const wchar_t* szTag, LARGE_INTEGER endTime)
{    
    auto find = _DataMap.find(szTag);
    ProfileSample* pSample;

    if (find == _DataMap.end())
    {
        wprintf(L"Profile sample information not found.");
        throw std::bad_exception();
    }
    else
    {
        pSample = find->second;
    }

    ULONGLONG interval = endTime.QuadPart - pSample->lastStartTime;
    pSample->totalTime += interval;

    if (pSample->numOfCalls == 0)
    {
        pSample->minimumTime = pSample->maximumTime = interval;
    }
    else
    {
        if (interval < pSample->minimumTime)
        {
            pSample->minimumTime = interval;
        }

        if (interval > pSample->maximumTime)
        {
            pSample->maximumTime = interval;
        }
    }

    pSample->numOfCalls += 1;
}

void Profiler::SaveProfileToFile(const wchar_t* szFileName)
{
    FILE* fp;
    wchar_t logBuffer[256];
    wchar_t szRealFileName[MAX_PATH + 4];
    StringCbPrintfW(szRealFileName, sizeof(szRealFileName), L"%s.csv", szFileName);

    errno_t e = _wfopen_s(&fp, szRealFileName, L"at");
    if (e != 0)
    {
        wprintf(L"_wfopen_s error!\n");
        exit(-999);
    }

    time_t base_time = time(NULL);
    tm base_date_local;
    e = localtime_s(&base_date_local, &base_time);
    if (e != 0)
    {
        wprintf(L"localtime_s error!\n");
        exit(-999);
    }

#ifdef _DEBUG
    #ifdef _WIN64
    StringCbPrintfW(logBuffer, sizeof(logBuffer), L"%04d-%02d-%02d %02d:%02d:%02d,,,x64 Debug mode,\n",
        base_date_local.tm_year + 1900, base_date_local.tm_mon + 1, base_date_local.tm_mday,
        base_date_local.tm_hour, base_date_local.tm_min, base_date_local.tm_sec);
    #else
    StringCbPrintfW(logBuffer, sizeof(logBuffer), L"%04d-%02d-%02d %02d:%02d:%02d,,,x86 Debug mode,\n",
        base_date_local.tm_year + 1900, base_date_local.tm_mon + 1, base_date_local.tm_mday,
        base_date_local.tm_hour, base_date_local.tm_min, base_date_local.tm_sec);
    #endif
#else
    #ifdef _WIN64
    StringCbPrintfW(logBuffer, sizeof(logBuffer), L"%04d-%02d-%02d %02d:%02d:%02d,,,x64 Release mode,\n",
        base_date_local.tm_year + 1900, base_date_local.tm_mon + 1, base_date_local.tm_mday,
        base_date_local.tm_hour, base_date_local.tm_min, base_date_local.tm_sec);
    #else
    StringCbPrintfW(logBuffer, sizeof(logBuffer), L"%04d-%02d-%02d %02d:%02d:%02d,,,x86 Release mode,\n",
        base_date_local.tm_year + 1900, base_date_local.tm_mon + 1, base_date_local.tm_mday,
        base_date_local.tm_hour, base_date_local.tm_min, base_date_local.tm_sec);
    #endif
#endif

    fwprintf(fp, logBuffer);
    fwprintf(fp, Profiler::szItems);

    std::map<CWideString<>, ProfileSample*>::iterator i = _DataMap.begin();
    std::map<CWideString<>, ProfileSample*>::iterator end = _DataMap.end();
    while (i != end)
    {
        ProfileSample* pSample = i->second;

        StringCbPrintfW(logBuffer, sizeof(logBuffer), L"%s,%f¥ìs,%f¥ìs,%f¥ìs,%d\n",
            pSample->tag.c_str(),
            pSample->numOfCalls <= 2 ? (pSample->totalTime) / (double)(_Frequency / 1000000) / pSample->numOfCalls
            :
            (pSample->totalTime - (pSample->maximumTime + pSample->minimumTime)) / (double)(_Frequency / 1000000) / (pSample->numOfCalls - 2),
            (pSample->minimumTime) / (double)(_Frequency / 1000000),
            (pSample->maximumTime) / (double)(_Frequency / 1000000),
            pSample->numOfCalls);

        fwprintf(fp, logBuffer);
        ++i;
    }
    fwprintf(fp, L"\n");

    fclose(fp);
}

Profiler::ProfileSample::ProfileSample(const wchar_t* szTag)
    : tag(szTag)
    , lastStartTime(0)
    , totalTime(0)
    , minimumTime(0)
    , maximumTime(0)
    , numOfCalls(0)
{
}
