#pragma once

#include <string>
#include <map>
#define _WINSOCKAPI_
#include <Windows.h>
#undef _WINSOCKAPI_

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
    std::map<CWideString<>, ProfileSample*> _DataMap;
    LONGLONG _Frequency;
    static const wchar_t* szItems;
private:
    struct ProfileSample
    {
        friend Profiler;
    public:
        ProfileSample(const wchar_t* szTag);
    private:
        CWideString<> tag; // 프로파일 샘플 이름
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

