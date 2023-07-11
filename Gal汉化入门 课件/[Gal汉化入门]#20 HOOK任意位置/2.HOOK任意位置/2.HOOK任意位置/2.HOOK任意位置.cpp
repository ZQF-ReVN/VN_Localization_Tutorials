#include "test.h"
#include <iostream>

DWORD orgAddr = 0x00493ED3;
DWORD retAddr = orgAddr + 0x5;
LPCWSTR str = L"Dir-A";

VOID __declspec(naked) MyHook()
{
    __asm
    {
        pushad
        pushfd
    }

    __asm
    {
        push 0x3F4
        push 0x0
        push 0x0
        push str
        push str
        push 0x0
        call dword ptr ds : [0x005D1214]
    }
    MessageBoxW(NULL, L"Dir-A", NULL, MB_OK);
    __asm
    {
        popfd
        popad
    }
    __asm call dword ptr ds : [0x005D1214]
    __asm jmp retAddr
}

VOID WriteHookCode(DWORD tarAddr,DWORD orgAddr)
{
    DWORD oldProtect = 0;
    VirtualProtect((LPVOID)orgAddr, 0x6, PAGE_EXECUTE_READWRITE, &oldProtect);

    DWORD rvaAddr = tarAddr - orgAddr - 0x5;

    BYTE code[] = { 0xE9,0x90,0x90,0x90,0x90,0x90 };

    memcpy(&code[1], &rvaAddr, 0x4);

    memcpy((void*)orgAddr, code, 0x6);
}

int main()
{
    WriteHookCode((DWORD)MyHook, orgAddr);
    RunMethod1();
    std::cout << "Hello World!\n";
}
