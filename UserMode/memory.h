
#pragma once
#include "C:\Users\adam\source\repos\KernelAssultCubeProject\SharedFiles\KernalUser.h"


struct vec4 {
	float x;
	float y;
	float z;
	float w;
};



struct vec3 {
	float x;
	float y;
	float z;
};
struct vec2 {
	float x;
	float y;

};

struct Matrix4x4 {
	float m[16] = { 0 };  // 4x4 matrix //0x57DFD0 mat1
};										//0x057E010 mat2


struct  Offsets {

	//Entity List               [ac_client.exe + 0x18AC04]
	DWORD ent = 0x19BDCF0;
	DWORD vMatrix = 0x0057DFD0;

};


DWORD GetProcId(const wchar_t* procName);
// The function to call the kernel driver and get the resolved DMA address
bool FindDMAAddy(HANDLE processId, PVOID baseAddress, ULONG* offsets, int offsetCount, PVOID* resolvedAddress);

bool MemRead(ULONG Addrs, void* buffer, SIZE_T size);

bool MemWrite(ULONG Addrs);

bool WorldToScreen(vec3 pos, vec2& screen, float matrix[16], int windowWidth, int windowHeight);