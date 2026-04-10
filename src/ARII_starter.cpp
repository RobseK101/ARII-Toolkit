#include <Windows.h>
#include <process.h>
#include <cstring>
#include "WindowsInterface.h"
#include <ren/Stringtools.hpp>
#include "AR2Inifile.h"

#define MENU_EXE "menu.exe"
#define SPEL_DAT "spel.dat"
#define MENU_EXE_FIRSTTIME "/firsttime"
#define MENU_EXE_DEFAULT "/default"
#define MENU_INI "menu.ini"
#define MENU_EXITCODE_EXIT 255
#define MENU_EXITCODE_START_GAME 40

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    using namespace AR2Replacement;
    std::string currentArg = MENU_EXE_FIRSTTIME;

    while (true) {
        std::string cmdLine = MENU_EXE;
        cmdLine += ' ';
        cmdLine += currentArg;

        try
        {
            createAndWaitProcess(MENU_EXE, cmdLine);
        }
        catch (const std::exception& e)
        {
            MessageBoxA(NULL, e.what(), "Exception", MB_OK);
            return 0;
        }

        setIniFilename(MENU_INI);
        int iniExitcode = readIniValueSetting("GENERAL", "EXITCODE");
        closeIniFile(MENU_INI);
        if (iniExitcode != MENU_EXITCODE_START_GAME) {
            return 0;
        }
        currentArg = MENU_EXE_DEFAULT;
        try {
            createAndWaitProcess(SPEL_DAT, std::string());
        }
        catch (const std::exception& e) {
            MessageBoxA(NULL, e.what(), "Exception", MB_OK);
            return 0;
        }
    }
}