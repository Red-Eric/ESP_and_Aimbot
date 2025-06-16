#include "func.h"

DWORD getPointerAddress(DWORD ptr, std::vector<DWORD> offsets)
{
    DWORD addrs = ptr;
    for (int i = 0; i < offsets.size(); i++) {
        addrs = *(DWORD*)addrs;
        addrs += offsets[i];
    }
    return addrs;
}

void RestoreBytes(uintptr_t* address, const std::vector<BYTE> originalBytes) {
    DWORD oldProtect;
    if (VirtualProtect(address, originalBytes.size(), PAGE_EXECUTE_READWRITE, &oldProtect)) {
        memcpy(reinterpret_cast<void*>(address), originalBytes.data(), originalBytes.size());
        VirtualProtect(address, originalBytes.size(), oldProtect, &oldProtect);
    }
}