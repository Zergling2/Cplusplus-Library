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
					// ���� ���Ͷ��� �����ϸ� ��� �Ǵ°� -> �ϴ� �� �ڵ�� tail�� pNewNode�� ����Ű�� �����ν� tail�� next�� null�� �ǰ� �Ͽ�
					// �ٸ� ��������� Push�� �� �� �ְ� ���ִ� ������ �Ѵ�.
					// �����ϸ� tail�� next�� null�� �ɶ����� �� � ������鵵 Push�� ��� ����.

					// �ٵ� �̰� �����ϴ� ��츦 �����غ���.
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
			_NodePool.ReturnObjectToPool(reinterpret_cast<CLFQueue64Node*>(GetLFStampRemovedAddress(pTempHead)));		// ���� ���� ���� ���� ���� ����
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
