#include <windows.h>

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

void WriteMemory(LPVOID lpAddress, LPCVOID lpBuffer, SIZE_T nSize)
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

void ReadMemory(LPCVOID lpAddress, LPVOID lpBuffer, SIZE_T nSize)
{

	if (!ReadProcessMemory(pi.hProcess, lpAddress, lpBuffer, nSize, NULL))
	{
		MessageBoxW(NULL, L"ReadMemory Failed!!", NULL, MB_OK);
	}
}

BOOL CmpMemory(LPCVOID Buffer1, LPCVOID Buffer2, SIZE_T nSize)
{
	if (!memcmp(Buffer1, Buffer2, nSize))
	{
		MessageBoxW(NULL, L"identical", NULL, MB_OK);
		return TRUE;
	}
	else
	{
		MessageBoxW(NULL, L"inequality", NULL, MB_OK);
		return FALSE;
	}
}


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	if (!GetStart(L"SiglusEngine.exe"))
	{
		ExitProcess(0);
	}

	DWORD my_ImageBase = GetImageBase();

	//BYTE my_fix[] = { 0x11,0xD1 };
	//WriteMemory((LPVOID)(my_ImageBase + 0x3489AE), my_fix, sizeof(my_fix));

	BYTE write_Bytes[] = { 0xB0,0x01,0xC3 };
	WriteMemory((LPVOID)(my_ImageBase + 0x22B8E0), write_Bytes, sizeof(write_Bytes));

	BYTE write_Bytes1[] = { 0x90,0x90 };
	WriteMemory((LPVOID)(my_ImageBase + 0x21FB8C), write_Bytes1, sizeof(write_Bytes1));

	//BYTE read_Bytes[7] = {0};
	//ReadMemory((LPCVOID)(my_ImageBase + 0x29366B), read_Bytes,sizeof(read_Bytes));

	//CmpMemory(write_Bytes, read_Bytes, 7);

	ResumeThread(pi.hThread);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}
