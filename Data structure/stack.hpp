#pragma once

#include <iostream>

namespace SJNET
{
	namespace LIB
	{
		template <typename T>
		class stack
		{
		public:
			inline stack(size_t size = 64);		// Failure will result in an access violation on 0x00000030.
			inline ~stack();
			inline T top();
			inline const T top() const;
			inline void push(const T data);
			inline void pop();
			inline size_t size() const;
		private:
			T* p_stackStartPoint;
			size_t stackSize;
			alignas(64) T* p_cursor;
			size_t numOfData;
		};

		template<typename T>
		inline stack<T>::stack(size_t size) : stackSize(size), numOfData(0)
		{
			p_stackStartPoint = new(std::nothrow) T[size];

			if (!p_stackStartPoint)
				*reinterpret_cast<int*>(0x00000030) = 0;		// Forced error.

			p_cursor = p_stackStartPoint;
		}

		template<typename T>
		inline stack<T>::~stack()
		{
			delete p_stackStartPoint;
		}

		template<typename T>
		inline T stack<T>::top()
		{
			if (numOfData == 0)
				*reinterpret_cast<int*>(0x00000031) = 0;		// Forced error.

			return p_stackStartPoint[numOfData - 1];
		}

		template<typename T>
		inline const T stack<T>::top() const
		{
			if (numOfData == 0)
				*reinterpret_cast<int*>(0x00000031) = 0;		// Forced error.

			return p_stackStartPoint[numOfData - 1];
		}

		template<typename T>
		inline void stack<T>::push(const T data)
		{
			if (numOfData >= stackSize)
				*reinterpret_cast<int*>(0x00000032) = 0;		// Forced error.

			p_stackStartPoint[numOfData++] = data;
		}

		template<typename T>
		inline void stack<T>::pop()
		{
			if(numOfData-- <= 0)
				*reinterpret_cast<int*>(0x00000033) = 0;		// Forced error.
		}

		template<typename T>
		inline size_t stack<T>::size() const
		{
			return numOfData;
		}
	}
}
