/*
* (WARNING!) It is not safe for buffer overflow. Please handle it by the user.
*/

#pragma once

#pragma warning (disable:26495)

#define _WINSOCKAPI_
#include <Windows.h>
#undef _WINSOCKAPI_

namespace SJNET
{
	namespace LIB
	{
		enum Size
		{
			SIZE = 4096
		};

		class CSerializedBuffer
		{
		public:
			CSerializedBuffer();		// Constructor

			inline void* GetBufferBeginAddress() const			{ return pointerSet.bufferBeginAddress; }
			inline void* GetBufferEndAddress() const			{ return pointerSet.bufferEndAddress; }
			inline void* GetWritePointer() const				{ return pointerSet.writePointer; }
			inline void SetWritePointer(void* p)				{ this->pointerSet.writePointer = p; }
			inline const void* GetReadPointer() const			{ return pointerSet.readPointer; }
			inline void SetReadPointer(void* p)				{ this->pointerSet.readPointer = p; }
			inline void MoveWritePointer(int offset)			{ pointerSet.writePointer = reinterpret_cast<char*>(pointerSet.writePointer) + offset; }
			inline void MoveReadPointer(int offset)			{ pointerSet.readPointer = reinterpret_cast<char*>(pointerSet.readPointer) + offset; }

			inline void In(_In_ char data);
			inline void In(_In_ unsigned char data);
			inline void In(_In_ short data);
			inline void In(_In_ unsigned short data);
			inline void In(_In_ int data);
			inline void In(_In_ unsigned int data);
			inline void In(_In_ long data);
			inline void In(_In_ unsigned long data);
			inline void In(_In_ __int64 data);
			inline void In(_In_ unsigned __int64 data);
			inline void In(_In_ float data);
			inline void In(_In_ double data);
			inline void InBytes(_In_ const void* pBuf, size_t cbSize);

			inline CSerializedBuffer& operator<<(_In_ char data);
			inline CSerializedBuffer& operator<<(_In_ unsigned char data);
			inline CSerializedBuffer& operator<<(_In_ short data);
			inline CSerializedBuffer& operator<<(_In_ unsigned short data);
			inline CSerializedBuffer& operator<<(_In_ int data);
			inline CSerializedBuffer& operator<<(_In_ unsigned int data);
			inline CSerializedBuffer& operator<<(_In_ long data);
			inline CSerializedBuffer& operator<<(_In_ unsigned long data);
			inline CSerializedBuffer& operator<<(_In_ __int64 data);
			inline CSerializedBuffer& operator<<(_In_ unsigned __int64 data);
			inline CSerializedBuffer& operator<<(_In_ float data);
			inline CSerializedBuffer& operator<<(_In_ double data);

			inline void Out(_Inout_ char& dest) const;
			inline void Out(_Inout_ unsigned char& dest) const;
			inline void Out(_Inout_ short& dest) const;
			inline void Out(_Inout_ unsigned short& dest) const;
			inline void Out(_Inout_ int& dest) const;
			inline void Out(_Inout_ unsigned int& dest) const;
			inline void Out(_Inout_ long& dest) const;
			inline void Out(_Inout_ unsigned long& dest) const;
			inline void Out(_Inout_ __int64& dest) const;
			inline void Out(_Inout_ unsigned __int64& dest) const;
			inline void Out(_Inout_ float& dest) const;
			inline void Out(_Inout_ double& dest) const;
			inline void OutBytes(_Inout_ void* pBuf, size_t cbSize) const;

			inline const CSerializedBuffer& operator>>(_Inout_ char& dest) const;
			inline const CSerializedBuffer& operator>>(_Inout_ unsigned char& dest) const;
			inline const CSerializedBuffer& operator>>(_Inout_ short& dest) const;
			inline const CSerializedBuffer& operator>>(_Inout_ unsigned short& dest) const;
			inline const CSerializedBuffer& operator>>(_Inout_ int& dest) const;
			inline const CSerializedBuffer& operator>>(_Inout_ unsigned int& dest) const;
			inline const CSerializedBuffer& operator>>(_Inout_ long& dest) const;
			inline const CSerializedBuffer& operator>>(_Inout_ unsigned long& dest) const;
			inline const CSerializedBuffer& operator>>(_Inout_ __int64& dest) const;
			inline const CSerializedBuffer& operator>>(_Inout_ unsigned __int64& dest) const;
			inline const CSerializedBuffer& operator>>(_Inout_ float& dest) const;
			inline const CSerializedBuffer& operator>>(_Inout_ double& dest) const;

