#pragma warning (disable:6387)
#include "Profiler.h"

#include <strsafe.h>
#include <time.h>
#include <locale.h>
#include <stdio.h>

Profiler Profiler::_Instance;

Profiler::Profiler()
    : _ProfileInfoArrayIndex(-1)
{
    _wsetlocale(LC_ALL, L"ko-kr");
    LARGE_INTEGER f;
    QueryPerformanceFrequency(&f);
    this->_Frequency = f.QuadPart;

    this->_TLSIndex = TlsAlloc();
    if (this->_TLSIndex == TLS_OUT_OF_INDEXES)
        throw ProfilerException(TLS_OUT_OF_INDEXES);
}

Profiler::~Profiler()
{
    for (LONG i = 0; i <= this->_ProfileInfoArrayIndex; i++)
    {
        std::map<CWideString<>, ProfileSample*>::iterator iter = _DataMap[i].begin();
        std::map<CWideString<>, ProfileSample*>::iterator end = _DataMap[i].end();

        while (iter != end)
        {
            ProfileSample* pDel = iter->second;
            iter = _DataMap[i].erase(iter);
            delete pDel;
        }
    }

    TlsFree(this->_TLSIndex);
}

void Profiler::InitializeThreadForProfiling() throw()
{
    LONG myIndex = Profiler::GetInstance().GetUniqueIndex();
    BOOL ret = TlsSetValue(Profiler::GetInstance().GetTLSIndex(), reinterpret_cast<LPVOID>(myIndex));
    if (ret == 0)
        throw ProfilerException(TLS_SET_VALUE_FAILED);
}

void Profiler::BeginRecord(const wchar_t* szTag)
{
    DWORD idx = reinterpret_cast<DWORD>(TlsGetValue(this->_TLSIndex));
    auto find = _DataMap[idx].find(szTag);
    ProfileSample* pSample;

    if (find == _DataMap[idx].end())
    {
        pSample = new ProfileSample(szTag);
        _DataMap[idx].emplace(szTag, pSample);
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
    DWORD idx = reinterpret_cast<DWORD>(TlsGetValue(this->_TLSIndex));
    auto find = _DataMap[idx].find(szTag);
    ProfileSample* pSample;

    if (find == _DataMap[idx].end())
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

LONG Profiler::GetUniqueIndex()
{
    LONG ret = _InterlockedIncrement(&this->_ProfileInfoArrayIndex);
    if (ret >= Profiler::ThreadCount::COUNT)
        throw ProfilerException(PROFILE_ARRAY_OVERFLOW);
}

void Profiler::SaveProfile(const wchar_t* szFileName)
{
    FILE* fp;
    wchar_t logBuffer[512];
    std::wstring strRealFileName(szFileName);
    strRealFileName.append(L".csv");

    errno_t e = _wfopen_s(&fp, strRealFileName.c_str(), L"at");
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
    StringCbPrintfW(logBuffer, sizeof(logBuffer), L"%04d-%02d-%02d %02d:%02d:%02d,x64 Debug mode,,,,\n",
        base_date_local.tm_year + 1900, base_date_local.tm_mon + 1, base_date_local.tm_mday,
        base_date_local.tm_hour, base_date_local.tm_min, base_date_local.tm_sec);
    #else
    StringCbPrintfW(logBuffer, sizeof(logBuffer), L"%04d-%02d-%02d %02d:%02d:%02d,x86 Debug mode,,,,\n",
        base_date_local.tm_year + 1900, base_date_local.tm_mon + 1, base_date_local.tm_mday,
        base_date_local.tm_hour, base_date_local.tm_min, base_date_local.tm_sec);
    #endif
#else
    #ifdef _WIN64
    StringCbPrintfW(logBuffer, sizeof(logBuffer), L"%04d-%02d-%02d %02d:%02d:%02d,x64 Release mode,,,,\n",
        base_date_local.tm_year + 1900, base_date_local.tm_mon + 1, base_date_local.tm_mday,
        base_date_local.tm_hour, base_date_local.tm_min, base_date_local.tm_sec);
    #else
    StringCbPrintfW(logBuffer, sizeof(logBuffer), L"%04d-%02d-%02d %02d:%02d:%02d,x86 Release mode,,,,\n",
        base_date_local.tm_year + 1900, base_date_local.tm_mon + 1, base_date_local.tm_mday,
        base_date_local.tm_hour, base_date_local.tm_min, base_date_local.tm_sec);
    #endif
#endif

    fwprintf(fp, logBuffer);
    fwprintf(fp, L"Thread ID,Name,Average,Min,Max,Call\n");

    for (LONG i = 0; i <= this->_ProfileInfoArrayIndex; i++)
    {
        std::map<CWideString<>, ProfileSample*>::iterator iter = _DataMap[i].begin();
        std::map<CWideString<>, ProfileSample*>::iterator end = _DataMap[i].end();
        while (iter != end)
        {
            ProfileSample* pSample = iter->second;

            StringCbPrintfW(logBuffer, sizeof(logBuffer), L"%d,%s,%f¥ìs,%f¥ìs,%f¥ìs,%d\n"
                , pSample->tid
                , pSample->tag.c_str()
                , pSample->numOfCalls <= 2 ? (pSample->totalTime) / (double)(_Frequency / 1000000) / pSample->numOfCalls :
                (pSample->totalTime - (pSample->maximumTime + pSample->minimumTime)) / (double)(_Frequency / 1000000) / (pSample->numOfCalls - 2)
                , (pSample->minimumTime) / (double)(_Frequency / 1000000)
                , (pSample->maximumTime) / (double)(_Frequency / 1000000)
                , pSample->numOfCalls);

            fwprintf(fp, logBuffer);
            ++iter;
        }
        fwprintf(fp, L"-,-,-,-,-,-\n");
    }
    fwprintf(fp, L"\n");

    fclose(fp);
}

Profiler::ProfileSample::ProfileSample(const wchar_t* szTag)
    : tid(GetCurrentThreadId())
    , tag(szTag)
    , lastStartTime(0)
    , totalTime(0)
    , minimumTime(0)
    , maximumTime(0)
    , numOfCalls(0)
{
}
