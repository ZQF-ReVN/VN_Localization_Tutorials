#include "Tools.h"


BOOL DetourAttachFunc(PVOID ppRawFunc, PVOID pNewFunc)
{
	DetourRestoreAfterWith();
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	LONG erroAttach = DetourAttach((PVOID*)ppRawFunc, pNewFunc);
	LONG erroCommit = DetourTransactionCommit();

	if (erroAttach == NO_ERROR && erroCommit == NO_ERROR) return false;

	MessageBoxW(NULL, L"DetourAttach Failed!!", NULL, NULL);

	return true;
}

BOOL WriteMemory(LPVOID lpAddress, LPCVOID lpBuffer, SIZE_T nSize)
{
	DWORD oldProtect = 0;
	BOOL isProtect = VirtualProtectEx(GetCurrentProcess(), lpAddress, nSize, PAGE_EXECUTE_READWRITE, &oldProtect);
	BOOL isWritten = WriteProcessMemory(GetCurrentProcess(), lpAddress, lpBuffer, nSize, NULL);

	if (isProtect && isWritten) return true;

	MessageBoxW(NULL, L"WriteMemory Failed!!", NULL, NULL);

	return false;
}

BOOL WriteHookCode(DWORD dwRawAddress, DWORD dwNewAddress, SIZE_T szHookCode)
{
	BYTE code[0xF];
	memset(code, 0x90, 0xF);

	*(BYTE* )(code + 0) = 0xE9;
	*(DWORD*)(code + 1) = dwNewAddress - dwRawAddress - 5;

	if (WriteMemory((LPVOID)dwRawAddress, &code, szHookCode)) return true;

	MessageBoxW(NULL, L"WriteHookCode Failed!!", NULL, NULL);

	return false;
}



//********************
//* Hook CreateFontA *
//********************
#ifdef Hook_CreateFontA_Func

DWORD g_dwCharSet = 0x86;
LPCSTR g_lpFontName = "ºÚÌå";

typedef HFONT(WINAPI* pCreateFontA)(INT, INT, INT, INT, INT, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, LPCSTR);
pCreateFontA rawCreateFontA = CreateFontA;

HFONT WINAPI newCreateFontA
(
	INT cHeight, INT cWidth, INT cEscapement, INT cOrientation, INT cWeight,
	DWORD bItalic, DWORD bUnderline, DWORD bStrikeOut, DWORD iCharSet, DWORD iOutPrecision,
	DWORD iClipPrecision, DWORD iQuality, DWORD iPitchAndFamily, LPCSTR pszFaceName
)
{
	return rawCreateFontA
	(
		cHeight, cWidth, cEscapement, cOrientation, cWeight,
		bItalic, bUnderline, bStrikeOut, g_dwCharSet, iOutPrecision,
		iClipPrecision, iQuality, iPitchAndFamily, g_lpFontName
	);
}

BOOL HookCreateFontA(DWORD dwCharSet, LPCSTR lpFontName)
{
	g_dwCharSet = dwCharSet;
	g_lpFontName = lpFontName;
	return DetourAttachFunc(&rawCreateFontA, newCreateFontA);
}

#endif // Hook_CreateFontA_Func