			inline size_t GetBufferSize() const { return pointerSet.bufferSize; }
			inline size_t GetFreeSize()	const { return reinterpret_cast<size_t>(pointerSet.bufferEndAddress) - reinterpret_cast<size_t>(pointerSet.writePointer); }
			inline size_t GetUsingSize() const { return reinterpret_cast<size_t>(pointerSet.writePointer) - reinterpret_cast<size_t>(pointerSet.bufferBeginAddress); }
			inline void Reuse() { pointerSet.readPointer = pointerSet.writePointer = pointerSet.bufferBeginAddress; }
		protected:
			struct PointerSet
			{
				size_t bufferSize;
				char* bufferBeginAddress;
				void* bufferEndAddress;
				mutable void* readPointer;
				mutable void* writePointer;
			};
		protected:
			inline void ForceCrash(size_t _violationAddr) const { *reinterpret_cast<int*>(_violationAddr) = 0; }
			PointerSet pointerSet;
			char buffer[SIZE];
		};

		inline CSerializedBuffer::CSerializedBuffer()
		{
			pointerSet.bufferSize = SIZE;
			pointerSet.bufferBeginAddress = buffer;
			pointerSet.bufferEndAddress = buffer + SIZE;
			pointerSet.readPointer = buffer;
			pointerSet.writePointer = buffer;
		}

		inline void CSerializedBuffer::In(_In_ char data)
		{
			*reinterpret_cast<char*>(pointerSet.writePointer) = data;
			pointerSet.writePointer = reinterpret_cast<char*>(pointerSet.writePointer) + 1;
		}

		inline void CSerializedBuffer::In(_In_ unsigned char data)
		{
			*reinterpret_cast<unsigned char*>(pointerSet.writePointer) = data;
			pointerSet.writePointer = reinterpret_cast<unsigned char*>(pointerSet.writePointer) + 1;
		}

		inline void CSerializedBuffer::In(_In_ short data)
		{
			*reinterpret_cast<short*>(pointerSet.writePointer) = data;
			pointerSet.writePointer = reinterpret_cast<short*>(pointerSet.writePointer) + 1;
		}

		inline void CSerializedBuffer::In(_In_ unsigned short data)
		{
			*reinterpret_cast<unsigned short*>(pointerSet.writePointer) = data;
			pointerSet.writePointer = reinterpret_cast<unsigned short*>(pointerSet.writePointer) + 1;
		}

		inline void CSerializedBuffer::In(_In_ int data)
		{
			*reinterpret_cast<int*>(pointerSet.writePointer) = data;
			pointerSet.writePointer = reinterpret_cast<int*>(pointerSet.writePointer) + 1;
		}

		inline void CSerializedBuffer::In(_In_ unsigned int data)
		{
			*reinterpret_cast<unsigned int*>(pointerSet.writePointer) = data;
			pointerSet.writePointer = reinterpret_cast<unsigned int*>(pointerSet.writePointer) + 1;
		}

		inline void CSerializedBuffer::In(_In_ long data)
		{
			*reinterpret_cast<long*>(pointerSet.writePointer) = data;
			pointerSet.writePointer = reinterpret_cast<int*>(pointerSet.writePointer) + 1;
		}

		inline void CSerializedBuffer::In(_In_ unsigned long data)
		{
			*reinterpret_cast<unsigned long*>(pointerSet.writePointer) = data;
			pointerSet.writePointer = reinterpret_cast<unsigned int*>(pointerSet.writePointer) + 1;
		}

		inline void CSerializedBuffer::In(_In_ __int64 data)
		{
			*reinterpret_cast<__int64*>(pointerSet.writePointer) = data;
			pointerSet.writePointer = reinterpret_cast<__int64*>(pointerSet.writePointer) + 1;
		}

