#pragma once
#include<Windows.h>
#include<vector>

DWORD getPointerAddress(DWORD ptr, std::vector<DWORD> offsets);
void RestoreBytes(uintptr_t* address, const std::vector<BYTE> originalBytes);