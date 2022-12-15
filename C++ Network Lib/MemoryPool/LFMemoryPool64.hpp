// Recommend using C++ 17 or later.
// <WARNING> Supported only for 64-bit applications.

/*

������ �Ű� ����

[in] flOptions
�� �Ҵ� �ɼ��Դϴ�. �̷��� �ɼ��� �� �Լ� ȣ���� ���� �� ���� ���� �ļ� �׼����� ������ �ݴϴ�. �� �Ű� ������ ����
�� �� 0�� �̻��� �� �ֽ��ϴ�.

�� �ǹ�

HEAP_CREATE_ENABLE_EXECUTE			�ϵ����� ������ ���� ������ �����ϴ� ��� �� ������ �Ҵ�� ��� �޸�
0x00040000							����� �ڵ� ������ ����մϴ�. ������ �ڵ带 �����ϴ� ���ø����̼ǿ��� ��
									�÷��� ���� ����մϴ�. HEAP_CREATE_ENABLE_EXECUTE �������� �ʰ� ����
									�����̼��� ��ȣ�� ���������� �ڵ带 �����Ϸ��� �ϸ� ���ø����̼��� ���� ��
									�� STATUS_ACCESS_VIOLATION ���ܸ� �޽��ϴ�.

HEAP_GENERATE_EXCEPTIONS			�ý����� NULL�� ��ȯ�ϴ� ��� HeapAlloc �� HeapReAlloc ȣ�⿡ ���� ����(��:
0x00000004							�޸� ���� ����)�� ��Ÿ���� ���ܸ� �߻��մϴ�.


HEAP_NO_SERIALIZE					����ȭ�� �׼����� �� �Լ��� �� ���� �׼����� �� ������ �ʽ��ϴ�. �� �ɼ���
0x00000001							��� �ļ� �� �Լ� ȣ�⿡ ����˴ϴ�. �Ǵ� ���� �� �Լ� ȣ�⿡�� �� �ɼ��� ��
									���� �� �ֽ��ϴ�.
									�� �ɼ��� ����Ͽ� ���� ������ LFH(���� ����ȭ ��)�� ����� �� �����ϴ�.

									�� �ɼ��� ����Ͽ� ���� ���� ��� �� �����ϴ�.

									����ȭ�� �׼����� ���� �ڼ��� ������ �� �׸��� ���� ������ �����ϼ���.

[in] dwInitialSize

���� �ʱ� ũ��(����Ʈ)�Դϴ�. �� ���� ���� ���� Ŀ�Ե� �ʱ� �޸� ���� �����մϴ�. ���� �ý��� ������ ũ���� �����
�ݿø��˴ϴ�. ���� dwMaximumSize���� �۾ƾ� �մϴ�.

�� �Ű� ������ 0�̸� �Լ��� �� �������� Ŀ���մϴ�. ȣ��Ʈ ��ǻ�Ϳ��� �������� ũ�⸦ Ȯ���Ϸ��� GetSystemInfo ��
���� ����մϴ�.

[in] dwMaximumSize

���� �ִ� ũ��(����Ʈ)�Դϴ�. HeapCreate �Լ��� dwMaximumSize�� �ý��� ������ ũ���� ����� �ݿø��� ���� ����
���� ���μ����� ���� �ּ� �������� �ش� ũ���� ����� �����մϴ�. HeapAlloc �Ǵ� HeapReAlloc �Լ��� �Ҵ� ��û��
dwInitialSize�� ������ ũ�⸦ �ʰ��ϸ� �ý����� ���� �ִ� ũ����� ���� ���� �߰� �޸� �������� Ŀ���մϴ�.

dwMaximumSize�� 0�� �ƴϸ� �� ũ�Ⱑ �����Ǿ� �ִ� ũ�⸦ �ʰ��� �� �����ϴ�. ���� ������ �Ҵ��� �� �ִ� ���� ū
�޸� ����� 32��Ʈ ���μ����� ��� 512KB �̸��̰� 64��Ʈ ���μ����� ��� 1,024KB���� �ణ �۽��ϴ�. ���� �ִ�
ũ�Ⱑ ����� ������ ��ŭ ū ��쿡�� �� ū ����� �Ҵ��ϴ� ��û�� �����մϴ�.

dwMaximumSize�� 0�̸� ���� ũ�Ⱑ Ŀ�� �� �ֽ��ϴ�. ���� ũ��� ��� ������ �޸𸮿� ���ؼ��� ���ѵ˴ϴ�. ���� ũ
�� ���� ���� �ѵ����� ū �޸� ����� �Ҵ��϶�� ��û�� �ڵ����� �������� �ʽ��ϴ�. ���, �ý����� VirtualAlloc �Լ�
�� ȣ���Ͽ� ū ��Ͽ� �ʿ��� �޸𸮸� �����ɴϴ�. ū �޸� ����� �Ҵ��ؾ� �ϴ� ���ø����̼��� dwMaximumSize ��
0���� �����ؾ� �մϴ�.

*/

