#pragma once
#pragma warning(disable : 4311)
#include "FindDMAAddy_kernel.h"
#define LOG(msg, ...) DbgPrintEx(0,0,msg,__VA_ARGS__)



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
) {



    
    BaseAddress = (PVOID)(UINT32)BaseAddress;


    LOG("32 bit address: %p", BaseAddress);

    NTSTATUS status;
    PVOID OldAddress = BaseAddress;
    PVOID NewAddress;
    NewAddress = (PVOID)(UINT32)BaseAddress;
    SIZE_T bytesCopied;
    //derefrince base first
    LOG("FindDMAAddy start BaseAddress: %p", BaseAddress);

    status = MmCopyVirtualMemory(
        TargetProcess,                     // Target process (source)
        OldAddress,                   // Current address to dereference (base address in this case)
        PsGetCurrentProcess(),             // Current process (target)
        &NewAddress,                   // Buffer to store the dereferenced value
        sizeof(OldAddress),            // Size of the pointer
        KernelMode,                        // KernelMode context
        &bytesCopied   
        // Number of bytes copied
    );
    int i ;
    //afterAddress is derefernced base, now add offsets
    
    PVOID temp;




    for (i = 0; i < OffsetCount; i++) {
        BaseAddress = (PVOID)((UINT_PTR)BaseAddress & 0xFFFFFFFF);
        OldAddress = (PVOID)((UINT_PTR)OldAddress & 0xFFFFFFFF);
        NewAddress = (PVOID)((UINT_PTR)NewAddress & 0xFFFFFFFF);



        LOG("Start Iteration: %d.  OldAddress :%p NewAddress: %p  ", i, OldAddress, NewAddress);

        //i=1 [[base]+off1]


        NewAddress = (PVOID)((ULONG)NewAddress + Offsets[i]);
        LOG("before first read base + %p: offsets[%d]%x", NewAddress,Offsets[i]);
        status = MmCopyVirtualMemory(
            TargetProcess,                     // Target process (source)
            NewAddress,                   // Current address to dereference
            PsGetCurrentProcess(),             // Current process (target)
            &OldAddress,                   // Buffer to store the dereferenced value
            sizeof(OldAddress),            // Size of the pointer
            KernelMode,                        // KernelMode context
            &bytesCopied                       // Number of bytes copied
        );

        if (!NT_SUCCESS(status)) {
            LOG("[-] Failed status in amd Iteration %d\n",i);
            return STATUS_UNSUCCESSFUL;
        }

        BaseAddress = (PVOID)((UINT_PTR)BaseAddress & 0xFFFFFFFF);
        OldAddress = (PVOID)((UINT_PTR)OldAddress & 0xFFFFFFFF);
        NewAddress = (PVOID)((UINT_PTR)NewAddress & 0xFFFFFFFF);



        temp = OldAddress;
        OldAddress = NewAddress; //switch       
        NewAddress = temp;
        LOG("Finisih Iteration: %d.  OldAddress :%p NewAddress: %p  ", i, OldAddress, NewAddress);
        //current contains [[base]+off1]
        //assume [[base]+off1]


    }


    if (!NT_SUCCESS(status)) {
        LOG("[-] Failed status in amd cd");
        return STATUS_UNSUCCESSFUL;
    }

    BaseAddress = (PVOID)((UINT_PTR)BaseAddress & 0xFFFFFFFF);
    OldAddress = (PVOID)((UINT_PTR)OldAddress & 0xFFFFFFFF);
    NewAddress = (PVOID)((UINT_PTR)NewAddress & 0xFFFFFFFF);


    *OutAddress = NewAddress;
     //afterAddress contains desired value
    LOG("Address of value: %p", OldAddress);
    LOG("Must Be value: %p", NewAddress);





    return STATUS_SUCCESS;
}



NTSTATUS GetProcessByPid(HANDLE pid, PEPROCESS* process) {
    if (!process) {
        return STATUS_INVALID_PARAMETER;
    }

    NTSTATUS status = PsLookupProcessByProcessId(pid, process);
    if (!NT_SUCCESS(status)) {
        DbgPrint("Failed to find process by PID: %p (status: 0x%x)\n", pid, status);
    }

    return status;
}