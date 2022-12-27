#pragma once

#include <string>
#include <map>
#define _WINSOCKAPI_
#include <Windows.h>
#undef _WINSOCKAPI_

#define TLS_SET_VALUE_FAILED ((DWORD)0x00000000)
#define PROFILE_ARRAY_OVERFLOW ((DWORD)0x00000001)

#define PROFILING_THREAD() Profiler::InitializeThreadForProfiling()

enum CWideStringLength
{
    DEFAULT_LENGTH = 47 + 1 // Include '\0'
};

template<unsigned N = CWideStringLength::DEFAULT_LENGTH>
struct CWideString
{
public:
    inline CWideString() { this->_Data[0] = L'\0'; }
    inline CWideString(const CWideString<N>& cb) { wcscpy_s(this->_Data, sizeof(_Data) / sizeof(wchar_t), cb._Data); }
    inline CWideString(const wchar_t* p) { wcscpy_s(this->_Data, sizeof(_Data) / sizeof(wchar_t), p); }
    inline CWideString<N>& operator=(const CWideString<N>& rhs) { wcscpy_s(this->_Data, sizeof(_Data) / sizeof(wchar_t), rhs._Data); }
    inline CWideString<N>& operator=(const wchar_t* rhs) { wcscpy_s(this->_Data, sizeof(_Data) / sizeof(wchar_t), rhs); }
    inline bool operator==(const CWideString<N>& that) const { return wcscmp(this->_Data, that._Data) ? false : true; }
    inline bool operator<(const CWideString<N>& that) const { return wcscmp(this->_Data, that._Data) < 0 ? true : false; }
    inline const wchar_t* c_str() { return this->_Data; }
private:
    wchar_t _Data[N];
};

class ProfilerException
{
public:
    inline ProfilerException(int errorCode) : errorCode(errorCode) {}
    void Report() const
    {
        switch (this->errorCode)
        {
        case TLS_OUT_OF_INDEXES:
            wprintf(L"TlsAlloc function failed!\n");
            break;
        case TLS_SET_VALUE_FAILED:
            wprintf(L"TlsSetValue function failed!\n");
            break;
        case PROFILE_ARRAY_OVERFLOW:
            wprintf(L"The array size of the data map is insufficient!\n");
            break;
        default:
            wprintf(L"Profiler error occurred!\n");
            break;
        }
    }
private:
    int errorCode;
};

class Profiler
{
    enum ThreadCount
    {
        COUNT = 40
    };
private:
    struct ProfileSample
    {
        friend Profiler;
    public:
        ProfileSample(const wchar_t* szTag);
    private:
        BOOL _Flag;
        int _tid;
        CWideString<> _Tag;         // Name of profile sample
        ULONGLONG _LastTimeBegan;
        ULONGLONG _TotalTime;
        ULONGLONG _MinumumTime;
        ULONGLONG _MaximumTime;
        int _NumOfCalls;
    };
private:
    Profiler();
    ~Profiler();
public:
    inline static Profiler& GetInstance() { return Profiler::_Instance; }
    static void InitializeThreadForProfiling();
    void BeginRecord(const wchar_t* szTag);
    inline void EndRecord(const wchar_t* szTag);
    void SaveProfile(const wchar_t* szFileName);
private:
    void RecordEndRequestHandler(const wchar_t* szTag, const LARGE_INTEGER endTime);
    LONG GetUniqueIndex();
    inline DWORD GetTLSIndex() { return this->_TLSIndex; }
    std::map<CWideString<>, ProfileSample*> _DataMap[ThreadCount::COUNT];
    SRWLOCK _SRWLock[ThreadCount::COUNT];
    LONGLONG _Frequency;
    DWORD _TLSIndex;
    LONG _ProfileInfoArrayIndex;
    static Profiler _Instance;
};

inline void Profiler::EndRecord(const wchar_t* szTag)
{
    LARGE_INTEGER endTime;
    QueryPerformanceCounter(&endTime);

    RecordEndRequestHandler(szTag, endTime);
}
