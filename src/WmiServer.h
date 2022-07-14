#define _WIN32_DCOM

#include <iostream>
#include <comdef.h>
#include <Wbemidl.h>
#include <iostream>

#include "URLDecode.h"

#pragma once
#pragma comment(lib, "wbemuuid.lib")

#define WQLQUERY L"SELECT * FROM Win32_Process WHERE name='msedge.exe'"
#define CMDLINEARGNEEDLE "Microsoft.Windows.Search"

using std::cout;
using std::hex;
using std::endl;
using std::wcout;


class WmiServer {
private:
    HRESULT hres;
    IWbemLocator* pLoc = NULL;
    IWbemServices* pSvc = NULL;

    static std::string ParseCmdLine(std::string arg) {
        int urlstartno = arg.find("url=");
        if (urlstartno == std::string::npos)
            return "";
        std::string urlstart = arg.substr(arg.find("url="));
        int querystart = urlstart.find("q%3D");
        int urlend = urlstart.find("%26");

        return URLDecode(urlstart.substr(querystart + 4, urlend - querystart - 4));
    }
    std::string __CheckForProcessesUnparsed() {
        IEnumWbemClassObject* pEnumerator = NULL;
        std::string out = "";
        hres = pSvc->ExecQuery(
            BSTR(L"WQL"),
            BSTR(WQLQUERY),
            WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
            NULL,
            &pEnumerator
        );


        IWbemClassObject* pclsObj = NULL;
        ULONG uReturn = 0;
        UINT32 LockStatus = 0;

        while (pEnumerator) {
            hres = pEnumerator->Next(
                WBEM_INFINITE,
                1,
                &pclsObj,
                &uReturn
            );

            if (0 == uReturn || FAILED(hres))
                break; //check for errors

            VARIANT vtProp;
            hres = pclsObj->Get(L"CommandLine", 0, &vtProp, 0, 0);// String
            if (!FAILED(hres) && vtProp.vt != VT_NULL && vtProp.vt != VT_EMPTY) {
                //wcout << "CommandLine : " << vtProp.bstrVal << endl;
                std::wstring wide = vtProp.bstrVal;
                std::string str(wide.begin(), wide.end());

                if (str.find(CMDLINEARGNEEDLE) != std::string::npos) {
                    out.assign(str);
                }
            }
            VariantClear(&vtProp);

            pclsObj->Release();
        }

        cout << endl << endl;

        pEnumerator->Reset();
        pEnumerator->Release();

        return out;
    }

public:
    WmiServer() {
        cout << "constructing" << std::endl;

        // Code from https://docs.microsoft.com/en-us/windows/win32/wmisdk/wmi-c---application-examples

        // Step 1: --------------------------------------------------
        // Initialize COM. ------------------------------------------

        this->hres = CoInitializeEx(0, COINIT_MULTITHREADED);
        if (FAILED(this->hres))
        {
            cout << "Failed to initialize COM library. Error code = 0x"
                << hex << this->hres << endl;
            return;                  // Program has failed.
        }

        // Step 2: --------------------------------------------------
        // Set general COM security levels --------------------------

        this->hres = CoInitializeSecurity(
            NULL,
            -1,                          // COM negotiates service
            NULL,                        // Authentication services
            NULL,                        // Reserved
            RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
            RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation
            NULL,                        // Authentication info
            EOAC_NONE,                   // Additional capabilities 
            NULL                         // Reserved
        );


        if (FAILED(this->hres))
        {
            cout << "Failed to initialize security. Error code = 0x"
                << hex << this->hres << endl;
            CoUninitialize();
            return;                      // Program has failed.
        }

        // Step 3: ---------------------------------------------------
        // Obtain the initial locator to WMI -------------------------


        this->hres = CoCreateInstance(
            CLSID_WbemLocator,
            0,
            CLSCTX_INPROC_SERVER,
            IID_IWbemLocator,
            (LPVOID*)&this->pLoc);

        if (FAILED(this->hres))
        {
            cout << "Failed to create IWbemLocator object. "
                << "Err code = 0x"
                << hex << this->hres << endl;
            CoUninitialize();
            return;                 // Program has failed.
        }

        // Step 4: ---------------------------------------------------
        // Connect to WMI through the IWbemLocator::ConnectServer method


        // Connect to the local root\cimv2 namespace
        // and obtain pointer this->pSvc to make IWbemServices calls.
        this->hres = this->pLoc->ConnectServer(
            _bstr_t(L"ROOT\\CIMV2"),
            NULL,
            NULL,
            0,
            NULL,
            0,
            0,
            &this->pSvc
        );

        if (FAILED(this->hres))
        {
            cout << "Could not connect. Error code = 0x"
                << hex << this->hres << endl;
            this->pLoc->Release();
            CoUninitialize();
            return;                // Program has failed.
        }

        cout << "Connected to ROOT\\CIMV2 WMI namespace" << endl;


        // Step 5: --------------------------------------------------
        // Set security levels for the proxy ------------------------

        this->hres = CoSetProxyBlanket(
            this->pSvc,                        // Indicates the proxy to set
            RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx 
            RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx 
            NULL,                        // Server principal name 
            RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
            RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
            NULL,                        // client identity
            EOAC_NONE                    // proxy capabilities 
        );

        if (FAILED(this->hres))
        {
            cout << "Could not set proxy blanket. Error code = 0x"
                << hex << this->hres << endl;
            this->pSvc->Release();
            this->pLoc->Release();
            CoUninitialize();
            return;               // Program has failed.
        }
    }
    ~WmiServer() {
        cout << "deconstructing" << std::endl;


        this->pLoc->Release();
        this->pSvc->Release();
        CoUninitialize();
    }

    std::string CheckForProcesses() {
        return this->ParseCmdLine(this->__CheckForProcessesUnparsed());
    }
};