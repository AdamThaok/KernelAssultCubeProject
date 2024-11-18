
#define MY_KERNEL_MODE
#pragma once
#include "Memory_driver.h"
#include "C:\\Users\\adam\\source\\repos\\KernelAssultCubeProject\\KMDFDriver1\\KernelFiles.h"


#define LOG(msg, ...) DbgPrintEx(0,0,msg,__VA_ARGS__)










NTSTATUS ProcessPowerCreateClose(PDEVICE_OBJECT pDeviceObject, PIRP Irp){
    UNREFERENCED_PARAMETER(pDeviceObject);
    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, 0);
    return STATUS_SUCCESS;
}

NTSTATUS ProcessPowerControl(PDEVICE_OBJECT pDeviceObject, PIRP Irp) {
    UNREFERENCED_PARAMETER(pDeviceObject);
    NTSTATUS Status = STATUS_UNSUCCESSFUL;
    ULONG ByteIO = 0;


    static PEPROCESS TargetProcess;
   


    Req* request = (Req*)(Irp->AssociatedIrp.SystemBuffer);


    PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
    ULONG ControlCode = stack->Parameters.DeviceIoControl.IoControlCode;


    LOG("[+] Kernel: Successfully called device control! CASE:0x%lx\n",ControlCode);



    switch (ControlCode)
    {
    case IO_READ_REQUEST:
        //read process -> kernel
        LOG("[+] Getting a request to read!\n");
        Status = MmCopyVirtualMemory(TargetProcess,request->Target, PsGetCurrentProcess(),request->Buffer,request->Size, KernelMode,&request->ReturnSize);
        ByteIO = sizeof(Req);
       
            break;

    case IO_WRITE_REQUEST:
        //write kernel -> process
        LOG("[+] Getting a request to write !\n");
        Status = MmCopyVirtualMemory(PsGetCurrentProcess(), request->Target, TargetProcess, request->Buffer, request->Size, KernelMode, &request->ReturnSize);

        break;
    case IO_DMA:
    
        LOG("[+] calling IO_DMA\n");
        FIND_DMA_INPUT* input;
        input = (FIND_DMA_INPUT*)Irp->AssociatedIrp.SystemBuffer;

        LOG("input->offsets[0] val: %x", input->Offsets[0]);
        LOG("input->offsets[1] val: %x", input->Offsets[1]);
     
        Status = FindDMAAddy(TargetProcess, input->BaseAddress, input->Offsets, input->OffsetCount, &input->Resolved);
        
       
        ByteIO = sizeof(FIND_DMA_INPUT);
        if (!NT_SUCCESS(Status))
            LOG("[-] status is bad in FindDMAAddy 0x%08X\n", Status);

       

        break;
    
    case IO_PROCESS_ATTACH:
        LOG("[+] IO Attaching to proess\n");
        Status = GetProcessByPid(request->ProcessPid, &TargetProcess);
           
        break;
    }


   
    Irp->IoStatus.Status = Status;
    Irp->IoStatus.Information = ByteIO;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    
    return Status;


}











VOID UnloadDriver(PDRIVER_OBJECT pDriverObject) {
    // Perform cleanup
    LOG("Deleting object device");
    if (pDriverObject->DeviceObject) {
        IoDeleteDevice(pDriverObject->DeviceObject);
        UNICODE_STRING SymbolicLink;
        RtlInitUnicodeString(&SymbolicLink, L"\\??\\AdamsDriverSymbol");
        IoDeleteSymbolicLink(&SymbolicLink);

    }


}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath) {
    LOG("[+] started the EntryPoint");
    UNREFERENCED_PARAMETER(pRegistryPath);

    pDriverObject->DriverUnload = UnloadDriver;
    NTSTATUS status;
    PDEVICE_OBJECT DeviceObject;

    pDriverObject->MajorFunction[IRP_MJ_CREATE]=ProcessPowerCreateClose;
    pDriverObject->MajorFunction[IRP_MJ_CLOSE]= ProcessPowerCreateClose;
    pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = ProcessPowerControl;



    UNICODE_STRING DeviceName;
    RtlInitUnicodeString(&DeviceName, L"\\Device\\AdamsDriver");
    
    status = IoCreateDevice(pDriverObject, 0, &DeviceName, FILE_DEVICE_UNKNOWN, 0, FALSE, &DeviceObject);

    if (!NT_SUCCESS(status)) {
        LOG("[-] FAILED TO CREATE A DEVICE");
        return status;
    }

    UNICODE_STRING SymbolicLink;
    RtlInitUnicodeString(&SymbolicLink, L"\\??\\AdamsDriverSymbol");
    status = IoCreateSymbolicLink(&SymbolicLink, &DeviceName);

    if (!NT_SUCCESS(status)) {
        LOG("[-] FAILED TO CREATE A IoCreateSymbolicLink");
        IoDeleteDevice(DeviceObject);
        return status;
    }
    DeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;
    LOG("[+] Successfully created and linked device!\n");
    return STATUS_SUCCESS;
}
