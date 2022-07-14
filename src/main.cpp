#define _WIN32_DCOM

#include <iostream>
#include <comdef.h>
#include <Wbemidl.h>

#include "WmiServer.h"
#include "measureTime.h"

#pragma comment(lib, "wbemuuid.lib")

int P = 0;
const std::string PREFIXES[] = {
    "https://www.google.com/search?q=",
    "https://search.yahoo.com/search?p="
};

void edgeradicate(std::string query) {
    system("taskkill /IM msedge.exe /F");


    std::string startcmd =
        "start " +
        PREFIXES[P] +
        query;
    cout << startcmd.c_str() << endl;
    system(startcmd.c_str());
}


int main(int argc, char** argv)
{
    URLDecodeInit();

    system("taskkill /IM msedge.exe /F");


    WmiServer abc;
    cout << hex << &abc << endl;

    while (true) {
        Timer();
        std::string result = abc.CheckForProcesses();
        cout << "<" << result << ">" << endl;
        if (result != "") {
            edgeradicate(result);
        }
        cout << Measure() << endl;
    }

    return 0;
}