#pragma once

#pragma warning(disable:26495)

#define _WINSOCKAPI_
#include <Windows.h>
#undef _WINSOCKAPI_
#include <strsafe.h>
#include "Logger.h"
#include "ForceCrash.h"

enum class LFMPDestructorCallOption
{
	AUTO,
	MANUAL
};

namespace SJNET
{
	namespace LIB
	{
		constexpr ULONG64 LF_MASK = 0xFFFF800000000000U;
		constexpr ULONG64 LF_MASK_NOT = 0x00007FFFFFFFFFFFU;
		constexpr ULONG64 LF_MASK_INC = 0x0000800000000000U;

		inline void* GetLFStampRemovedAddress(void* _Address)
		{
			return reinterpret_cast<void*>(reinterpret_cast<ULONG64>(_Address) & LF_MASK_NOT);
		}

		inline ULONG64 GetLFStamp(void* _Address)
		{
			return reinterpret_cast<ULONG64>(_Address) & LF_MASK;
		}

		template<typename ObjectType, LFMPDestructorCallOption opt>
		class CLFMemoryPool64
		{
		private:
			struct Node
			{
			public:
				// template<typename... Types> Node(Types ...args);	// deprecated
				// void* operator new(size_t size);			// deprecated
				// void operator delete(void* _Block);		// deprecated
				ObjectType object;
				CLFMemoryPool64<ObjectType, opt>* _pSource;
				Node* _pBelow;
			};
		public:
			CLFMemoryPool64(DWORD flOptions, SIZE_T dwInitialSize, SIZE_T dwMaximumSize);	// (���!) dwMaximumSize�� 0�� �ƴ� ��� LFH ���� �Ұ� �� �ش� �������� ��ü �ִ� �Ҵ� ũ��� 0x7FFF8�� ���ѵ�.
			~CLFMemoryPool64();
			template<typename ...Types> ObjectType* GetObjectFromPool(Types ...args);
			void ReturnObjectToPool(ObjectType* _Ret);
		private:
			void GetObjectErrorCrash();
			void ReturnObjectErrorCrash(CLFMemoryPool64<ObjectType, opt>* address);
			alignas(8) Node* volatile _pTop;		//[hard-coded] Because this library only supports 64-bit.
			HANDLE hHeapHandle;
			DWORD dwHeapOptions;
		};

		template<typename ObjectType, LFMPDestructorCallOption opt>
		CLFMemoryPool64<ObjectType, opt>::CLFMemoryPool64(DWORD flOptions, SIZE_T dwInitialSize, SIZE_T dwMaximumSize)
			: _pTop(nullptr)
			, hHeapHandle(HeapCreate(flOptions, dwInitialSize, dwMaximumSize))
			, dwHeapOptions(flOptions)
		{
		}

		template<typename ObjectType, LFMPDestructorCallOption opt>
		CLFMemoryPool64<ObjectType, opt>::~CLFMemoryPool64()
		{
			HeapDestroy(this->hHeapHandle);
		}

