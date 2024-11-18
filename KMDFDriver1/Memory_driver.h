#define MY_KERNEL_MODE
#pragma once
#include "FindDMAAddy_kernel.h"







NTSTATUS MemWrite(PEPROCESS process, uintptr_t addr,  void* buffer, size_t size) {
    if (!process || !addr || !buffer || size == 0) {
        return STATUS_INVALID_PARAMETER;
    }

    SIZE_T copiedBytes = 0;
    NTSTATUS status = MmCopyVirtualMemory(PsGetCurrentProcess(), buffer, process, (PVOID)addr, size, KernelMode, &copiedBytes);

    return status;
}


NTSTATUS MemRead(PEPROCESS process, uintptr_t addr, void* buffer, size_t size) {
    if (!process || !addr || !buffer || size == 0) {
        return STATUS_INVALID_PARAMETER;
    }

    SIZE_T copiedBytes = 0;
    NTSTATUS status = MmCopyVirtualMemory(process, (PVOID)addr, PsGetCurrentProcess(), buffer, size, KernelMode, &copiedBytes);

    return status;
}




