// Recommend using C++ 17 or later.
// <WARNING> Supported only for 64-bit applications.
#pragma once

#include "LFMemoryPool64.hpp"

namespace SJNET
{
	namespace LIB
	{
		template<typename T>
		class CLFQueue64
		{
		private:
			struct CLFQueue64Node
			{
				friend CLFQueue64;
			public:
				inline CLFQueue64Node(T data);
			private:
				T data;
				CLFQueue64Node* _pNext;
			};
			struct CLFQueue64DummyNode
			{
				char dummy[sizeof(T)];
				CLFQueue64Node* _pNext;
			};
		public:
			template<typename ...Types> CLFQueue64(DWORD flOptions, SIZE_T dwInitialSize, SIZE_T dwMaximumSize, Types ...args);
			void Push(const T data);
			bool Pop(T& buf);
		private:
			alignas(64) CLFQueue64Node* _pHead;
			alignas(64) CLFQueue64Node* _pTail;
			alignas(64) CLFMemoryPool64<CLFQueue64Node, LFMPDestructorCallOption::AUTO> _NodePool;
		};

		template<typename T>
		template<typename ...Types>
		inline CLFQueue64<T>::CLFQueue64(DWORD flOptions, SIZE_T dwInitialSize, SIZE_T dwMaximumSize, Types ...dummyConstructorArgs)
			: _NodePool(flOptions, dwInitialSize, dwMaximumSize)
		{
			_pHead = _pTail = _NodePool.GetObjectFromPool(dummyConstructorArgs...);
			_pHead->_pNext = nullptr;
		}

		template<typename T>
		void CLFQueue64<T>::Push(const T data)
		{
			CLFQueue64Node* pNewNode = _NodePool.GetObjectFromPool(data);

			CLFQueue64Node* volatile pTempTail;
			CLFQueue64Node* volatile pTempTailNext;
			for (;;)
			{
				do
				{
					pTempTail = this->_pTail;
					pTempTailNext = pTempTail->_pNext;
				} while (pTempTailNext != nullptr);

				if (nullptr == InterlockedCompareExchangePointer(reinterpret_cast<volatile PVOID*>(&pTempTail->_pNext), pNewNode, nullptr))
				{
					InterlockedCompareExchangePointer(reinterpret_cast<volatile PVOID*>(&this->_pTail), pNewNode, pTempTail);
					// 위의 인터락이 실패하면 어떻게 되는가 -> 일단 위 코드는 tail이 pNewNode를 가리키게 함으로써 tail의 next가 null이 되게 하여
					// 다른 스레드들이 Push를 할 수 있게 해주는 역할을 한다.
					// 실패하면 tail의 next가 null이 될때까지 그 어떤 스레드들도 Push를 계속 실패.

					// 근데 이게 실패하는 경우를 생각해보자.
					break;
				}
			}
		}

		template<typename T>
		bool CLFQueue64<T>::Pop(T& buf)
		{
			CLFQueue64Node* volatile pTempHead;
			CLFQueue64Node* volatile pTempHeadNext;
			PVOID pNewHead;
			do
			{
				pTempHead = this->_pHead;
				pTempHeadNext = reinterpret_cast<CLFQueue64Node*>(GetLFStampRemovedAddress(pTempHead))->_pNext;
				if (pTempHeadNext == nullptr)
					return false;
				pNewHead = reinterpret_cast<PVOID>(reinterpret_cast<ULONG64>(pTempHeadNext) | (GetLFStamp(pTempHead) + LF_MASK_INC));
			} while (pTempHead != InterlockedCompareExchangePointer(reinterpret_cast<volatile PVOID*>(&this->_pHead), pNewHead, pTempHead));

			buf = pTempHeadNext->data;
			_NodePool.ReturnObjectToPool(reinterpret_cast<CLFQueue64Node*>(GetLFStampRemovedAddress(pTempHead)));		// 더미 노드로 쓰인 노드는 이제 해제
			return true;
		}

		template<typename T>
		inline CLFQueue64<T>::CLFQueue64Node::CLFQueue64Node(T data)
			: data(data)
			, _pNext(nullptr)
		{
		}
	}
}