		template<typename ObjectType, LFMPDestructorCallOption opt>
		template<typename ...Types>
		ObjectType* CLFMemoryPool64<ObjectType, opt>::GetObjectFromPool(Types ...args)
		{
			Node* pNode;
			Node* pNewTop;
			do
			{
				pNode = this->_pTop;
				if (GetLFStampRemovedAddress(pNode) == nullptr)
				{
					pNode = reinterpret_cast<Node*>(HeapAlloc(this->hHeapHandle, this->dwHeapOptions, sizeof(CLFMemoryPool64<ObjectType, opt>::Node)));
					if (pNode == NULL) CLFMemoryPool64<ObjectType, opt>::GetObjectErrorCrash();
					new (pNode) ObjectType(args...);		// placement new
					pNode->_pSource = this;
					return reinterpret_cast<ObjectType*>(pNode);
				}
				pNewTop = reinterpret_cast<Node*>(reinterpret_cast<LONG64>(reinterpret_cast<Node*>(GetLFStampRemovedAddress(pNode))->_pBelow) | GetLFStamp(pNode));	// ���� Pop������ ������ �� ������ų �ʿ�� ����.
			} while (pNode != InterlockedCompareExchangePointer(reinterpret_cast<volatile PVOID*>(&_pTop), pNewTop, pNode));

			pNode = reinterpret_cast<Node*>(GetLFStampRemovedAddress(pNode));
			new (pNode) ObjectType(args...);		// placement new
			return reinterpret_cast<ObjectType*>(pNode);
		}

		template<typename ObjectType, LFMPDestructorCallOption opt>
		void CLFMemoryPool64<ObjectType, opt>::ReturnObjectToPool(ObjectType* pObj)
		{
			if (reinterpret_cast<Node*>(pObj)->_pSource != this)
				CLFMemoryPool64<ObjectType, opt>::ReturnObjectErrorCrash(reinterpret_cast<Node*>(pObj)->_pSource);

			if constexpr (opt == LFMPDestructorCallOption::AUTO)
				pObj->~ObjectType();

			Node* pTemp;
			Node* pNewTop;
			do
			{
				pTemp = this->_pTop;
				reinterpret_cast<Node*>(pObj)->_pBelow = reinterpret_cast<Node*>(GetLFStampRemovedAddress(pTemp));
				pNewTop = reinterpret_cast<Node*>(reinterpret_cast<ULONG64>(pObj) | (GetLFStamp(pTemp) + LF_MASK_INC));
			} while (pTemp != InterlockedCompareExchangePointer(reinterpret_cast<volatile PVOID*>(&_pTop), pNewTop, pTemp));
		}

		template<typename ObjectType, LFMPDestructorCallOption opt>
		void CLFMemoryPool64<ObjectType, opt>::GetObjectErrorCrash()
		{
			SJNET::API::CFileLogger fLogger(L"CMemoryPool_LF Error_", true);
			wchar_t logBuffer[256];
			StringCbPrintfW(logBuffer, sizeof(logBuffer), L"[LFMPOOL ERROR] std::bad_alloc exception occurred. Pool address is 0x%p", this);
			fLogger.WriteLog(logBuffer, LogType::LT_CRITICAL);
			ForceCrash(0xAFAFAFAF);
		}

		template<typename ObjectType, LFMPDestructorCallOption opt>
		void CLFMemoryPool64<ObjectType, opt>::ReturnObjectErrorCrash(CLFMemoryPool64<ObjectType, opt>* address)
		{
			SJNET::API::CFileLogger fLogger(L"CMemoryPool_LF Error_", true);
			wchar_t logBuffer[256];
			StringCbPrintfW(logBuffer, sizeof(logBuffer), L"[LFMPOOL ERROR] An object created from another pool(0x%p) has been returned to the pool located at address 0x%p.", address, this);
			fLogger.WriteLog(logBuffer, LogType::LT_CRITICAL);
			ForceCrash(0xAFAFAFAF);
		}
	}
}

#pragma warning(default:26495)
