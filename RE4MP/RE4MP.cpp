// RE4MP.cpp : This file contains the 'main' function. Program execution begins and ends there.
// NOTE OVERWRITING CODE REQUIRES FULL ADDRESS (DO NOT INCLUDE BASE ADDRESS OF RAM)
//

#include <Windows.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <vector>
#include "RE4MP.h"

DWORD procID; // A 32-bit unsigned integer, DWORDS are mostly used to store Hexadecimal Addresses
HANDLE handle;

typedef __int64(__thiscall* fn_cSubChar_movePos)(int cSubCharPtr, std::vector<float>* toPos, float spd);
fn_cSubChar_movePos cSubChar_movePos;

typedef __int64(__fastcall* fn_cSubChar_move)(int cSubCharPtr);
fn_cSubChar_move cSubChar_move;

typedef int(__thiscall* fn_cManager_cEm__create)(int* emMgr, uint32_t id);
fn_cManager_cEm__create cManager_cEm__create;

DWORD WINAPI MainThread(LPVOID param) {

    DWORD base_addr = (DWORD)GetModuleHandleA(0);
    HWND hwnd = FindWindowA(NULL, "Resident Evil 4"); // HWND (Windows window) by Window Name
    
    GetWindowThreadProcessId(hwnd, &procID); // Getting our Process ID, as an ex. like 000027AC
    handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID); // Opening the Process with All Access

    HookFunctions(base_addr);

    CodeInjection(base_addr);
    //std::string wstr = std::to_string(base_addr + 0x3567e0);
    
    while (true) {

        if (GetAsyncKeyState(VK_F5)) {
            cManager_cEm__create(GetEmMgrPointer(base_addr), 44);
        }

        // cSubChar manual control
        float moveFactor = 20.0;
        if (GetAsyncKeyState(VK_NUMPAD0)) {
            float* pos = GetPlayerPosition(base_addr);
            MoveSubChar(base_addr, pos);
        }

        if (GetAsyncKeyState(VK_UP)) {
            float* pos = GetSubCharDestinationPos(base_addr);
            pos[2] += moveFactor;
            MoveSubChar(base_addr, pos);
        }
        else if (GetAsyncKeyState(VK_DOWN)) {
            float* pos = GetSubCharDestinationPos(base_addr);
            pos[2] -= moveFactor;
            MoveSubChar(base_addr, pos);
        }

        if (GetAsyncKeyState(VK_LEFT)) {
            float* pos = GetSubCharDestinationPos(base_addr);
            pos[0] += moveFactor;
            MoveSubChar(base_addr, pos);
        }
        else if (GetAsyncKeyState(VK_RIGHT)) {
            float* pos = GetSubCharDestinationPos(base_addr);
            pos[0] -= moveFactor;
            MoveSubChar(base_addr, pos);
        }

        // y pos
        if (GetAsyncKeyState(VK_F6)) {
            float* pos = GetSubCharPos(base_addr);
            pos[1] += moveFactor;
            MoveSubChar(base_addr, pos);
        }
        else if (GetAsyncKeyState(VK_F7)) {
            float* pos = GetSubCharPos(base_addr);
            pos[1] -= moveFactor;
            MoveSubChar(base_addr, pos);
        }

        // exit scenario
        if (GetAsyncKeyState(VK_END)) {
            break;
        }

        Sleep(10); //may cause lag
    }

    FreeLibraryAndExitThread((HMODULE) param, 0);
    return 0;
}


BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID l)
{
        switch (reason)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(NULL, NULL, MainThread, hModule, NULL, NULL);
        break;
    default:
        break;
    }

    return TRUE;
}

void OverwriteBytes(DWORD addr, char* bytes, int len)
{
    WriteProcessMemory(handle, (LPVOID)(addr), bytes, len, NULL);
}

void HookFunctions(DWORD base_addr) 
{
    cSubChar_movePos = (fn_cSubChar_movePos)(base_addr + 0x3567e0); //0x7567e0
    cSubChar_move = (fn_cSubChar_move)(base_addr + 0x361a70);
    cManager_cEm__create = (fn_cManager_cEm__create)(base_addr + 0x1b2350);

}

void CodeInjection(DWORD base_addr)
{
    char twoNop[2] = { 0x90, 0x90 }; //nop
    char threeNop[3] = { 0x90, 0x90, 0x90 }; //nop
    char fiveNop[5] = { 0x90, 0x90, 0x90, 0x90, 0x90 }; //nop

    // Disable cSubChar setting partner location for movement
    /*OverwriteBytes((base_addr + 0x35e9fa), twoNop, 2);
    OverwriteBytes((base_addr + 0x35ea02), threeNop, 3);
    OverwriteBytes((base_addr + 0x35ea0b), threeNop, 3);

    OverwriteBytes((base_addr + 0x35e9cb), twoNop, 2);
    OverwriteBytes((base_addr + 0x35e9cd), threeNop, 3);
    OverwriteBytes((base_addr + 0x35e9d0), threeNop, 3);

    OverwriteBytes((base_addr + 0x35eb4c), fiveNop, 5); // Routing

    OverwriteBytes((base_addr + 0x35e9df), fiveNop, 5); // odd math funcs
    OverwriteBytes((base_addr + 0x35eb00), fiveNop, 5);
    */

    // spawn subChar everywhere
    //OverwriteBytes((base_addr + 0x2c520b), twoNop, 2);
}

int* GetEmMgrPointer(DWORD base_addr)
{
    return (int*)(base_addr + 0x7fDB04);
}

int* PlayerPointer(DWORD base_addr)
{
    return (int*)(base_addr + 0x857054);
}

float* GetPlayerPosition(DWORD base_addr)
{
    int* ptr = PlayerPointer(base_addr);
    return (float*)((*ptr) + 0x94);
}

int* SubCharPointer(DWORD base_addr) 
{
    return (int*)(base_addr + 0x857060);
}

float* GetSubCharPos(DWORD base_addr)
{
    int* ptr = SubCharPointer(base_addr);
    return (float*)((*ptr) + 0x94);
}

float* GetSubCharDestinationPos(DWORD base_addr) 
{
    int* ptr = SubCharPointer(base_addr);
    return (float*)((*ptr) + 0x450);
}

char* GetSubCharGravityCheck(DWORD base_addr)
{
    int* ptr = SubCharPointer(base_addr);
    return (char*)((*ptr) + 0x2ce);
}

void MoveSubChar(DWORD base_addr, float* toPos)
{
    float* ptr = GetSubCharDestinationPos(base_addr);
    *GetSubCharGravityCheck(base_addr) = 2; // disable grabity
    ptr[0] = toPos[0];
    ptr[1] = toPos[1];
    ptr[2] = toPos[2];
}