		inline void CSerializedBuffer::In(_In_ unsigned __int64 data)
		{
			*reinterpret_cast<unsigned __int64*>(pointerSet.writePointer) = data;
			pointerSet.writePointer = reinterpret_cast<unsigned __int64*>(pointerSet.writePointer) + 1;
		}

		inline void CSerializedBuffer::In(_In_ float data)
		{
			*reinterpret_cast<float*>(pointerSet.writePointer) = data;
			pointerSet.writePointer = reinterpret_cast<float*>(pointerSet.writePointer) + 1;
		}

		inline void CSerializedBuffer::In(_In_ double data)
		{
			*reinterpret_cast<double*>(pointerSet.writePointer) = data;
			pointerSet.writePointer = reinterpret_cast<double*>(pointerSet.writePointer) + 1;
		}

		inline void CSerializedBuffer::InBytes(_In_ const void* pBuf, size_t cbSize)
		{
			memmove(pointerSet.writePointer, pBuf, cbSize);
			pointerSet.writePointer = reinterpret_cast<char*>(pointerSet.writePointer) + cbSize;
		}

		/* ---------------------------------------------------------------------------------------- */

		inline CSerializedBuffer& CSerializedBuffer::operator<<(_In_ char data)
		{
			*reinterpret_cast<char*>(pointerSet.writePointer) = data;
			pointerSet.writePointer = reinterpret_cast<char*>(pointerSet.writePointer) + 1;

			return *this;
		}

		inline CSerializedBuffer& CSerializedBuffer::operator<<(_In_ unsigned char data)
		{
			*reinterpret_cast<unsigned char*>(pointerSet.writePointer) = data;
			pointerSet.writePointer = reinterpret_cast<unsigned char*>(pointerSet.writePointer) + 1;

			return *this;
		}

		inline CSerializedBuffer& CSerializedBuffer::operator<<(_In_ short data)
		{
			*reinterpret_cast<short*>(pointerSet.writePointer) = data;
			pointerSet.writePointer = reinterpret_cast<short*>(pointerSet.writePointer) + 1;

			return *this;
		}

		inline CSerializedBuffer& CSerializedBuffer::operator<<(_In_ unsigned short data)
		{
			*reinterpret_cast<unsigned short*>(pointerSet.writePointer) = data;
			pointerSet.writePointer = reinterpret_cast<unsigned short*>(pointerSet.writePointer) + 1;

			return *this;
		}

		inline CSerializedBuffer& CSerializedBuffer::operator<<(_In_ int data)
		{
			*reinterpret_cast<int*>(pointerSet.writePointer) = data;
			pointerSet.writePointer = reinterpret_cast<int*>(pointerSet.writePointer) + 1;

			return *this;
		}

		inline CSerializedBuffer& CSerializedBuffer::operator<<(_In_ unsigned int data)
		{
			*reinterpret_cast<unsigned int*>(pointerSet.writePointer) = data;
			pointerSet.writePointer = reinterpret_cast<unsigned int*>(pointerSet.writePointer) + 1;

			return *this;
		}

		inline CSerializedBuffer& CSerializedBuffer::operator<<(_In_ long data)
		{
			*reinterpret_cast<long*>(pointerSet.writePointer) = data;
			pointerSet.writePointer = reinterpret_cast<int*>(pointerSet.writePointer) + 1;

			return *this;
		}

		inline CSerializedBuffer& CSerializedBuffer::operator<<(_In_ unsigned long data)
		{
			*reinterpret_cast<unsigned long*>(pointerSet.writePointer) = data;
			pointerSet.writePointer = reinterpret_cast<unsigned int*>(pointerSet.writePointer) + 1;

			return *this;
		}

		inline CSerializedBuffer& CSerializedBuffer::operator<<(_In_ __int64 data)
		{
			*reinterpret_cast<__int64*>(pointerSet.writePointer) = data;
			pointerSet.writePointer = reinterpret_cast<__int64*>(pointerSet.writePointer) + 1;

			return *this;
		}

		inline CSerializedBuffer& CSerializedBuffer::operator<<(_In_ unsigned __int64 data)
		{
			*reinterpret_cast<unsigned __int64*>(pointerSet.writePointer) = data;
			pointerSet.writePointer = reinterpret_cast<unsigned __int64*>(pointerSet.writePointer) + 1;

			return *this;
		}

