#include <windows.h>

BYTE driveType[] = { 0x03 };

BYTE langPass[] = {0xB0,0x01,0xC3};

BYTE evb[] = { 
	0x2A,0x00,0x2E,0x00,
	0x63,0x00,0x70,0x00,
	0x6B,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,
	0x00,0x00 };

BYTE keyPass[] = {
	0x8B,0x15,0x7C,0xFD,0x4C, 
	0x00,0xC7,0x02,0x4D,0x55, 
	0x47,0x36,0xC7,0x42,0x04, 
	0x2D,0x37,0x57,0x36,0xC7, 
	0x42,0x08,0x53,0x2D,0x52, 
	0x44,0xC7,0x42,0x0C,0x54, 
	0x5A,0x2D,0x33,0xC7,0x42, 
	0x10,0x32,0x45,0x4D,0x2D, 
	0xC3 };

BYTE keyPassCall[] = { 0xE8,0x7B,0x7F,0x06,0x00,0x90 };


STARTUPINFOW si;
PROCESS_INFORMATION pi;


BOOL GetStart(LPCWSTR lpApplicationName)
{
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	if (!CreateProcessW(lpApplicationName, NULL, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &si, &pi))
	{
		MessageBoxW(NULL, L"Get Start Failed!!", NULL, MB_OK);
		return FALSE;
	}

	return TRUE;
}

DWORD GetImageBase()
{
	DWORD dwImageBase = 0;

	CONTEXT Regs = {};
	Regs.ContextFlags = CONTEXT_ALL;
	GetThreadContext(pi.hThread, &Regs);

	if (!ReadProcessMemory(pi.hProcess, (LPCVOID)(Regs.Ebx + 0x8), &dwImageBase, sizeof(DWORD), NULL))
	{
		MessageBoxW(NULL, L"Get BaseImage Failed!!", NULL, MB_OK);
		return FALSE;
	}
	return dwImageBase;
}

VOID WriteMemory(LPVOID lpAddress, LPCVOID lpBuffer, SIZE_T nSize)
{
	DWORD oldProtect;

	if (!VirtualProtectEx(pi.hProcess, lpAddress, nSize, PAGE_EXECUTE_READWRITE, &oldProtect))
	{
		MessageBoxW(NULL, L"VirtualProtectEx Failed!!", NULL, MB_OK);
	}
	else if (!WriteProcessMemory(pi.hProcess, lpAddress, lpBuffer, nSize, NULL))
	{
		MessageBoxW(NULL, L"WriteProcessMemory Failed!!", NULL, MB_OK);
	}
}


INT APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	if (!GetStart(L"いますぐお兄ちゃんに妹だっていいたい！.exe"))
	{
		ExitProcess(0);
	}

	DWORD my_ImageBase = GetImageBase();

	//BYTE my_fix[] = { 0xB8,0x01,0x00,0x00,0x00,0xC3 };
	//WriteMemory((LPVOID)(my_ImageBase + 0x56C58), my_fix, sizeof(my_fix));


	WriteMemory((LPVOID)(my_ImageBase + 0x4E9F0), driveType, sizeof(driveType));
	WriteMemory((LPVOID)(my_ImageBase + 0x1EC0F6), evb, sizeof(evb));
	//WriteMemory((LPVOID)(my_ImageBase + 0x56C58), langPass, sizeof(langPass));
	WriteMemory((LPVOID)(my_ImageBase + 0xAFC90), keyPass, sizeof(keyPass));
	WriteMemory((LPVOID)(my_ImageBase + 0x47D10), keyPassCall, sizeof(keyPassCall));






	//BYTE read_Bytes[7] = {0};
	//ReadMemory((LPCVOID)(my_ImageBase + 0x29366B), read_Bytes,sizeof(read_Bytes));

	//CmpMemory(write_Bytes, read_Bytes, 7);

	ResumeThread(pi.hThread);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}