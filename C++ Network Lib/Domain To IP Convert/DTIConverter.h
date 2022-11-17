#pragma once

#pragma comment (lib, "Ws2_32.lib")
#include <winsock2.h>
#include <WS2tcpip.h>
#include <stdio.h>

BOOL DomainToIPW(const wchar_t* szDomain, IN_ADDR* pAddr);

void PrintDomainIPW(const wchar_t* szDomain);