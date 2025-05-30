
#include "memory.h"


extern DWORD ac_client;
extern HANDLE Driver;
extern Req request;






DWORD GetProcId(const wchar_t* procName)
{
	DWORD procId = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnap != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32 procEntry;
		procEntry.dwSize = sizeof(procEntry);

		if (Process32First(hSnap, &procEntry))
		{
			do
			{
				if (!_wcsicmp(procEntry.szExeFile, procName))
				{
					procId = procEntry.th32ProcessID;
					break;
				}
			} while (Process32Next(hSnap, &procEntry));

		}
	}
	CloseHandle(hSnap);
	return procId;
}




// The function to call the kernel driver and get the resolved DMA address
bool FindDMAAddy(HANDLE processId, PVOID baseAddress, ULONG* offsets, int offsetCount, PVOID* resolvedAddress) {
	FIND_DMA_INPUT input = {};
	DWORD bytesReturned = 0;

	// Initialize the input structure
	input.BaseAddress = baseAddress;
	input.OffsetCount = offsetCount;
	input.ProcessPid = processId;




	// Copy the offsets into the input struct
	for (int i = 0; i < offsetCount; i++) {
		input.Offsets[i] = offsets[i];
	}

	// Send the IOCTL to the driver
	printf("[+] Usermode: Calling DMA of kernel\n");
	bool success = DeviceIoControl(
		Driver,                // Handle to the driver (obtained from CreateFile)
		IO_DMA,                 // IOCTL code for the DMA operation
		&input,                 // Input buffer
		sizeof(FIND_DMA_INPUT),  // Size of the input buffer
		&input,                 // Output buffer (same structure will be used to retrieve the resolved address)
		sizeof(FIND_DMA_INPUT),  // Size of the output buffer
		&bytesReturned,          // Number of bytes returned (not used in this case)
		NULL                    // Overlapped I/O (NULL for synchronous calls)
	);
	int health = (int)(ULONG)input.Resolved;

	if (!success) {
		printf("DeviceIoControl failed with error: %d\n", GetLastError());
		return false;
	}

	// The driver will fill in the resolved address in input.Resolved
	*resolvedAddress = input.Resolved;
	return true;
}

bool MemRead(ULONG Addrs, void* buffer, SIZE_T size) {
	//init struct
	Req RequestTmp;
	RequestTmp.Target = (void*)Addrs;
	RequestTmp.ReturnSize = 0;
	RequestTmp.Size = size;
	RequestTmp.Buffer = buffer;
	//call driver
	if (!DeviceIoControl(Driver, IO_READ_REQUEST, &RequestTmp, sizeof(RequestTmp), &RequestTmp, sizeof(RequestTmp), nullptr, nullptr) || RequestTmp.ReturnSize!=size)
		return false;
	

	return true;
}

bool MemWrite(ULONG Addrs) {
	//init struct
	Req RequestTmp;
	RequestTmp.Buffer = (void*)Addrs;
	RequestTmp.ReturnSize = 0;
	RequestTmp.Size = 0;
	//call driver
	return DeviceIoControl(Driver, IO_WRITE_REQUEST, &RequestTmp, sizeof(RequestTmp), &RequestTmp, sizeof(RequestTmp), nullptr, nullptr);
}







bool WorldToScreen(vec3 pos, vec2& screen, float matrix[4][4], int windowWidth, int windowHeight)
{
	// Matrix-vector Product: Multiply world coordinates by the view-projection matrix
	vec4 clipCoords;
	clipCoords.x = pos.x * matrix[0][0] + pos.y * matrix[1][0] + pos.z * matrix[2][0] + matrix[3][0];
	clipCoords.y = pos.x * matrix[0][1] + pos.y * matrix[1][1] + pos.z * matrix[2][1] + matrix[3][1];
	clipCoords.z = pos.x * matrix[0][2] + pos.y * matrix[1][2] + pos.z * matrix[2][2] + matrix[3][2];
	clipCoords.w = pos.x * matrix[0][3] + pos.y * matrix[1][3] + pos.z * matrix[2][3] + matrix[3][3];

	// Check if the object is behind the camera
	if (clipCoords.w <= 0.0f)
		return false;

	// Perspective division: Transform clip coordinates to NDC
	vec3 NDC;
	NDC.x = clipCoords.x / clipCoords.w;
	NDC.y = clipCoords.y / clipCoords.w;
	NDC.z = clipCoords.z / clipCoords.w;

	// Transform NDC to window-space coordinates
	screen.x = (NDC.x + 1.0f) * 0.5f * windowWidth;
	screen.y = (1.0f - NDC.y) * 0.5f * windowHeight; // Flip Y-axis for screen coordinates

	return true;
}
