#pragma once

struct IN_ADDR;

BOOL DomainToIPW(const wchar_t* szDomain, IN_ADDR* pAddr);

void PrintDomainIPW(const wchar_t* szDomain);