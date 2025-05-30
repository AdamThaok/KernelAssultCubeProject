// UserMode.cpp : This file contains the 'main' function. Program execution begins and ends there.
#pragma once
#include "Draw.h"


#include "memory.h"
#include "offsets.h"



GLFWwindow* overlayWindow =nullptr;
DWORD ac_client;
HANDLE Driver;
Req request;


VOID Enviroment32(PVOID &a) {
	 a= PVOID ((ULONG)a & (ULONG)0x00000000FFFFFFFF);
}

bool CordsRange(float a[3]) {
	if (a[0] == 0 || a[1] == 0 || a[2] == 0)
		return false;
	if (a[0] > 1000 || a[0] < -1000)
		return false;
	return true;
}

void PrintMatrix(float matrix[4][4]) {
	printf("Matrix (4x4):\n");
	for (int row = 0; row < 4; row++) {
		for (int col = 0; col < 4; col++) {
			printf("%8.2f ", matrix[row][col]);
		}
		printf("\n");
	}
}
















void DrawOnScreen(GLFWwindow* window,float a[3],float vMat[4][4]) {
	vec3 cords;
	vec2 screen;
	cords.x = a[0];
	cords.y = a[1];
	cords.z = a[2];
	WorldToScreen(cords, screen, vMat, 2560, 1440);
	printf("Will draw to location: (%f,%f)", screen.x, screen.y);
	render(window, screen.x, screen.y);

}















bool DrawPlayers(GLFWwindow* window,HANDLE processId, PVOID baseAddress, PVOID resolvedAddress) {

	int cnt = 0;
	vec2 ScreenRes;
	vec3 Cords;
	PVOID buffer;
	float cords[3] = { 0 };


	PVOID PlayerList;

	float vMatrix[4][4];
	if (!MemRead((ULONG)(off::ModuleBase + off::EntityList), &PlayerList, sizeof(PlayerList)) || !MemRead((ULONG)(off::ModuleBase + off::vMatrix), &vMatrix, sizeof(vMatrix))) {	//read vMat
		printf("[-] Failed to get vMatrix\n");
		return 0;
	}
	Enviroment32(PlayerList);


	ULONG PlayerCount = 0;
	if (!MemRead(off::ModuleBase + off::PlayerCount, &PlayerCount, sizeof(PlayerCount))) {		//get player count
		printf("[-] Couldnt get player count\n");
		return 0;
	}

	ULONG  playeri=0x0;
	ULONG temp;
	for (ULONG PlayerStride = 0;; PlayerStride = PlayerStride + off::PlayerStride) {
		if (cnt > (PlayerCount / 2))
			break;

		temp = (ULONG)(PlayerList)+(ULONG)PlayerStride;
		if (!MemRead(temp, &playeri, sizeof(playeri)))  //readplayer base pointer, must derefrnce and read cords at offsets 0x4 0x8 0xc
			printf("faul");
		if (MemRead((ULONG)playeri + off::Align, &cords, sizeof(cords)))
			if (CordsRange(cords)) {
				printf("(%f ,%f ,%f)\n", cords[0], cords[1], cords[2]);
				DrawOnScreen(window,cords, vMatrix);
				cnt++;
	}
		
		cords[0] = cords[1] = cords[2] = 0;
		
	}

	
}





int main()
{
	//get handle of Driver
	ac_client = 0x400000;
	Driver = CreateFile(L"\\\\.\\AdamsDriverSymbol", GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);











	request.ProcessPid = (HANDLE)GetProcId(off::ProcessName);
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
	ULONG offsets[] = { 0,0,0 };
	void* Resolved;

	//test wether reading worked
	PVOID BaseAddress = (void*)(ac_client + 0x18AC04);


	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return -1; // Exit the application if GLFW initialization fails
	}




	
		RECT gameWindowRect = getGameWindowRect(off::WindowName);

		// Create an overlay window that matches the size of the game window
		GLFWwindow* overlayWindow = createOverlayWindow(2560, 1440);  // Placeholder, to be resized to match the game window
		if (!overlayWindow) {
			return -1;
		}

		// Set the overlay window's position to match the game window
		setOverlayPosition(overlayWindow, gameWindowRect);

		// Initialize OpenGL settings for the overlay
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);  // Set transparent background



		//main loop
		while (!glfwWindowShouldClose(overlayWindow)) {
			printf("Iterating render\n");
				DrawPlayers(overlayWindow,request.ProcessPid, BaseAddress, &Resolved);
				Sleep(10);
		}




		CloseHandle(Driver);
		std::cin.get();
		return 0;



	
}

