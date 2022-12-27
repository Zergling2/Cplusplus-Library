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

    for (int i = 0; i < Profiler::ThreadCount::COUNT; i++)
        InitializeSRWLock(&this->_SRWLock[i]);

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

void Profiler::InitializeThreadForProfiling()
{
    SIZE_T myIndex = static_cast<SIZE_T>(Profiler::GetInstance().GetUniqueIndex());
    BOOL ret = TlsSetValue(Profiler::GetInstance().GetTLSIndex(), reinterpret_cast<LPVOID>(myIndex));
    if (ret == 0)
        throw ProfilerException(TLS_SET_VALUE_FAILED);
}

void Profiler::BeginRecord(const wchar_t* szTag)
{
    SIZE_T idx = reinterpret_cast<SIZE_T>(TlsGetValue(this->_TLSIndex));
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
    pSample->_LastTimeBegan = currentTime.QuadPart;
}

void Profiler::RecordEndRequestHandler(const wchar_t* szTag, const LARGE_INTEGER endTime)
{
    SIZE_T idx = reinterpret_cast<SIZE_T>(TlsGetValue(this->_TLSIndex));
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

    AcquireSRWLockExclusive(&this->_SRWLock[idx]);
    ULONGLONG interval = endTime.QuadPart - pSample->_LastTimeBegan;
    pSample->_TotalTime += interval;

    if (pSample->_NumOfCalls == 0)
    {
        pSample->_MinumumTime = pSample->_MaximumTime = interval;
    }
    else
    {
        if (interval < pSample->_MinumumTime)
        {
            pSample->_MinumumTime = interval;
        }

        if (interval > pSample->_MaximumTime)
        {
            pSample->_MaximumTime = interval;
        }
    }
    pSample->_NumOfCalls += 1;
    pSample->_Flag = TRUE;
    ReleaseSRWLockExclusive(&this->_SRWLock[idx]);
}

LONG Profiler::GetUniqueIndex()
{
    LONG ret = _InterlockedIncrement(&this->_ProfileInfoArrayIndex);
    if (ret >= Profiler::ThreadCount::COUNT)
        throw ProfilerException(PROFILE_ARRAY_OVERFLOW);
    
    return ret;
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
        std::map<CWideString<>, ProfileSample*>::iterator end = _DataMap[i].end();
        for (std::map<CWideString<>, ProfileSample*>::iterator iter = _DataMap[i].begin(); iter != end; ++iter)
        {
            ProfileSample* pSample = iter->second;
            AcquireSRWLockShared(&this->_SRWLock[i]);
            if (pSample->_Flag == FALSE)
            {
                ReleaseSRWLockShared(&this->_SRWLock[i]);
                continue;
            }

            StringCbPrintfW(logBuffer, sizeof(logBuffer), L"%d,%s,%f¥ìs,%f¥ìs,%f¥ìs,%d\n"
                , pSample->_tid
                , pSample->_Tag.c_str()
                , pSample->_NumOfCalls <= 2 ? (pSample->_TotalTime) / (double)(_Frequency / 1000000) / pSample->_NumOfCalls :
                (pSample->_TotalTime - (pSample->_MaximumTime + pSample->_MinumumTime)) / (double)(_Frequency / 1000000) / (pSample->_NumOfCalls - 2)
                , (pSample->_MinumumTime) / (double)(_Frequency / 1000000)
                , (pSample->_MaximumTime) / (double)(_Frequency / 1000000)
                , pSample->_NumOfCalls);

            fwprintf(fp, logBuffer);
            ReleaseSRWLockShared(&this->_SRWLock[i]);
        }
        fwprintf(fp, L"-,-,-,-,-,-\n");
    }
    fwprintf(fp, L"\n");
    fclose(fp);
}

Profiler::ProfileSample::ProfileSample(const wchar_t* szTag)
    : _Flag(FALSE)
    , _tid(GetCurrentThreadId())
    , _Tag(szTag)
    , _LastTimeBegan(0)
    , _TotalTime(0)
    , _MinumumTime(0)
    , _MaximumTime(0)
    , _NumOfCalls(0)
{
}
