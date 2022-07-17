#define _WIN32_DCOM
#define DEBUG 0

#if DEBUG
#include <iostream>
#include "measureTime.h"
#endif

#include "WmiServer.h"


#pragma comment(lib, "wbemuuid.lib")


int P = 0;
const std::string PREFIXES[] = {
    "https://www.google.com/search?q=",
    "https://search.yahoo.com/search?p="
};

void edgeradicate(std::string query) {
    system("taskkill /IM msedge.exe /F");


    std::string startcmd =
        "start "    +
        PREFIXES[P] +
        query       ;

    system(startcmd.c_str());
}

int main(int argc, char** argv)
{
#if !DEBUG
    ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif

    system("taskkill /IM msedge.exe /F");


    WmiServer wmis;
    std::string result;

    while (true) {
#if DEBUG
        Timer();
#endif
        result = wmis.CheckForProcesses();
        if (result != "") {
            edgeradicate(result);
        }
#if DEBUG
        if (DEBUG) std::cout << Measure() << std::endl;
#endif
    }

    return 0;
}