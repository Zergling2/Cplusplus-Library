#pragma once

#include <unordered_map>
#include <string>

class CSJParser
{
public:
	bool LoadFile(const wchar_t* szFileName);
	bool GetValue(const wchar_t* szKey, wchar_t* _Buf, size_t cchSize);
	bool GetValue(const wchar_t* szKey, std::wstring& ref);
	bool GetValue(const wchar_t* szKey, char& ref);
	bool GetValue(const wchar_t* szKey, unsigned char& ref);
	bool GetValue(const wchar_t* szKey, short& ref);
	bool GetValue(const wchar_t* szKey, unsigned short& ref);
	bool GetValue(const wchar_t* szKey, int& ref);
	bool GetValue(const wchar_t* szKey, unsigned int& ref);
	bool GetValue(const wchar_t* szKey, long long& ref);
	bool GetValue(const wchar_t* szKey, unsigned long long& ref);
	bool GetValue(const wchar_t* szKey, float& ref);
	bool GetValue(const wchar_t* szKey, double& ref);
private:
	void ClearAllDataMap();
	std::unordered_map<std::wstring, std::wstring> _StringDataMap;
	std::unordered_map<std::wstring, char> _CharacterDataMap;
	std::unordered_map<std::wstring, short> _Integer16DataMap;
	std::unordered_map<std::wstring, int> _Integer32DataMap;
	std::unordered_map<std::wstring, long long> _Integer64DataMap;
	std::unordered_map<std::wstring, float> _FloatDataMap;
	std::unordered_map<std::wstring, double> _DoubleDataMap;
};
