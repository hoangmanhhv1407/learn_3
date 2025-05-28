//---------------------------------------------------------------------------

#pragma hdrstop

#include "exthook.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#include <Windows.h>
#include <vector>
#include <iostream>

using std::vector;

int extHook::grabBytes()
{
    for (int i = 0;; i++)
    {
        if ((*(BYTE*)(m_ourFunct + i)) == 0xCC && (*(BYTE*)(m_ourFunct + i - 1)) == 0xC3 || (*(BYTE*)(m_ourFunct + i + 1)) == 0xCC && (*(BYTE*)(m_ourFunct + i)) == 0xCC)
        {
            return i; // Returns the number of bytes found
        }

        //Add the byte to the vector
        m_ourFunctBytes.push_back(*(BYTE*)(m_ourFunct + i));
    }
}

DWORD extHook::getAddressHook()
{
    return (DWORD)(&m_ourCave);
}

void extHook::insertFunction()
{
    DWORD oldProtect, Bkup;

    //Grab the bytes of our hook function
    int numOfBytes = grabBytes();
    BYTE* tempBytes = new BYTE[m_ourFunctBytes.size()];

    //Move all of the bytes into an array
    int it = 0;
    for (auto i : m_ourFunctBytes)
    {
        tempBytes[it] = (BYTE)i;
        it++;
    }

    //Allocate memory in the other process to place our hook function in
    BYTE jump = 0xE9;
    LPVOID arg = (LPVOID)VirtualAllocEx(m_handle, NULL, numOfBytes + 0x5, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

    //This is so that the program can run our function, and so that we can write our bytes there.
    //We leave it at PAGE_EXECUTE_READWRITE so the program can run it.
    VirtualProtectEx(m_handle, (LPVOID)arg, numOfBytes + 0x5, PAGE_EXECUTE_READWRITE, &oldProtect);
    //Calculate the jump back to the original function
    DWORD relAddy = (m_toHook - (DWORD)arg) - 25;
    //std::cout << arg << std::endl;
    //Put our function into the allocated memory
    WriteProcessMemory(m_handle, arg, tempBytes, numOfBytes, NULL);
    //At the end of it, add a jump

    //Used in the CreateDetour function
    m_ourCave = (DWORD)arg;
    std::cout << arg << std::endl;
    std::cout << m_ourCave << std::endl;
    //Prevent mem leaks
    delete[] tempBytes;
};
void extHook::insertFunction13(LPVOID& valueofnew, LPVOID& valueofnew2)
{
    DWORD oldProtect, Bkup;

    //Grab the bytes of our hook function
    int numOfBytes = grabBytes();
    BYTE* tempBytes = new BYTE[m_ourFunctBytes.size()];

    //Move all of the bytes into an array
    int it = 0;
    for (auto i : m_ourFunctBytes)
    {
        tempBytes[it] = (BYTE)i;
        it++;
    }

    //Allocate memory in the other process to place our hook function in
    BYTE jump = 0xE9;
    BYTE callFun = 0xE8;
    BYTE pushed = 0x68;
    BYTE move = 0xBA;
    BYTE move2 = 0xFF;
    BYTE move3 = 0xD2;
    LPVOID arg = (LPVOID)VirtualAllocEx(m_handle, NULL, numOfBytes, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

    //This is so that the program can run our function, and so that we can write our bytes there.
    //We leave it at PAGE_EXECUTE_READWRITE so the program can run it.
    VirtualProtectEx(m_handle, (LPVOID)arg, numOfBytes, PAGE_EXECUTE_READWRITE, &oldProtect);
    //Calculate the jump back to the original function
    DWORD relAddy = (m_toHook - (DWORD)arg) - 25;
    std::cout << arg << std::endl;
    //Put our function into the allocated memory
    WriteProcessMemory(m_handle, arg, tempBytes, numOfBytes, NULL);
    WriteProcessMemory(m_handle, (LPVOID)(DWORD)arg, &move, sizeof(move), NULL);
    WriteProcessMemory(m_handle, (LPVOID)((DWORD)arg + 0x01), &valueofnew2, sizeof(valueofnew2), NULL);
    WriteProcessMemory(m_handle, (LPVOID)((DWORD)arg + 187), &move, 1, NULL);
    WriteProcessMemory(m_handle, (LPVOID)((DWORD)arg + 188), &valueofnew, 4, NULL);
    WriteProcessMemory(m_handle, (LPVOID)((DWORD)arg + 192), &move2, 1, NULL);
    WriteProcessMemory(m_handle, (LPVOID)((DWORD)arg + 193), &move3, 1, NULL);
    //At the end of it, add a jump

    //Used in the CreateDetour function
    m_ourCave = (DWORD)arg;
    std::cout << arg << std::endl;
    std::cout << m_ourCave << std::endl;
    //Prevent mem leaks
    delete[] tempBytes;
}
void extHook::insertFunction2(LPVOID& valueofnew)
{
    DWORD oldProtect, Bkup;

    //Grab the bytes of our hook function
    int numOfBytes = grabBytes();
    BYTE* tempBytes = new BYTE[m_ourFunctBytes.size()];

    //Move all of the bytes into an array
    int it = 0;
    for (auto i : m_ourFunctBytes)
    {
        tempBytes[it] = (BYTE)i;
        it++;
    }

    //Allocate memory in the other process to place our hook function in
    BYTE jump = 0xE9;
    BYTE pushed = 0x68;
    BYTE move = 0xB8;
    LPVOID arg = (LPVOID)VirtualAllocEx(m_handle, NULL, numOfBytes, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

    //This is so that the program can run our function, and so that we can write our bytes there.
    //We leave it at PAGE_EXECUTE_READWRITE so the program can run it.
    VirtualProtectEx(m_handle, (LPVOID)arg, numOfBytes, PAGE_EXECUTE_READWRITE, &oldProtect);
    //Calculate the jump back to the original function
    DWORD relAddy = (m_toHook - (DWORD)arg) - 25;
    std::cout << arg << std::endl;
    //Put our function into the allocated memory
    WriteProcessMemory(m_handle, arg, tempBytes, numOfBytes, NULL);
    WriteProcessMemory(m_handle, (LPVOID)(DWORD)arg, &move, sizeof(move), NULL);
    WriteProcessMemory(m_handle, (LPVOID)((DWORD)arg + 0x01), &valueofnew, sizeof(valueofnew), NULL);
    //At the end of it, add a jump

    //Used in the CreateDetour function
    m_ourCave = (DWORD)arg;
    std::cout << arg << std::endl;
    std::cout << m_ourCave << std::endl;
    //Prevent mem leaks
    delete[] tempBytes;
}
void extHook::insertFunction12(LPVOID& valueofnew)
{
    DWORD oldProtect, Bkup;

    //Grab the bytes of our hook function
    int numOfBytes = grabBytes();
    BYTE* tempBytes = new BYTE[m_ourFunctBytes.size()];

    //Move all of the bytes into an array
    int it = 0;
    for (auto i : m_ourFunctBytes)
    {
        tempBytes[it] = (BYTE)i;
        it++;
    }

    //Allocate memory in the other process to place our hook function in
    BYTE jump = 0xE9;
    BYTE pushed = 0x68;
    BYTE move = 0xB8;
    LPVOID arg = (LPVOID)VirtualAllocEx(m_handle, NULL, numOfBytes, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

    //This is so that the program can run our function, and so that we can write our bytes there.
    //We leave it at PAGE_EXECUTE_READWRITE so the program can run it.
    VirtualProtectEx(m_handle, (LPVOID)arg, numOfBytes, PAGE_EXECUTE_READWRITE, &oldProtect);
    //Calculate the jump back to the original function
    DWORD relAddy = (m_toHook - (DWORD)arg) - 25;
    std::cout << arg << std::endl;
    //Put our function into the allocated memory
    WriteProcessMemory(m_handle, arg, tempBytes, numOfBytes, NULL);
    //WriteProcessMemory(m_handle, (LPVOID)(DWORD)arg, &move, sizeof(move), NULL);
    //WriteProcessMemory(m_handle, (LPVOID)((DWORD)arg+0x01), &valueofnew, sizeof(valueofnew), NULL);
    //At the end of it, add a jump

    //Used in the CreateDetour function
    m_ourCave = (DWORD)arg;
    std::cout << arg << std::endl;
    std::cout << m_ourCave << std::endl;
    //Prevent mem leaks
    delete[] tempBytes;
}
void extHook::insertFunction3(LPVOID& valueofnew)
{
    DWORD oldProtect, Bkup;

    //Grab the bytes of our hook function
    int numOfBytes = grabBytes();
    BYTE* tempBytes = new BYTE[m_ourFunctBytes.size()];

    //Move all of the bytes into an array
    int it = 0;
    for (auto i : m_ourFunctBytes)
    {
        tempBytes[it] = (BYTE)i;
        it++;
    }

    //Allocate memory in the other process to place our hook function in
    BYTE jump = 0xE9;
    BYTE pushed = 0x68;
    BYTE move = 0xB9;
    LPVOID arg = (LPVOID)VirtualAllocEx(m_handle, NULL, numOfBytes, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

    //This is so that the program can run our function, and so that we can write our bytes there.
    //We leave it at PAGE_EXECUTE_READWRITE so the program can run it.
    VirtualProtectEx(m_handle, (LPVOID)arg, numOfBytes, PAGE_EXECUTE_READWRITE, &oldProtect);
    //Calculate the jump back to the original function
    DWORD relAddy = (m_toHook - (DWORD)arg) - 25;
    std::cout << arg << std::endl;
    //Put our function into the allocated memory
    WriteProcessMemory(m_handle, arg, tempBytes, numOfBytes, NULL);
    WriteProcessMemory(m_handle, (LPVOID)(DWORD)arg, &move, sizeof(move), NULL);
    WriteProcessMemory(m_handle, (LPVOID)((DWORD)arg + 0x01), &valueofnew, sizeof(valueofnew), NULL);
    //At the end of it, add a jump

    //Used in the CreateDetour function
    m_ourCave = (DWORD)arg;
    std::cout << arg << std::endl;
    std::cout << m_ourCave << std::endl;
    //Prevent mem leaks
    delete[] tempBytes;
}
void extHook::insertFunction4(LPVOID& valueofnew)
{
    DWORD oldProtect, Bkup;

    //Grab the bytes of our hook function
    int numOfBytes = grabBytes();
    BYTE* tempBytes = new BYTE[m_ourFunctBytes.size()];

    //Move all of the bytes into an array
    int it = 0;
    for (auto i : m_ourFunctBytes)
    {
        tempBytes[it] = (BYTE)i;
        it++;
    }

    //Allocate memory in the other process to place our hook function in
    BYTE jump = 0xE9;
    BYTE pushed = 0x68;
    BYTE move = 0xBA;
    LPVOID arg = (LPVOID)VirtualAllocEx(m_handle, NULL, numOfBytes, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

    //This is so that the program can run our function, and so that we can write our bytes there.
    //We leave it at PAGE_EXECUTE_READWRITE so the program can run it.
    VirtualProtectEx(m_handle, (LPVOID)arg, numOfBytes, PAGE_EXECUTE_READWRITE, &oldProtect);
    //Calculate the jump back to the original function
    DWORD relAddy = (m_toHook - (DWORD)arg) - 25;
    std::cout << arg << std::endl;
    //Put our function into the allocated memory
    WriteProcessMemory(m_handle, arg, tempBytes, numOfBytes, NULL);
    WriteProcessMemory(m_handle, (LPVOID)(DWORD)arg, &move, sizeof(move), NULL);
    WriteProcessMemory(m_handle, (LPVOID)((DWORD)arg + 0x01), &valueofnew, sizeof(valueofnew), NULL);
    //At the end of it, add a jump

    //Used in the CreateDetour function
    m_ourCave = (DWORD)arg;
    std::cout << arg << std::endl;
    std::cout << m_ourCave << std::endl;
    //Prevent mem leaks
    delete[] tempBytes;
}

bool extHook::CreateDetour()
{
    DWORD oldProtect, Bkup, relativeAddy;

    //The hook jmp takes 5 bytes
    if (m_len < 5)
    {
        return false;
    }

    //Make sure we can even write to the section of memory
    if (!VirtualProtectEx(m_handle, (LPVOID)m_toHook, m_len, PAGE_EXECUTE_READWRITE, &oldProtect))
    {
        return false;
    }

    //Just some variables needed to apply the hook
    BYTE jump = 0xE9;
    BYTE jmp2 = 0xC3;
    BYTE NOP = 0x90;
    BYTE NOPS[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };

    //Put the original function bytes into m_restoreBytes for when we want to undo the hook
    ReadProcessMemory(m_handle, (LPVOID)(m_toHook), m_restoreBytes, m_len, NULL);

    //Calculate the jump to the hook function
    relativeAddy = (m_ourCave - m_toHook) - 5;
    std::cout << relativeAddy << " / " << m_ourCave << " / " << m_toHook << std::endl;
    //Write the jump
    WriteProcessMemory(m_handle, (LPVOID)m_toHook, &jump, sizeof(jump), NULL);
    WriteProcessMemory(m_handle, (LPVOID)(m_toHook + 0x1), &relativeAddy, 4, NULL);
    //WriteProcessMemory(m_handle, (LPVOID)(m_toHook + 0x5), &jmp2, sizeof(jmp2), NULL);

    //Write all of the nops at once, instead of making multiple calls
    int new_len = m_len - 5;
    if (new_len > 0)
    {
        WriteProcessMemory(m_handle, (LPVOID)(m_toHook + 0x5), NOPS, new_len, NULL);
    }

    //Restore the previous protection
    if (!VirtualProtectEx(m_handle, (LPVOID)m_toHook, m_len, oldProtect, &Bkup))
    {
        return false;
    }

    return true;
}
bool extHook::CreateDetour4(LPVOID& valueofnew, int nextcall)
{
    DWORD oldProtect, Bkup, relativeAddy;

    //The hook jmp takes 5 bytes
    if (m_len < 4)
    {
        return false;
    }

    //Make sure we can even write to the section of memory
    if (!VirtualProtectEx(m_handle, (LPVOID)m_toHook, m_len, PAGE_EXECUTE_READWRITE, &oldProtect))
    {
        return false;
    }

    //Just some variables needed to apply the hook
    BYTE jump = 0xE9;
    BYTE jmp2 = 0xC3;
    BYTE NOP = 0x90;
    BYTE NOPS[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };

    //Put the original function bytes into m_restoreBytes for when we want to undo the hook
    ReadProcessMemory(m_handle, (LPVOID)(m_toHook), m_restoreBytes, m_len, NULL);
    LPVOID newvalue = (int*)valueofnew + nextcall;
    //WriteProcessMemory(m_handle, (LPVOID)m_toHook, &jump, sizeof(jump), NULL);
    WriteProcessMemory(m_handle, (LPVOID)(newvalue), &m_ourCave, 4, NULL);
    //WriteProcessMemory(m_handle, (LPVOID)(m_toHook + 0x5), &jmp2, sizeof(jmp2), NULL);

    //Write all of the nops at once, instead of making multiple calls

    //Restore the previous protection
    if (!VirtualProtectEx(m_handle, (LPVOID)m_toHook, m_len, oldProtect, &Bkup))
    {
        return false;
    }

    return true;
}
bool extHook::CreateDetour2(LPVOID& valueofnew)
{
    DWORD oldProtect, Bkup, relativeAddy;

    //The hook jmp takes 5 bytes
    if (m_len < 5)
    {
        return false;
    }

    //Make sure we can even write to the section of memory
    if (!VirtualProtectEx(m_handle, (LPVOID)m_toHook, m_len, PAGE_EXECUTE_READWRITE, &oldProtect))
    {
        return false;
    }

    //Just some variables needed to apply the hook
    BYTE jump = 0xB9;
    BYTE jmp2 = 0xC3;
    BYTE NOP = 0x90;
    BYTE call = 0xFF;
    BYTE call1 = 0x11;
    BYTE NOPS[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };

    //Put the original function bytes into m_restoreBytes for when we want to undo the hook
    ReadProcessMemory(m_handle, (LPVOID)(m_toHook), m_restoreBytes, m_len, NULL);

    //Calculate the jump to the hook function
    relativeAddy = (m_ourCave - m_toHook) - 5;
    std::cout << relativeAddy << " / " << m_ourCave << " / " << m_toHook << std::endl;
    //Write the jump
    WriteProcessMemory(m_handle, (LPVOID)m_toHook, &jump, sizeof(jump), NULL);
    WriteProcessMemory(m_handle, (LPVOID)(m_toHook + 0x1), &valueofnew, 4, NULL);
    WriteProcessMemory(m_handle, (LPVOID)(m_toHook + 0x5), &call, sizeof(call), NULL);
    WriteProcessMemory(m_handle, (LPVOID)(m_toHook + 0x6), &call1, sizeof(call1), NULL);
    //Write all of the nops at once, instead of making multiple calls
    int new_len = m_len - 7;
    if (new_len > 0)
    {
        WriteProcessMemory(m_handle, (LPVOID)(m_toHook + 0x7), NOPS, new_len, NULL);
    }

    //Restore the previous protection
    if (!VirtualProtectEx(m_handle, (LPVOID)m_toHook, m_len, oldProtect, &Bkup))
    {
        return false;
    }

    return true;
}
bool extHook::CreateDetourcall()
{
    DWORD oldProtect, Bkup, relativeAddy;

    //The hook jmp takes 5 bytes
    if (m_len < 5)
    {
        return false;
    }

    //Make sure we can even write to the section of memory
    if (!VirtualProtectEx(m_handle, (LPVOID)m_toHook, m_len, PAGE_EXECUTE_READWRITE, &oldProtect))
    {
        return false;
    }

    //Just some variables needed to apply the hook
    BYTE jump = 0xE9;
    BYTE call2 = 0xE9;
    BYTE jmp2 = 0xC3;
    BYTE NOP = 0x90;
    BYTE NOPS[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };

    //Put the original function bytes into m_restoreBytes for when we want to undo the hook
    ReadProcessMemory(m_handle, (LPVOID)(m_toHook), m_restoreBytes, m_len, NULL);

    //Calculate the jump to the hook function
    relativeAddy = (m_ourCave - m_toHook) - 5;
    std::cout << relativeAddy << " / " << m_ourCave << " / " << m_toHook << std::endl;
    //Write the jump
    WriteProcessMemory(m_handle, (LPVOID)m_toHook, &call2, sizeof(call2), NULL);
    WriteProcessMemory(m_handle, (LPVOID)(m_toHook + 0x1), &relativeAddy, 4, NULL);
    //WriteProcessMemory(m_handle, (LPVOID)(m_toHook + 0x5), &jmp2, sizeof(jmp2), NULL);

    //Write all of the nops at once, instead of making multiple calls
    int new_len = m_len - 5;
    if (new_len > 0)
    {
        WriteProcessMemory(m_handle, (LPVOID)(m_toHook + 0x5), NOPS, new_len, NULL);
    }

    //Restore the previous protection
    if (!VirtualProtectEx(m_handle, (LPVOID)m_toHook, m_len, oldProtect, &Bkup))
    {
        return false;
    }

    return true;
}

bool extHook::Restore()
{
    DWORD oldProtect, Bkup;

    //So we can restore the bytes
    if (!VirtualProtectEx(m_handle, (LPVOID)m_toHook, m_len, PAGE_EXECUTE_READWRITE, &oldProtect))
    {
        return false;
    }

    //Write the original bytes back into the function
    WriteProcessMemory(m_handle, (LPVOID)m_toHook, m_restoreBytes, m_len, NULL);

    if (!VirtualProtectEx(m_handle, (LPVOID)m_toHook, m_len, oldProtect, &Bkup))
    {
        return false;
    }

    //Free the memory we allocated for our hook
    VirtualFreeEx(m_handle, (LPVOID)m_ourCave, 0, MEM_RELEASE);

    return true;
}
void extHook::insertCode(LPVOID valueofnew)
{

}

//This ones pretty self explanatory.
void extHook::ToggleHook4(LPVOID valueofnew)
{
    insertFunction();
    // CreateDetour2(valueofnew);
}

void extHook::ToggleHook()
{
    if (m_isHooked == false)
    {
        insertFunction();
        CreateDetour();
        m_isHooked = true;
    }

    else if (m_isHooked == true)
    {
        Restore();
        m_isHooked = false;
    }
}
void extHook::ToggleHook3()
{
    if (m_isHooked == false)
    {
        insertFunction();
        CreateDetourcall();
        m_isHooked = true;
    }

    else if (m_isHooked == true)
    {
        Restore();
        m_isHooked = false;
    }
}
void extHook::ToggleHook5(LPVOID& valueofnew)
{
    if (m_isHooked == false)
    {
        insertFunction3(valueofnew);
        CreateDetourcall();
        m_isHooked = true;
    }

    else if (m_isHooked == true)
    {
        Restore();
        m_isHooked = false;
    }
}
void extHook::ToggleHook6(LPVOID& valueofnew)
{
    if (m_isHooked == false)
    {
        insertFunction4(valueofnew);
        CreateDetourcall();
        m_isHooked = true;
    }

    else if (m_isHooked == true)
    {
        Restore();
        m_isHooked = false;
    }
}
void extHook::ToggleHook2(LPVOID& valueofnew)
{
    if (m_isHooked == false)
    {
        insertFunction2(valueofnew);
        CreateDetour();
        m_isHooked = true;
    }

    else if (m_isHooked == true)
    {
        Restore();
        m_isHooked = false;
    }
}
void extHook::ToggleHook13(LPVOID& valueofnew, LPVOID& valueofnew2)
{
    if (m_isHooked == false)
    {
        insertFunction13(valueofnew, valueofnew2);
        CreateDetour();
        m_isHooked = true;
    }

    else if (m_isHooked == true)
    {
        Restore();
        m_isHooked = false;
    }
}
void extHook::ToggleHook12(LPVOID& valueofnew, int nextcall)
{
    if (m_isHooked == false)
    {
        insertFunction12(valueofnew);
        CreateDetour4(valueofnew, nextcall);
        m_isHooked = true;
    }

    else if (m_isHooked == true)
    {
        Restore();
        m_isHooked = false;
    }
}

