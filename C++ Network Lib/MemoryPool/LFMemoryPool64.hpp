// Recommend using C++ 17 or later.
// <WARNING> Supported only for 64-bit applications.

/*

생성자 매개 변수

[in] flOptions
힙 할당 옵션입니다. 이러한 옵션은 힙 함수 호출을 통해 새 힙에 대한 후속 액세스에 영향을 줍니다. 이 매개 변수는 다음
값 중 0개 이상일 수 있습니다.

값 의미

HEAP_CREATE_ENABLE_EXECUTE			하드웨어에서 데이터 실행 방지를 적용하는 경우 이 힙에서 할당된 모든 메모리
0x00040000							블록은 코드 실행을 허용합니다. 힙에서 코드를 실행하는 애플리케이션에서 이
									플래그 힙을 사용합니다. HEAP_CREATE_ENABLE_EXECUTE 지정하지 않고 애플
									리케이션이 보호된 페이지에서 코드를 실행하려고 하면 애플리케이션은 상태 코
									드 STATUS_ACCESS_VIOLATION 예외를 받습니다.

HEAP_GENERATE_EXCEPTIONS			시스템은 NULL을 반환하는 대신 HeapAlloc 및 HeapReAlloc 호출에 대한 실패(예:
0x00000004							메모리 부족 조건)를 나타내는 예외를 발생합니다.


HEAP_NO_SERIALIZE					직렬화된 액세스는 힙 함수가 이 힙에 액세스할 때 사용되지 않습니다. 이 옵션은
0x00000001							모든 후속 힙 함수 호출에 적용됩니다. 또는 개별 힙 함수 호출에서 이 옵션을 지
									정할 수 있습니다.
									이 옵션을 사용하여 만든 힙에는 LFH(하위 조각화 힙)를 사용할 수 없습니다.

									이 옵션을 사용하여 만든 힙은 잠글 수 없습니다.

									직렬화된 액세스에 대한 자세한 내용은 이 항목의 설명 섹션을 참조하세요.

[in] dwInitialSize

힙의 초기 크기(바이트)입니다. 이 값은 힙에 대해 커밋된 초기 메모리 양을 결정합니다. 값은 시스템 페이지 크기의 배수로
반올림됩니다. 값은 dwMaximumSize보다 작아야 합니다.

이 매개 변수가 0이면 함수는 한 페이지를 커밋합니다. 호스트 컴퓨터에서 페이지의 크기를 확인하려면 GetSystemInfo 함
수를 사용합니다.

[in] dwMaximumSize

힙의 최대 크기(바이트)입니다. HeapCreate 함수는 dwMaximumSize를 시스템 페이지 크기의 배수로 반올림한 다음 힙에
대한 프로세스의 가상 주소 공간에서 해당 크기의 블록을 예약합니다. HeapAlloc 또는 HeapReAlloc 함수의 할당 요청이
dwInitialSize에 지정된 크기를 초과하면 시스템은 힙의 최대 크기까지 힙에 대한 추가 메모리 페이지를 커밋합니다.

dwMaximumSize가 0이 아니면 힙 크기가 고정되어 최대 크기를 초과할 수 없습니다. 또한 힙에서 할당할 수 있는 가장 큰
메모리 블록은 32비트 프로세스의 경우 512KB 미만이고 64비트 프로세스의 경우 1,024KB보다 약간 작습니다. 힙의 최대
크기가 블록을 포함할 만큼 큰 경우에도 더 큰 블록을 할당하는 요청이 실패합니다.

dwMaximumSize가 0이면 힙의 크기가 커질 수 있습니다. 힙의 크기는 사용 가능한 메모리에 의해서만 제한됩니다. 고정 크
기 힙에 대한 한도보다 큰 메모리 블록을 할당하라는 요청은 자동으로 실패하지 않습니다. 대신, 시스템은 VirtualAlloc 함수
를 호출하여 큰 블록에 필요한 메모리를 가져옵니다. 큰 메모리 블록을 할당해야 하는 애플리케이션은 dwMaximumSize 를
0으로 설정해야 합니다.

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
			CLFMemoryPool64(DWORD flOptions, SIZE_T dwInitialSize, SIZE_T dwMaximumSize);	// (경고!) dwMaximumSize가 0이 아닌 경우 LFH 적용 불가 및 해당 힙에서의 객체 최대 할당 크기는 0x7FFF8로 제한됨.
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
				pNewTop = reinterpret_cast<Node*>(reinterpret_cast<LONG64>(reinterpret_cast<Node*>(GetLFStampRemovedAddress(pNode))->_pBelow) | GetLFStamp(pNode));	// 굳이 Pop에서도 스탬프 값 증가시킬 필요는 없음.
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
