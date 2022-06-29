#pragma once
#include <Windows.h>

typedef int (WINAPI* pMessageBoxTimeoutA)(
    IN HWND hWnd,
    IN LPCSTR lpText,
    IN LPCSTR lpCaption,
    IN UINT uType,
    IN WORD wLanguageId,
    IN DWORD dwMilliseconds);
pMessageBoxTimeoutA MessageBoxTimeoutA;

typedef int(WINAPI* pMessageBoxTimeoutW)(
    IN HWND hWnd,
    IN LPCWSTR lpText,
    IN LPCWSTR lpCaption,
    IN UINT uType,
    IN WORD wLanguageId,
    IN DWORD dwMilliseconds);
pMessageBoxTimeoutW MessageBoxTimeoutW;

VOID ConvertStr(CHAR* procName)
{
    for (size_t i = 0; i < 19; i++)
    {
        procName[i] = ~procName[i];
    }
}

FARPROC GetFuncAddress(CHAR* procName)
{
    ConvertStr(procName);

    HMODULE hDll = LoadLibraryW(L"user32.dll");
    if (hDll == NULL)
    {
        ExitProcess(0);
    }

    FARPROC procAddr = GetProcAddress(hDll, procName);
    FreeLibrary(hDll);

    return procAddr;
}

VOID RunMethod1()
{
    CHAR strW[] = { 0xb2,0x9a,0x8c,0x8c,0x9e,0x98,0x9a,0xbd,0x90,0x87,0xab,0x96,0x92,0x9a,0x90,0x8a,0x8b,0xa8,0xff };

    MessageBoxTimeoutW = (pMessageBoxTimeoutW)GetFuncAddress(strW);
    MessageBoxTimeoutW(NULL, L"Test this BoxW", L"Method1", MB_OK, NULL, 500);

    //MessageBoxTimeoutA = (pMessageBoxTimeoutA)GetFuncAddress(strW);
    //MessageBoxTimeoutA(NULL, "Test this BoxA", "Method1", MB_OK, NULL, 500);
}