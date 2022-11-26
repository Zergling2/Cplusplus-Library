#include "SJParser.h"

const wchar_t* DELIM = L" \t";

bool ReadLine(FILE* hFile, wchar_t* buf, int cchSize)
{
	if (NULL == fgetws(buf, cchSize, hFile))
		if (feof(hFile))
			return false;

	size_t len = wcslen(buf);
	if (len > 0)
		if (buf[len - 1] == '\n')
			buf[len - 1] = '\0';

	return true;
}

bool CSJParser::LoadFile(const wchar_t* szFileName)
{
	ClearAllDataMap();

	wchar_t* szType;
	wchar_t* szKey;
	wchar_t* szEqualMark;
	wchar_t* szValue;

	wchar_t* szContext = nullptr;

	FILE* file = NULL;
	wchar_t buf[512];

	errno_t e = _wfopen_s(&file, szFileName, L"r, ccs=UTF-16LE");
	if (e != 0)
		return false;

	while (ReadLine(file, buf, sizeof(buf) / sizeof(wchar_t)))
	{
		if (wcslen(buf) > 2)
		{
			if (buf[0] == '/' && buf[1] == '/')		// 주석 무시
				continue;

			szType = wcstok_s(buf, DELIM, &szContext);
			szKey = wcstok_s(NULL, DELIM, &szContext);
			szEqualMark = wcstok_s(NULL, DELIM, &szContext);
			szValue = wcstok_s(NULL, DELIM, &szContext);

			if (wcscmp(szType, L"[STRING]") == 0)
				_StringDataMap.emplace(szKey, szValue);
			else if (wcscmp(szType, L"[CHAR]") == 0 || wcscmp(szType, L"[UCHAR]") == 0)
				_CharacterDataMap.emplace(szKey, szValue[0]);
			else if (wcscmp(szType, L"[SHORT]") == 0 || wcscmp(szType, L"[USHORT]") == 0)
				_Integer16DataMap.emplace(szKey, _wtoi(szValue));
			else if (wcscmp(szType, L"[INT]") == 0 || wcscmp(szType, L"[UINT]") == 0)
				_Integer32DataMap.emplace(szKey, _wtoi(szValue));
			else if (wcscmp(szType, L"[LONGLONG]") == 0 || wcscmp(szType, L"[ULONGLONG]") == 0)
				_Integer64DataMap.emplace(szKey, _wtoll(szValue));
			else if (wcscmp(szType, L"[FLOAT]") == 0)
				_FloatDataMap.emplace(szKey, wcstof(szValue, NULL));
			else if (wcscmp(szType, L"[DOUBLE]") == 0)
				_DoubleDataMap.emplace(szKey, wcstod(szValue, NULL));
			else
			{
				wprintf(L"Failed to read symbol!\n");
				*(int*)0 = 0;
			}
		}
	}

	if (file != NULL)
		fclose(file);

	return true;
}

bool CSJParser::GetValue(const wchar_t* szKey, wchar_t* _Buf, size_t cchSize)
{
	auto find = _StringDataMap.find(szKey);
	if (find == _StringDataMap.end())
		return false;

	wcscpy_s(_Buf, cchSize, find->second.c_str());
	return true;
}

bool CSJParser::GetValue(const wchar_t* szKey, std::wstring& ref)
{
	auto find = _StringDataMap.find(szKey);
	if (find == _StringDataMap.end())
		return false;

	ref = find->second;
	return true;
}

bool CSJParser::GetValue(const wchar_t* szKey, char& ref)
{
	auto find = _CharacterDataMap.find(szKey);
	if (find == _CharacterDataMap.end())
		return false;

	ref = find->second;
	return true;
}

bool CSJParser::GetValue(const wchar_t* szKey, unsigned char& ref)
{
	auto find = _CharacterDataMap.find(szKey);
	if (find == _CharacterDataMap.end())
		return false;

	ref = find->second;
	return true;
}

bool CSJParser::GetValue(const wchar_t* szKey, short& ref)
{
	auto find = _Integer16DataMap.find(szKey);
	if (find == _Integer16DataMap.end())
		return false;

	ref = find->second;
	return true;
}

bool CSJParser::GetValue(const wchar_t* szKey, unsigned short& ref)
{
	auto find = _Integer16DataMap.find(szKey);
	if (find == _Integer16DataMap.end())
		return false;

	ref = find->second;
	return true;
}

bool CSJParser::GetValue(const wchar_t* szKey, int& ref)
{
	auto find = _Integer32DataMap.find(szKey);
	if (find == _Integer32DataMap.end())
		return false;

	ref = find->second;
	return true;
}

bool CSJParser::GetValue(const wchar_t* szKey, unsigned int& ref)
{
	auto find = _Integer32DataMap.find(szKey);
	if (find == _Integer32DataMap.end())
		return false;

	ref = find->second;
	return true;
}

bool CSJParser::GetValue(const wchar_t* szKey, long long& ref)
{
	auto find = _Integer64DataMap.find(szKey);
	if (find == _Integer64DataMap.end())
		return false;

	ref = find->second;
	return true;
}

bool CSJParser::GetValue(const wchar_t* szKey, unsigned long long& ref)
{
	auto find = _Integer64DataMap.find(szKey);
	if (find == _Integer64DataMap.end())
		return false;

	ref = find->second;
	return true;
}

bool CSJParser::GetValue(const wchar_t* szKey, float& ref)
{
	auto find = _FloatDataMap.find(szKey);
	if (find == _FloatDataMap.end())
		return false;

	ref = find->second;
	return true;
}

bool CSJParser::GetValue(const wchar_t* szKey, double& ref)
{
	auto find = _DoubleDataMap.find(szKey);
	if (find == _DoubleDataMap.end())
		return false;

	ref = find->second;
	return true;
}

void CSJParser::ClearAllDataMap()
{
	_StringDataMap.clear();
	_CharacterDataMap.clear();
	_Integer16DataMap.clear();
	_Integer32DataMap.clear();
	_Integer64DataMap.clear();
	_FloatDataMap.clear();
	_DoubleDataMap.clear();
}
