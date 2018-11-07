#include <windows.h>
unsigned int base;

//I believe coord1 is zone y and coord2 is zone x, but it really doesn't matter.
signed __int64 __cdecl GetGUID(int coord1, int coord2, int offset){

    /*
    This is exactly the same as the original code, except for one change.
    When the coordinate is (0,0), the original code can generate 0 as a GUID, which is invalid,
    or it can generate 1 as a GUID, which overlaps with the players' GUID(s).
    The change is to offset any GUID by 0x10000000000.
    This way, even at the origin of the world, no entity will generate with a bad GUID.

    For clarity, the original code from Cube World is as follows:
    return ((coord1 + ((signed __int64)coord2 << 16)) << 8) + offset;
    */

    return 0x10000000000L + ((coord1 + ((signed __int64)coord2 << 16)) << 8) + offset;
}

void WriteJMP(BYTE* location, BYTE* newFunction){
	DWORD dwOldProtection;
	VirtualProtect(location, 5, PAGE_EXECUTE_READWRITE, &dwOldProtection);
    location[0] = 0xE9; //jmp
    *((DWORD*)(location + 1)) = (DWORD)(( (unsigned INT32)newFunction - (unsigned INT32)location ) - 5);
	VirtualProtect(location, 5, dwOldProtection, &dwOldProtection);
}

extern "C" __declspec(dllexport) bool APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {

        case DLL_PROCESS_ATTACH:
            base = (unsigned int)GetModuleHandle(NULL);

            //Inject into Cube World's GetGUID function and replace it with our own.
            WriteJMP((BYTE*)(base + 0xF3850), (BYTE*)&GetGUID);
            break;

    }
    return true;
}
