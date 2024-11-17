#pragma once
#define MY_KERNEL_MODE
#include "KernalUser.h"


NTSTATUS MmCopyVirtualMemory(
    PEPROCESS SourceProcess,
    PVOID SourceAddress,    //read data
    PEPROCESS TargetProcess,
    PVOID TargetAddress,    //write data read from Source
    SIZE_T BufferSize,
    KPROCESSOR_MODE PreviousMode,
    PSIZE_T ReturnSize
);

NTSTATUS PsLookupProcessByProcessId(
    HANDLE ProcessId,      // Input: The process ID (PID) of the process to look up
    PEPROCESS* Process     // Output: A pointer to receive the PEPROCESS structure
);






NTSTATUS FindDMAAddy(
    PEPROCESS TargetProcess,  // Target process handle
    PVOID BaseAddress,        // The base address of the pointer chain
    ULONG_PTR* Offsets,       // Array of offsets to walk the pointer chain
    int OffsetCount,          // Number of offsets in the chain
    PVOID* OutAddress         // Pointer to store the final resolved address
);