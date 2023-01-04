// Recommend using C++ 17 or later.
// <WARNING> Supported only for 64-bit applications.
#pragma once

#include "LFMemoryPool64.hpp"

namespace SJNET
{
	namespace LIB
	{
		template<typename T>
		class CLFStack64
		{
		private:
			class CLFStack64Node
			{
				friend CLFStack64;
			public:
				inline CLFStack64Node(T data);
			private:
				T data;
				CLFStack64Node* _pBelow;
			};
		public:
			CLFStack64(DWORD flOptions, SIZE_T dwInitialSize, SIZE_T dwMaximumSize);
			void Push(const T data);
			bool Pop(T& buf);
			bool Empty() { return !GetLFStampRemovedAddress(_pTop); }
		private:
			alignas(8) CLFStack64Node* _pTop;		// 64비트 경계에 정렬
			alignas(64) CLFMemoryPool64<CLFStack64<T>::CLFStack64Node, LFMPDestructorCallOption::AUTO> _NodePool;
		};

		template<typename T>
		CLFStack64<T>::CLFStack64Node::CLFStack64Node(T data)
			: data(data)
		{
		}

		template<typename T>
		CLFStack64<T>::CLFStack64(DWORD flOptions, SIZE_T dwInitialSize, SIZE_T dwMaximumSize)
			: _pTop(nullptr)
			, _NodePool(flOptions, dwInitialSize, dwMaximumSize)
		{
		}

		template<typename T>
		void CLFStack64<T>::Push(const T data)
		{
			CLFStack64Node* pNewNode = _NodePool.New(data);

			CLFStack64Node* volatile pTemp;
			CLFStack64Node* volatile pNewTop;
			do
			{
				pTemp = this->_pTop;		// load
				pNewNode->_pBelow = reinterpret_cast<CLFStack64Node*>(GetLFStampRemovedAddress(pTemp));	// load
				pNewTop = reinterpret_cast<CLFStack64Node*>(reinterpret_cast<ULONG64>(pNewNode) | (GetLFStamp(pTemp) + LF_MASK_INC));
			} while (pTemp != InterlockedCompareExchangePointer(reinterpret_cast<volatile PVOID*>(&_pTop), pNewTop, pTemp));
		}

		template<typename T>
		bool CLFStack64<T>::Pop(T& buf)
		{
			CLFStack64Node* volatile pNode;
			CLFStack64Node* volatile pNewTop;
			do
			{
				pNode = this->_pTop;
				if (GetLFStampRemovedAddress(pNode) == nullptr)
					return false;
				pNewTop = reinterpret_cast<CLFStack64Node*>(reinterpret_cast<LONG64>(reinterpret_cast<CLFStack64Node*>(GetLFStampRemovedAddress(pNode))->_pBelow) | GetLFStamp(pNode));	// 굳이 Pop에서도 GetLFStamp 반환값에 LF_MASK_INC를 더할 필요는 없다고 생각됨.
			} while (pNode != InterlockedCompareExchangePointer(reinterpret_cast<volatile PVOID*>(&_pTop), pNewTop, pNode));

			pNode = reinterpret_cast<CLFStack64Node*>(GetLFStampRemovedAddress(pNode));
			buf = pNode->data;
			_NodePool.Delete(pNode);
			return true;
		}
	}
}