		inline CSerializedBuffer& CSerializedBuffer::operator<<(_In_ float data)
		{
			*reinterpret_cast<float*>(pointerSet.writePointer) = data;
			pointerSet.writePointer = reinterpret_cast<float*>(pointerSet.writePointer) + 1;

			return *this;
		}

		inline CSerializedBuffer& CSerializedBuffer::operator<<(_In_ double data)
		{
			*reinterpret_cast<double*>(pointerSet.writePointer) = data;
			pointerSet.writePointer = reinterpret_cast<double*>(pointerSet.writePointer) + 1;

			return *this;
		}


		/* ---------------------------------------------------------------------------------------- */

		inline void CSerializedBuffer::Out(_Inout_ char& dest) const
		{
			dest = *reinterpret_cast<char*>(pointerSet.readPointer);
			pointerSet.readPointer = reinterpret_cast<char*>(pointerSet.readPointer) + 1;
		}

		inline void CSerializedBuffer::Out(_Inout_ unsigned char& dest) const
		{
			dest = *reinterpret_cast<unsigned char*>(pointerSet.readPointer);
			pointerSet.readPointer = reinterpret_cast<unsigned char*>(pointerSet.readPointer) + 1;
		}

		inline void CSerializedBuffer::Out(_Inout_ short& dest) const
		{
			dest = *reinterpret_cast<short*>(pointerSet.readPointer);
			pointerSet.readPointer = reinterpret_cast<short*>(pointerSet.readPointer) + 1;
		}

		inline void CSerializedBuffer::Out(_Inout_ unsigned short& dest) const
		{
			dest = *reinterpret_cast<unsigned short*>(pointerSet.readPointer);
			pointerSet.readPointer = reinterpret_cast<unsigned short*>(pointerSet.readPointer) + 1;
		}

		inline void CSerializedBuffer::Out(_Inout_ int& dest) const
		{
			dest = *reinterpret_cast<int*>(pointerSet.readPointer);
			pointerSet.readPointer = reinterpret_cast<int*>(pointerSet.readPointer) + 1;
		}

		inline void CSerializedBuffer::Out(_Inout_ unsigned int& dest) const
		{
			dest = *reinterpret_cast<unsigned int*>(pointerSet.readPointer);
			pointerSet.readPointer = reinterpret_cast<unsigned int*>(pointerSet.readPointer) + 1;
		}

		inline void CSerializedBuffer::Out(_Inout_ long& dest) const
		{
			dest = *reinterpret_cast<long*>(pointerSet.readPointer);
			pointerSet.readPointer = reinterpret_cast<int*>(pointerSet.readPointer) + 1;
		}

		inline void CSerializedBuffer::Out(_Inout_ unsigned long& dest) const
		{
			dest = *reinterpret_cast<unsigned long*>(pointerSet.readPointer);
			pointerSet.readPointer = reinterpret_cast<unsigned int*>(pointerSet.readPointer) + 1;
		}

		inline void CSerializedBuffer::Out(_Inout_ __int64& dest) const
		{
			dest = *reinterpret_cast<__int64*>(pointerSet.readPointer);
			pointerSet.readPointer = reinterpret_cast<__int64*>(pointerSet.readPointer) + 1;
		}

		inline void CSerializedBuffer::Out(_Inout_ unsigned __int64& dest) const
		{
			dest = *reinterpret_cast<unsigned __int64*>(pointerSet.readPointer);
			pointerSet.readPointer = reinterpret_cast<unsigned __int64*>(pointerSet.readPointer) + 1;
		}

		inline void CSerializedBuffer::Out(_Inout_ float& dest) const
		{
			dest = *reinterpret_cast<float*>(pointerSet.readPointer);
			pointerSet.readPointer = reinterpret_cast<float*>(pointerSet.readPointer) + 1;
		}

		inline void CSerializedBuffer::Out(_Inout_ double& dest) const
		{
			dest = *reinterpret_cast<double*>(pointerSet.readPointer);
			pointerSet.readPointer = reinterpret_cast<double*>(pointerSet.readPointer) + 1;
		}

		inline void CSerializedBuffer::OutBytes(_Inout_ void* pBuf, size_t cbSize) const
		{
			memmove(pBuf, pointerSet.readPointer, cbSize);
			pointerSet.readPointer = reinterpret_cast<char*>(pointerSet.readPointer) + cbSize;
		}

