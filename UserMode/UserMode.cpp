// UserMode.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "memory.h"




DWORD ac_client;
HANDLE Driver;
Req request;






void DrwPlayers(HANDLE processId, PVOID baseAddress, PVOID resolvedAddress) {
	//iterate through playerlist and print each
	//ac_client + 0x18AC04 read
	PVOID PlayerList = (PVOID)(ac_client + 0x18AC04);
	//GET VIEW MATRIX
	PVOID vMatrix;
	MemRead((ULONG)0x0057DFD0, &vMatrix, 16 * sizeof(float));
	if (!FindDMAAddy(processId, (PVOID)0x0057DFD0, 0, 0, &vMatrix))
		printf("[-] failed to get vMatrix\n");



	ULONG offsets[] = {0x4,0x4};
	ULONG32 i;
	int cnt=0;
	vec2 ScreenRes;
	vec3 Cords;
	PVOID buffer;
	

	for (i = 0x4;; i= i + 0x4) {
		//read x 
		offsets[0] = i;
		offsets[1] = 0x4;
		if (!FindDMAAddy(processId, (PVOID)PlayerList, offsets, 2, &resolvedAddress)) {
			printf("[!] reached end of playerList with count: %d\n", cnt);
			break;
		}

		Cords.x = *(float*)&resolvedAddress;
		//read y
		if ((int)Cords.x == 0)
			break;
		offsets[1] = 0x8;
		FindDMAAddy(processId, (PVOID)PlayerList, offsets, 2, &resolvedAddress);
		Cords.y = *(float*)&resolvedAddress;
		//read z
		offsets[1] = 0xC;
		FindDMAAddy(processId, (PVOID)PlayerList, offsets, 2, &resolvedAddress);
		Cords.z = *(float*)&resolvedAddress;

		
		
		WorldToScreen(Cords,ScreenRes,(float*)vMatrix,1440,2560);
		printf("(%d, %d )", ScreenRes.x, ScreenRes.y);
		Cords.x = Cords.y = Cords.z = 0;
		cnt++;
	}
	

}



int main()
{
   //get handle of Driver
	ac_client = 0x400000;
	Driver = CreateFile(L"\\\\.\\AdamsDriverSymbol", GENERIC_READ | GENERIC_WRITE,0,nullptr,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr);
    
	request.ProcessPid = (HANDLE)GetProcId(L"ac_client.exe");
	if (request.ProcessPid == NULL) {
		printf("[-] Usermode: Failed to get Pid\n");
		CloseHandle(Driver);
		std::cin.get();	
		return 0;
	}
	
	//attach to process

	if (!DeviceIoControl(Driver, IO_PROCESS_ATTACH, &request, sizeof(request), &request, sizeof(request), nullptr, nullptr)) {
		printf("[-] Usermode: Failed to call DeviceControl to attach % lu\n", GetLastError());
		CloseHandle(Driver);
		std::cin.get();
		return 0;
	}
	printf("[+] Usermode: Successfully called DeviceControl 1\n");
	//get basemoudles virtual address
	ULONG offsets[] = {0,0,0};
	void* Resolved;

	//test wether reading worked
	PVOID BaseAddress = (void*)(ac_client + 0x18AC04);

	FindDMAAddy(request.ProcessPid, BaseAddress, offsets,2, &Resolved);

	void* buffer=0;

	printf("health value from kernel: %d\n", *(int*)&Resolved);
	
	DrawPlayers(request.ProcessPid, BaseAddress, &Resolved);






	CloseHandle(Driver);
	std::cin.get();
	return 0;
		


}

