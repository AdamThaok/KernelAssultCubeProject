
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

	//call driver
	if (!DeviceIoControl(Driver, IO_READ_REQUEST, &RequestTmp, sizeof(RequestTmp), &RequestTmp, sizeof(RequestTmp), nullptr, nullptr))
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







bool WorldToScreen(vec3 pos, vec2& screen, float matrix[16], int windowWidth, int windowHeight)
{
	// Matrix-vector Product, multiplying world(eye) coordinates by projection matrix = clipCoords
	vec4 clipCoords;
	clipCoords.x = pos.x * matrix[0] + pos.y * matrix[4] + pos.z * matrix[8] + matrix[12];
	clipCoords.y = pos.x * matrix[1] + pos.y * matrix[5] + pos.z * matrix[9] + matrix[13];
	clipCoords.z = pos.x * matrix[2] + pos.y * matrix[6] + pos.z * matrix[10] + matrix[14];
	clipCoords.w = pos.x * matrix[3] + pos.y * matrix[7] + pos.z * matrix[11] + matrix[15];

	if (clipCoords.w < 0.1f)
		return false;  // Object is behind the camera

	// Perspective division, dividing by clip.W = Normalized Device Coordinates (NDC)
	vec3 NDC;
	NDC.x = clipCoords.x / clipCoords.w;
	NDC.y = clipCoords.y / clipCoords.w;
	NDC.z = clipCoords.z / clipCoords.w;

	// Transform NDC to window coordinates
	screen.x = (windowWidth / 2 * NDC.x) + (NDC.x + windowWidth / 2);
	screen.y = -(windowHeight / 2 * NDC.y) + (NDC.y + windowHeight / 2);

	screen.x /= 2560.0f;
	screen.x *= 2.0f;
	screen.x -= 1.0f;

	screen.y /= 1440.0f;
	screen.y *= 2.0f;
	screen.y -= 1.0f;



	return true;
}