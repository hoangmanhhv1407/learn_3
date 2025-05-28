//---------------------------------------------------------------------------

#ifndef exthookH
#define exthookH
//---------------------------------------------------------------------------
#endif
#include <Windows.h>
#include <vector>

using std::vector;

class extHook
{
protected:
    virtual void insertFunction();
    virtual void insertFunction2(LPVOID& valueofnew);
    virtual void insertFunction3(LPVOID& valueofnew);
    virtual void insertFunction4(LPVOID& valueofnew);
    virtual void insertFunction12(LPVOID& valueofnew);
    virtual void insertFunction13(LPVOID& valueofnew, LPVOID& valueofnew2);
    virtual int grabBytes();
    virtual bool Restore();
    virtual bool CreateDetour();
    virtual bool CreateDetour4(LPVOID& valueofnew, int nextcall);
    virtual bool CreateDetour2(LPVOID& valueofnew);
    virtual bool CreateDetourcall();
    vector<BYTE> m_ourFunctBytes;
    DWORD m_toHook;
    DWORD m_ourFunct;
    DWORD m_ourCave;
    BYTE* m_restoreBytes;
    DWORD m_len;
    HANDLE m_handle;
    bool m_isHooked;
    DWORD AddressHook;

public:
    DWORD getAddressHook();
    virtual void insertCode(LPVOID valueofnew);
    extHook(HANDLE handle, DWORD ourFunction, DWORD toHook, int len) :
        m_handle(handle),
        m_ourFunct(ourFunction),
        m_toHook(toHook),
        m_len(len),
        m_isHooked(false),
        m_restoreBytes(new BYTE[500])
    { }
    ~extHook() { delete[] m_restoreBytes; }
    virtual void ToggleHook3();
    virtual void ToggleHook4(LPVOID valueofnew);
    virtual void ToggleHook5(LPVOID& valueofnew);
    virtual void ToggleHook6(LPVOID& valueofnew);
    virtual void ToggleHook();
    virtual void ToggleHook2(LPVOID& valueofnew);
    virtual void ToggleHook13(LPVOID& valueofnew, LPVOID& valueofnew2);
    virtual void ToggleHook12(LPVOID& valueofnew, int nextcall);
};