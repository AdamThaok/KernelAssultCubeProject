#pragma once

#ifdef MY_KERNEL_MODE
#include <ntddk.h> 

#else  
#include <iostream>
#include <stdio.h>
#include <Windows.h>
#include <TlHelp32.h>
#include <vector>

#endif

#define IO_PROCESS_ATTACH CTL_CODE(FILE_DEVICE_UNKNOWN, 0x814, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IO_READ_REQUEST CTL_CODE(FILE_DEVICE_UNKNOWN, 0x812, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IO_WRITE_REQUEST CTL_CODE(FILE_DEVICE_UNKNOWN, 0x813, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IO_GET_CLIENTADDRESS CTL_CODE(FILE_DEVICE_UNKNOWN, 0x666, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IO_DMA CTL_CODE(FILE_DEVICE_UNKNOWN, 0x810, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)


typedef struct _Req {
    HANDLE ProcessPid;
    PVOID Target;
    PVOID Buffer;
    SIZE_T Size;
    SIZE_T ReturnSize;
} Req;

typedef struct _FIND_DMA_INPUT {
    PVOID BaseAddress;        // Base address of the pointer chain
    PVOID Resolved;           // Resolved address (will be filled by the driver)
    ULONG_PTR Offsets[3];     // Array of offsets to walk the pointer chain
    int OffsetCount;          // Number of offsets in the chain
    HANDLE ProcessPid;        // Target process ID
} FIND_DMA_INPUT;
