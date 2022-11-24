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

		class SerializedBuffer
		{
		protected:
			struct PointerSet
			{
				size_t bufferSize;
				char* bufferBeginAddress;
				void* bufferEndAddress;
				mutable void* readPointer;
				mutable void* writePointer;
			};
		public:
			SerializedBuffer();		// Constructor

			inline void* GetBufferBeginAddress() const { return stPointerSet.bufferBeginAddress; }
			inline void* GetBufferEndAddress() const { return stPointerSet.bufferEndAddress; }
			inline void* GetWritePointer() const { return stPointerSet.writePointer; }
			inline void SetWritePointer(void* p) { this->stPointerSet.writePointer = p; }
			inline const void* GetReadPointer() const { return stPointerSet.readPointer; }
			inline void SetReadPointer(void* p) { this->stPointerSet.readPointer = p; }
			inline void MoveWritePointer(int offset) { stPointerSet.writePointer = reinterpret_cast<char*>(stPointerSet.writePointer) + offset; }
			inline void MoveReadPointer(int offset) { stPointerSet.readPointer = reinterpret_cast<char*>(stPointerSet.readPointer) + offset; }

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

			inline SerializedBuffer& operator<<(_In_ char data);
			inline SerializedBuffer& operator<<(_In_ unsigned char data);
			inline SerializedBuffer& operator<<(_In_ short data);
			inline SerializedBuffer& operator<<(_In_ unsigned short data);
			inline SerializedBuffer& operator<<(_In_ int data);
			inline SerializedBuffer& operator<<(_In_ unsigned int data);
			inline SerializedBuffer& operator<<(_In_ long data);
			inline SerializedBuffer& operator<<(_In_ unsigned long data);
			inline SerializedBuffer& operator<<(_In_ __int64 data);
			inline SerializedBuffer& operator<<(_In_ unsigned __int64 data);
			inline SerializedBuffer& operator<<(_In_ float data);
			inline SerializedBuffer& operator<<(_In_ double data);

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

			inline const SerializedBuffer& operator>>(_Inout_ char& dest) const;
			inline const SerializedBuffer& operator>>(_Inout_ unsigned char& dest) const;
			inline const SerializedBuffer& operator>>(_Inout_ short& dest) const;
			inline const SerializedBuffer& operator>>(_Inout_ unsigned short& dest) const;
			inline const SerializedBuffer& operator>>(_Inout_ int& dest) const;
			inline const SerializedBuffer& operator>>(_Inout_ unsigned int& dest) const;
			inline const SerializedBuffer& operator>>(_Inout_ long& dest) const;
			inline const SerializedBuffer& operator>>(_Inout_ unsigned long& dest) const;
			inline const SerializedBuffer& operator>>(_Inout_ __int64& dest) const;
			inline const SerializedBuffer& operator>>(_Inout_ unsigned __int64& dest) const;
			inline const SerializedBuffer& operator>>(_Inout_ float& dest) const;
			inline const SerializedBuffer& operator>>(_Inout_ double& dest) const;

			inline size_t GetBufferSize() const { return stPointerSet.bufferSize; }
			inline size_t GetFreeSize()	const { return reinterpret_cast<size_t>(stPointerSet.bufferEndAddress) - reinterpret_cast<size_t>(stPointerSet.writePointer); }
			inline size_t GetUsingSize() const { return reinterpret_cast<size_t>(stPointerSet.writePointer) - reinterpret_cast<size_t>(stPointerSet.bufferBeginAddress); }
			inline void Reuse() { stPointerSet.readPointer = stPointerSet.writePointer = stPointerSet.bufferBeginAddress; }
		protected:
			inline void ForceCrash(size_t _violationAddr) const { *reinterpret_cast<int*>(_violationAddr) = 0; }
			PointerSet stPointerSet;
			char buffer[SIZE];
		};

		inline SerializedBuffer::SerializedBuffer()
		{
			stPointerSet.bufferSize = SIZE;
			stPointerSet.bufferBeginAddress = buffer;
			stPointerSet.bufferEndAddress = buffer + SIZE;
			stPointerSet.readPointer = buffer;
			stPointerSet.writePointer = buffer;
		}

		inline void SerializedBuffer::In(_In_ char data)
		{
			*reinterpret_cast<char*>(stPointerSet.writePointer) = data;
			stPointerSet.writePointer = reinterpret_cast<char*>(stPointerSet.writePointer) + 1;
		}

		inline void SerializedBuffer::In(_In_ unsigned char data)
		{
			*reinterpret_cast<unsigned char*>(stPointerSet.writePointer) = data;
			stPointerSet.writePointer = reinterpret_cast<unsigned char*>(stPointerSet.writePointer) + 1;
		}

		inline void SerializedBuffer::In(_In_ short data)
		{
			*reinterpret_cast<short*>(stPointerSet.writePointer) = data;
			stPointerSet.writePointer = reinterpret_cast<short*>(stPointerSet.writePointer) + 1;
		}

		inline void SerializedBuffer::In(_In_ unsigned short data)
		{
			*reinterpret_cast<unsigned short*>(stPointerSet.writePointer) = data;
			stPointerSet.writePointer = reinterpret_cast<unsigned short*>(stPointerSet.writePointer) + 1;
		}

		inline void SerializedBuffer::In(_In_ int data)
		{
			*reinterpret_cast<int*>(stPointerSet.writePointer) = data;
			stPointerSet.writePointer = reinterpret_cast<int*>(stPointerSet.writePointer) + 1;
		}

		inline void SerializedBuffer::In(_In_ unsigned int data)
		{
			*reinterpret_cast<unsigned int*>(stPointerSet.writePointer) = data;
			stPointerSet.writePointer = reinterpret_cast<unsigned int*>(stPointerSet.writePointer) + 1;
		}

		inline void SerializedBuffer::In(_In_ long data)
		{
			*reinterpret_cast<long*>(stPointerSet.writePointer) = data;
			stPointerSet.writePointer = reinterpret_cast<int*>(stPointerSet.writePointer) + 1;
		}

		inline void SerializedBuffer::In(_In_ unsigned long data)
		{
			*reinterpret_cast<unsigned long*>(stPointerSet.writePointer) = data;
			stPointerSet.writePointer = reinterpret_cast<unsigned int*>(stPointerSet.writePointer) + 1;
		}

		inline void SerializedBuffer::In(_In_ __int64 data)
		{
			*reinterpret_cast<__int64*>(stPointerSet.writePointer) = data;
			stPointerSet.writePointer = reinterpret_cast<__int64*>(stPointerSet.writePointer) + 1;
		}

		inline void SerializedBuffer::In(_In_ unsigned __int64 data)
		{
			*reinterpret_cast<unsigned __int64*>(stPointerSet.writePointer) = data;
			stPointerSet.writePointer = reinterpret_cast<unsigned __int64*>(stPointerSet.writePointer) + 1;
		}

		inline void SerializedBuffer::In(_In_ float data)
		{
			*reinterpret_cast<float*>(stPointerSet.writePointer) = data;
			stPointerSet.writePointer = reinterpret_cast<float*>(stPointerSet.writePointer) + 1;
		}

		inline void SerializedBuffer::In(_In_ double data)
		{
			*reinterpret_cast<double*>(stPointerSet.writePointer) = data;
			stPointerSet.writePointer = reinterpret_cast<double*>(stPointerSet.writePointer) + 1;
		}

		inline void SerializedBuffer::InBytes(_In_ const void* pBuf, size_t cbSize)
		{
			memmove(stPointerSet.writePointer, pBuf, cbSize);
			stPointerSet.writePointer = reinterpret_cast<char*>(stPointerSet.writePointer) + cbSize;
		}

		/* ---------------------------------------------------------------------------------------- */

		inline SerializedBuffer& SerializedBuffer::operator<<(_In_ char data)
		{
			*reinterpret_cast<char*>(stPointerSet.writePointer) = data;
			stPointerSet.writePointer = reinterpret_cast<char*>(stPointerSet.writePointer) + 1;

			return *this;
		}

		inline SerializedBuffer& SerializedBuffer::operator<<(_In_ unsigned char data)
		{
			*reinterpret_cast<unsigned char*>(stPointerSet.writePointer) = data;
			stPointerSet.writePointer = reinterpret_cast<unsigned char*>(stPointerSet.writePointer) + 1;

			return *this;
		}

		inline SerializedBuffer& SerializedBuffer::operator<<(_In_ short data)
		{
			*reinterpret_cast<short*>(stPointerSet.writePointer) = data;
			stPointerSet.writePointer = reinterpret_cast<short*>(stPointerSet.writePointer) + 1;

			return *this;
		}

		inline SerializedBuffer& SerializedBuffer::operator<<(_In_ unsigned short data)
		{
			*reinterpret_cast<unsigned short*>(stPointerSet.writePointer) = data;
			stPointerSet.writePointer = reinterpret_cast<unsigned short*>(stPointerSet.writePointer) + 1;

			return *this;
		}

		inline SerializedBuffer& SerializedBuffer::operator<<(_In_ int data)
		{
			*reinterpret_cast<int*>(stPointerSet.writePointer) = data;
			stPointerSet.writePointer = reinterpret_cast<int*>(stPointerSet.writePointer) + 1;

			return *this;
		}

		inline SerializedBuffer& SerializedBuffer::operator<<(_In_ unsigned int data)
		{
			*reinterpret_cast<unsigned int*>(stPointerSet.writePointer) = data;
			stPointerSet.writePointer = reinterpret_cast<unsigned int*>(stPointerSet.writePointer) + 1;

			return *this;
		}

		inline SerializedBuffer& SerializedBuffer::operator<<(_In_ long data)
		{
			*reinterpret_cast<long*>(stPointerSet.writePointer) = data;
			stPointerSet.writePointer = reinterpret_cast<int*>(stPointerSet.writePointer) + 1;

			return *this;
		}

		inline SerializedBuffer& SerializedBuffer::operator<<(_In_ unsigned long data)
		{
			*reinterpret_cast<unsigned long*>(stPointerSet.writePointer) = data;
			stPointerSet.writePointer = reinterpret_cast<unsigned int*>(stPointerSet.writePointer) + 1;

			return *this;
		}

		inline SerializedBuffer& SerializedBuffer::operator<<(_In_ __int64 data)
		{
			*reinterpret_cast<__int64*>(stPointerSet.writePointer) = data;
			stPointerSet.writePointer = reinterpret_cast<__int64*>(stPointerSet.writePointer) + 1;

			return *this;
		}

		inline SerializedBuffer& SerializedBuffer::operator<<(_In_ unsigned __int64 data)
		{
			*reinterpret_cast<unsigned __int64*>(stPointerSet.writePointer) = data;
			stPointerSet.writePointer = reinterpret_cast<unsigned __int64*>(stPointerSet.writePointer) + 1;

			return *this;
		}

		inline SerializedBuffer& SerializedBuffer::operator<<(_In_ float data)
		{
			*reinterpret_cast<float*>(stPointerSet.writePointer) = data;
			stPointerSet.writePointer = reinterpret_cast<float*>(stPointerSet.writePointer) + 1;

			return *this;
		}

		inline SerializedBuffer& SerializedBuffer::operator<<(_In_ double data)
		{
			*reinterpret_cast<double*>(stPointerSet.writePointer) = data;
			stPointerSet.writePointer = reinterpret_cast<double*>(stPointerSet.writePointer) + 1;

			return *this;
		}


		/* ---------------------------------------------------------------------------------------- */

		inline void SerializedBuffer::Out(_Inout_ char& dest) const
		{
			dest = *reinterpret_cast<char*>(stPointerSet.readPointer);
			stPointerSet.readPointer = reinterpret_cast<char*>(stPointerSet.readPointer) + 1;
		}

		inline void SerializedBuffer::Out(_Inout_ unsigned char& dest) const
		{
			dest = *reinterpret_cast<unsigned char*>(stPointerSet.readPointer);
			stPointerSet.readPointer = reinterpret_cast<unsigned char*>(stPointerSet.readPointer) + 1;
		}

		inline void SerializedBuffer::Out(_Inout_ short& dest) const
		{
			dest = *reinterpret_cast<short*>(stPointerSet.readPointer);
			stPointerSet.readPointer = reinterpret_cast<short*>(stPointerSet.readPointer) + 1;
		}

		inline void SerializedBuffer::Out(_Inout_ unsigned short& dest) const
		{
			dest = *reinterpret_cast<unsigned short*>(stPointerSet.readPointer);
			stPointerSet.readPointer = reinterpret_cast<unsigned short*>(stPointerSet.readPointer) + 1;
		}

		inline void SerializedBuffer::Out(_Inout_ int& dest) const
		{
			dest = *reinterpret_cast<int*>(stPointerSet.readPointer);
			stPointerSet.readPointer = reinterpret_cast<int*>(stPointerSet.readPointer) + 1;
		}

		inline void SerializedBuffer::Out(_Inout_ unsigned int& dest) const
		{
			dest = *reinterpret_cast<unsigned int*>(stPointerSet.readPointer);
			stPointerSet.readPointer = reinterpret_cast<unsigned int*>(stPointerSet.readPointer) + 1;
		}

		inline void SerializedBuffer::Out(_Inout_ long& dest) const
		{
			dest = *reinterpret_cast<long*>(stPointerSet.readPointer);
			stPointerSet.readPointer = reinterpret_cast<int*>(stPointerSet.readPointer) + 1;
		}

		inline void SerializedBuffer::Out(_Inout_ unsigned long& dest) const
		{
			dest = *reinterpret_cast<unsigned long*>(stPointerSet.readPointer);
			stPointerSet.readPointer = reinterpret_cast<unsigned int*>(stPointerSet.readPointer) + 1;
		}

		inline void SerializedBuffer::Out(_Inout_ __int64& dest) const
		{
			dest = *reinterpret_cast<__int64*>(stPointerSet.readPointer);
			stPointerSet.readPointer = reinterpret_cast<__int64*>(stPointerSet.readPointer) + 1;
		}

		inline void SerializedBuffer::Out(_Inout_ unsigned __int64& dest) const
		{
			dest = *reinterpret_cast<unsigned __int64*>(stPointerSet.readPointer);
			stPointerSet.readPointer = reinterpret_cast<unsigned __int64*>(stPointerSet.readPointer) + 1;
		}

		inline void SerializedBuffer::Out(_Inout_ float& dest) const
		{
			dest = *reinterpret_cast<float*>(stPointerSet.readPointer);
			stPointerSet.readPointer = reinterpret_cast<float*>(stPointerSet.readPointer) + 1;
		}

		inline void SerializedBuffer::Out(_Inout_ double& dest) const
		{
			dest = *reinterpret_cast<double*>(stPointerSet.readPointer);
			stPointerSet.readPointer = reinterpret_cast<double*>(stPointerSet.readPointer) + 1;
		}

		inline void SerializedBuffer::OutBytes(_Inout_ void* pBuf, size_t cbSize) const
		{
			memmove(pBuf, stPointerSet.readPointer, cbSize);
			stPointerSet.readPointer = reinterpret_cast<char*>(stPointerSet.readPointer) + cbSize;
		}

		/* ---------------------------------------------------------------------------------------- */

		inline const SerializedBuffer& SerializedBuffer::operator>>(_Inout_ char& dest) const
		{
			dest = *reinterpret_cast<char*>(stPointerSet.readPointer);
			stPointerSet.readPointer = reinterpret_cast<char*>(stPointerSet.readPointer) + 1;

			return *this;
		}

		inline const SerializedBuffer& SerializedBuffer::operator>>(_Inout_ unsigned char& dest) const
		{
			dest = *reinterpret_cast<unsigned char*>(stPointerSet.readPointer);
			stPointerSet.readPointer = reinterpret_cast<unsigned char*>(stPointerSet.readPointer) + 1;

			return *this;
		}

		inline const SerializedBuffer& SerializedBuffer::operator>>(_Inout_ short& dest) const
		{
			dest = *reinterpret_cast<short*>(stPointerSet.readPointer);
			stPointerSet.readPointer = reinterpret_cast<short*>(stPointerSet.readPointer) + 1;

			return *this;
		}

		inline const SerializedBuffer& SerializedBuffer::operator>>(_Inout_ unsigned short& dest) const
		{
			dest = *reinterpret_cast<unsigned short*>(stPointerSet.readPointer);
			stPointerSet.readPointer = reinterpret_cast<unsigned short*>(stPointerSet.readPointer) + 1;

			return *this;
		}

		inline const SerializedBuffer& SerializedBuffer::operator>>(_Inout_ int& dest) const
		{
			dest = *reinterpret_cast<int*>(stPointerSet.readPointer);
			stPointerSet.readPointer = reinterpret_cast<int*>(stPointerSet.readPointer) + 1;

			return *this;
		}

		inline const SerializedBuffer& SerializedBuffer::operator>>(_Inout_ unsigned int& dest) const
		{
			dest = *reinterpret_cast<unsigned int*>(stPointerSet.readPointer);
			stPointerSet.readPointer = reinterpret_cast<unsigned int*>(stPointerSet.readPointer) + 1;

			return *this;
		}

		inline const SerializedBuffer& SerializedBuffer::operator>>(_Inout_ long& dest) const
		{
			dest = *reinterpret_cast<long*>(stPointerSet.readPointer);
			stPointerSet.readPointer = reinterpret_cast<int*>(stPointerSet.readPointer) + 1;

			return *this;
		}

		inline const SerializedBuffer& SerializedBuffer::operator>>(_Inout_ unsigned long& dest) const
		{
			dest = *reinterpret_cast<unsigned long*>(stPointerSet.readPointer);
			stPointerSet.readPointer = reinterpret_cast<unsigned int*>(stPointerSet.readPointer) + 1;

			return *this;
		}

		inline const SerializedBuffer& SerializedBuffer::operator>>(_Inout_ __int64& dest) const
		{
			dest = *reinterpret_cast<__int64*>(stPointerSet.readPointer);
			stPointerSet.readPointer = reinterpret_cast<__int64*>(stPointerSet.readPointer) + 1;

			return *this;
		}

		inline const SerializedBuffer& SerializedBuffer::operator>>(_Inout_ unsigned __int64& dest) const
		{
			dest = *reinterpret_cast<unsigned __int64*>(stPointerSet.readPointer);
			stPointerSet.readPointer = reinterpret_cast<unsigned __int64*>(stPointerSet.readPointer) + 1;

			return *this;
		}

		inline const SerializedBuffer& SerializedBuffer::operator>>(_Inout_ float& dest) const
		{
			dest = *reinterpret_cast<float*>(stPointerSet.readPointer);
			stPointerSet.readPointer = reinterpret_cast<float*>(stPointerSet.readPointer) + 1;

			return *this;
		}

		inline const SerializedBuffer& SerializedBuffer::operator>>(_Inout_ double& dest) const
		{
			dest = *reinterpret_cast<double*>(stPointerSet.readPointer);
			stPointerSet.readPointer = reinterpret_cast<double*>(stPointerSet.readPointer) + 1;

			return *this;
		}

		/* ---------------------------------------------------------------------------------------- */
	}
}
