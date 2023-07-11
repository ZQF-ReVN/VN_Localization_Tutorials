#pragma once
#include <Windows.h>
#include "detours.h"

#define Hook_CreateFontA_Func


BOOL WriteMemory(LPVOID lpAddress, LPCVOID lpBuffer, SIZE_T nSize);
BOOL WriteHookCode(DWORD dwRawAddress, DWORD dwNewAddress, SIZE_T szHookCode);
BOOL HookCreateFontA(DWORD dwCharSet, LPCSTR lpFontName);