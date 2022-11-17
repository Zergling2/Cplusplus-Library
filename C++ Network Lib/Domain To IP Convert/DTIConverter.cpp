#include "DTIConverter.h"

BOOL DomainToIPW(const wchar_t* szDomain, IN_ADDR* pAddr)
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	ADDRINFOW* pAddrInfo;
	SOCKADDR_IN* pSockAddr;

	if (GetAddrInfoW(szDomain, L"0", NULL, &pAddrInfo) != 0)
		return FALSE;

	pSockAddr = (SOCKADDR_IN*)pAddrInfo->ai_addr;
	*pAddr = pSockAddr->sin_addr;
	FreeAddrInfoW(pAddrInfo);

	WSACleanup();

	return TRUE;
}

void PrintDomainIPW(const wchar_t* szDomain)
{
	WSADATA wsa;
	IN_ADDR addrInfo;

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return;

	ADDRINFOW* pAddrInfo;
	SOCKADDR_IN* pSockAddr;

	if (GetAddrInfoW(szDomain, L"0", NULL, &pAddrInfo) != 0)
		return;

	while (pAddrInfo != NULL)
	{
		addrInfo = ((SOCKADDR_IN*)pAddrInfo->ai_addr)->sin_addr;
		printf("%d.%d.%d.%d\n", addrInfo.S_un.S_un_b.s_b1, addrInfo.S_un.S_un_b.s_b2, addrInfo.S_un.S_un_b.s_b3, addrInfo.S_un.S_un_b.s_b4);
		pAddrInfo = pAddrInfo->ai_next;
	}

	FreeAddrInfoW(pAddrInfo);
	WSACleanup();
}