		/* ---------------------------------------------------------------------------------------- */

		inline const CSerializedBuffer& CSerializedBuffer::operator>>(_Inout_ char& dest) const
		{
			dest = *reinterpret_cast<char*>(pointerSet.readPointer);
			pointerSet.readPointer = reinterpret_cast<char*>(pointerSet.readPointer) + 1;

			return *this;
		}

		inline const CSerializedBuffer& CSerializedBuffer::operator>>(_Inout_ unsigned char& dest) const
		{
			dest = *reinterpret_cast<unsigned char*>(pointerSet.readPointer);
			pointerSet.readPointer = reinterpret_cast<unsigned char*>(pointerSet.readPointer) + 1;

			return *this;
		}

		inline const CSerializedBuffer& CSerializedBuffer::operator>>(_Inout_ short& dest) const
		{
			dest = *reinterpret_cast<short*>(pointerSet.readPointer);
			pointerSet.readPointer = reinterpret_cast<short*>(pointerSet.readPointer) + 1;

			return *this;
		}

		inline const CSerializedBuffer& CSerializedBuffer::operator>>(_Inout_ unsigned short& dest) const
		{
			dest = *reinterpret_cast<unsigned short*>(pointerSet.readPointer);
			pointerSet.readPointer = reinterpret_cast<unsigned short*>(pointerSet.readPointer) + 1;

			return *this;
		}

		inline const CSerializedBuffer& CSerializedBuffer::operator>>(_Inout_ int& dest) const
		{
			dest = *reinterpret_cast<int*>(pointerSet.readPointer);
			pointerSet.readPointer = reinterpret_cast<int*>(pointerSet.readPointer) + 1;

			return *this;
		}

		inline const CSerializedBuffer& CSerializedBuffer::operator>>(_Inout_ unsigned int& dest) const
		{
			dest = *reinterpret_cast<unsigned int*>(pointerSet.readPointer);
			pointerSet.readPointer = reinterpret_cast<unsigned int*>(pointerSet.readPointer) + 1;

			return *this;
		}

		inline const CSerializedBuffer& CSerializedBuffer::operator>>(_Inout_ long& dest) const
		{
			dest = *reinterpret_cast<long*>(pointerSet.readPointer);
			pointerSet.readPointer = reinterpret_cast<int*>(pointerSet.readPointer) + 1;

			return *this;
		}

		inline const CSerializedBuffer& CSerializedBuffer::operator>>(_Inout_ unsigned long& dest) const
		{
			dest = *reinterpret_cast<unsigned long*>(pointerSet.readPointer);
			pointerSet.readPointer = reinterpret_cast<unsigned int*>(pointerSet.readPointer) + 1;

			return *this;
		}

		inline const CSerializedBuffer& CSerializedBuffer::operator>>(_Inout_ __int64& dest) const
		{
			dest = *reinterpret_cast<__int64*>(pointerSet.readPointer);
			pointerSet.readPointer = reinterpret_cast<__int64*>(pointerSet.readPointer) + 1;

			return *this;
		}

		inline const CSerializedBuffer& CSerializedBuffer::operator>>(_Inout_ unsigned __int64& dest) const
		{
			dest = *reinterpret_cast<unsigned __int64*>(pointerSet.readPointer);
			pointerSet.readPointer = reinterpret_cast<unsigned __int64*>(pointerSet.readPointer) + 1;

			return *this;
		}

		inline const CSerializedBuffer& CSerializedBuffer::operator>>(_Inout_ float& dest) const
		{
			dest = *reinterpret_cast<float*>(pointerSet.readPointer);
			pointerSet.readPointer = reinterpret_cast<float*>(pointerSet.readPointer) + 1;

			return *this;
		}

		inline const CSerializedBuffer& CSerializedBuffer::operator>>(_Inout_ double& dest) const
		{
			dest = *reinterpret_cast<double*>(pointerSet.readPointer);
			pointerSet.readPointer = reinterpret_cast<double*>(pointerSet.readPointer) + 1;

			return *this;
		}

		/* ---------------------------------------------------------------------------------------- */
	}
}
