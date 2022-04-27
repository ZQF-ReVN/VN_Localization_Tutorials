#include "Tools.h"

//#pragma comment(linker,"/subsystem:\"Windows\" /entry:\"mainCRTStartup\"")


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	
	if (!GetStart(L"C:\\Users\\Dir-A\\Desktop\\1\\SiglusEngine.exe"))
	{
		ExitProcess(0);
	}

	DWORD my_ImageBase = GetImageBase();

	BYTE my_fix[] = { 0x11,0xD1 };
	WriteMemory((LPVOID)(my_ImageBase + 0x3489AE), my_fix, sizeof(my_fix));

	//getchar();
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



