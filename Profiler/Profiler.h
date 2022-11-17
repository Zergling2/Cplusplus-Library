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
        std::wstring tag; // �������� ���� �̸�
        ULONGLONG lastStartTime; // �������� ���� ���� �ð�.
        ULONGLONG totalTime; // ��ü ���ð� ī���� Time. (��½� ȣ��ȸ���� ������ ��� ����)
        ULONGLONG minimumTime; // �ּ� ���ð� ī���� Time. (�ʴ����� ����Ͽ� ���� / [0] �����ּ� [1] ���� �ּ� [2])
        ULONGLONG maximumTime; // �ִ� ���ð� ī���� Time. (�ʴ����� ����Ͽ� ���� / [0] �����ִ� [1] ���� �ִ� [2])
        int numOfCalls; // ���� ȣ�� Ƚ��
    };
};

inline void Profiler::EndRecord(const wchar_t* szTag)
{
    LARGE_INTEGER endTime;
    QueryPerformanceCounter(&endTime);

    RecordEndRequestHandler(szTag, endTime);
}

#endif // !__PROFILER_H__