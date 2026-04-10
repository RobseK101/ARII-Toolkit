#include "spel.h"
#include <filesystem>
#include <string>
#include <cstring>

namespace Session
{
    void init()
    {
        namespace fs = std::filesystem;
        std::string filename = fs::path(moduleFilepath).filename().string();
        strncpy(moduleFilename, filename.c_str(), sizeof(moduleFilename));
    }

    intptr_t imageBaseAddress = 0x00400000;
    char moduleFilepath[512] = {};
    char moduleFilename[256] = {};
}