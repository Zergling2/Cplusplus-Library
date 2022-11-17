/*
* (WARNING!) It is not safe for buffer overflow. Please handle it by the user.
*/

#pragma once

#include "CSerializedBuffer.hpp"
#include <string>

namespace SJNET
{
	namespace LIB
	{
		class CSerializedBufferEx : public CSerializedBuffer
		{
		public:
			// using SJNET::LIB::CSerializedBuffer::In;
			// using SJNET::LIB::CSerializedBuffer::Out;
			// using SJNET::LIB::CSerializedBuffer::operator<<;
			// using SJNET::LIB::CSerializedBuffer::operator>>;
			inline void szIn(_In_ const char* szData);
			inline void szIn(_In_ const std::string& data);
			inline void szIn(_In_ const std::string&& data);
			inline void szIn(_In_ const wchar_t* szData);
			inline void szIn(_In_ const std::wstring& data);
			inline void szIn(_In_ const std::wstring&& data);

			inline void szOut(_Inout_ char*& dest);
			inline void szOut(_Inout_ std::string& dest);
			inline void szOut(_Inout_ wchar_t*& dest);
			inline void szOut(_Inout_ std::wstring& dest);
		};

		inline void CSerializedBufferEx::szIn(_In_ const char* szData)
		{
			size_t copySize = strlen(szData) + 1;
			memmove(pointerSet.writePointer, szData, copySize);
			pointerSet.writePointer = reinterpret_cast<char*>(pointerSet.writePointer) + copySize;
		}

		inline void CSerializedBufferEx::szIn(_In_ const std::string& data)
		{
			szIn(data.c_str());
		}

		inline void CSerializedBufferEx::szIn(_In_ const std::string&& data)
		{
			szIn(data.c_str());
		}

		inline void CSerializedBufferEx::szIn(_In_ const wchar_t* szData)
		{
			size_t copySize = (wcslen(szData) << 1) + 2;
			memmove(pointerSet.writePointer, szData, copySize);
			pointerSet.writePointer = reinterpret_cast<char*>(pointerSet.writePointer) + copySize;
		}

		inline void CSerializedBufferEx::szIn(_In_ const std::wstring& data)
		{
			szIn(data.c_str());
		}

		inline void CSerializedBufferEx::szIn(_In_ const std::wstring&& data)
		{
			szIn(data.c_str());
		}

		inline void CSerializedBufferEx::szOut(_Inout_ char*& dest)
		{
			dest = reinterpret_cast<char*>(pointerSet.readPointer);
			pointerSet.readPointer = reinterpret_cast<char*>(pointerSet.readPointer) + strlen(reinterpret_cast<char*>(pointerSet.readPointer)) + 1;
		}

		inline void CSerializedBufferEx::szOut(_Inout_ std::string& dest)
		{
			dest = reinterpret_cast<char*>(pointerSet.readPointer);
			pointerSet.readPointer = reinterpret_cast<char*>(pointerSet.readPointer) + strlen(reinterpret_cast<char*>(pointerSet.readPointer)) + 1;
		}

		inline void CSerializedBufferEx::szOut(_Inout_ wchar_t*& dest)
		{
			dest = reinterpret_cast<wchar_t*>(pointerSet.readPointer);
			pointerSet.readPointer = reinterpret_cast<wchar_t*>(pointerSet.readPointer) + wcslen(reinterpret_cast<wchar_t*>(pointerSet.readPointer)) + 1;
		}

		inline void CSerializedBufferEx::szOut(_Inout_ std::wstring& dest)
		{
			dest = reinterpret_cast<wchar_t*>(pointerSet.readPointer);
			pointerSet.readPointer = reinterpret_cast<wchar_t*>(pointerSet.readPointer) + wcslen(reinterpret_cast<wchar_t*>(pointerSet.readPointer)) + 1;
		}
	}
}

typedef SJNET::LIB::CSerializedBufferEx SJNetPacket;