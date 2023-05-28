#pragma once
#include <WinSock2.h>
#include <windows.h>
#include <chrono>
#include <iostream>
#include <fstream>
#include <iphlpapi.h>
#include <intrin.h>
#include <array>
#include <concurrent_vector.h>
#pragma comment(lib, "iphlpapi.lib")
#include <ppl.h>

enum VMStatus {
    inVM,
    notVM
};

class VMDetector {
public:
    VMDetector() : expected_time(calibrate()) { }

    void perform_operation() {
        for (long long j = 0; j < 1e9; ++j) {}
    }

    double calibrate() {
        const int iterations = 10;
        double total_time = 0.0;

        for (int i = 0; i < iterations; ++i) {
            auto start = std::chrono::high_resolution_clock::now();
            perform_operation();
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> diff = end - start;

            total_time += diff.count();
        }

        return total_time / iterations;
    }

    VMStatus virtual_machine() {
        auto start = std::chrono::high_resolution_clock::now();
        perform_operation();
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = end - start;

        return (diff.count() > expected_time) ? VMStatus::inVM : VMStatus::notVM;
    }

    bool is_registry_key_present(const char* key, const char* subkey = nullptr) {
        HKEY hKey;
        LONG lRes = RegOpenKeyExA(HKEY_LOCAL_MACHINE, key, 0, KEY_READ, &hKey);
        bool foundAndSuccess(lRes == ERROR_SUCCESS);
        bool couldNotOpenSubKey(false);

        if (foundAndSuccess) {
            if (subkey) {
                lRes = RegOpenKeyExA(hKey, subkey, 0, KEY_READ, &hKey);

                if (lRes != ERROR_SUCCESS) {
                    foundAndSuccess = false;
                    couldNotOpenSubKey = true;
                }
            }

            if (!couldNotOpenSubKey) {
                RegCloseKey(hKey);
            }
        }

        return foundAndSuccess;
    }

    bool is_vm_by_registry() {
        const char* vm_keys[] = {
            "SYSTEM\\CurrentControlSet\\Enum\\PCI\\VEN_15AD&DEV_0405&SUBSYS_040515AD&REV_00",
            "SYSTEM\\CurrentControlSet\\Enum\\PCI\\VEN_80EE&DEV_BEEF&SUBSYS_00000000&REV_00",
            "SOFTWARE\\VMware, Inc.\\VMware Tools",
            "SOFTWARE\\Oracle\\VirtualBox Guest Additions",
        };
        for (const char* key : vm_keys) {
            DectectionFiles.push_back(key);
        }
        for (const char* key : vm_keys) {
            if (is_registry_key_present(key)) {
                VmKeysFound.push_back(key); 
                return true;
            }
        }

        return false;
    }

    bool is_virtual_processor() {
        char HyperVendorID[13];
        int CPUInfo[4] = { -1 };
        unsigned nExIds, i = 0;
        __cpuid(CPUInfo, 0);
        nExIds = CPUInfo[0];
        for (i = 0; i <= nExIds; ++i)
        {
            __cpuid(CPUInfo, i);
            if (i == 1) {
                return (CPUInfo[2] >> 31) & 1;
            }
        }
        return false;
    }

    bool is_vm_by_files() {
        const char* vm_files[] = {
            "C:\\windows\\system32\\drivers\\VBoxMouse.sys",
            "C:\\windows\\system32\\drivers\\VBoxGuest.sys",
            "C:\\windows\\system32\\drivers\\VBoxSF.sys",
            "C:\\windows\\system32\\drivers\\VBoxVideo.sys",
            "C:\\windows\\system32\\vboxdisp.dll",
            "C:\\windows\\system32\\vboxhook.dll",
            "C:\\windows\\system32\\vboxmrxnp.dll",
            "C:\\windows\\system32\\vboxogl.dll",
            "C:\\windows\\system32\\vboxoglarrayspu.dll",
            "C:\\windows\\system32\\vboxoglcrutil.dll",
            "C:\\windows\\system32\\vboxoglfeedbackspu.dll",
            "C:\\windows\\system32\\vboxoglpackspu.dll",
            "C:\\windows\\system32\\vboxoglpassthroughspu.dll",
            "C:\\windows\\system32\\vboxservice.exe",
            "C:\\windows\\system32\\vboxtray.exe",
            "C:\\windows\\system32\\VBoxControl.exe",
        };//need to add other operating system file paths for unix.
        for (const char* file : vm_files) {
            DectectionDlls.push_back(file);
        }

        for (const char* file : vm_files) {
            std::ifstream ifile(file);
            if (ifile.is_open()) {
            VmFilesFound.push_back(file);
                return true;
            }
        }

        return false;
    }

    bool is_under_debug() {
        return IsDebuggerPresent();
    }

    VMStatus is_in_vm() {
        bool inVM = is_virtual_processor() || is_vm_by_registry() || is_vm_by_files() || is_under_debug();
        return inVM ? VMStatus::inVM : VMStatus::notVM;
    }
    
public:
    std::vector<std::string> VmFilesFound;
    std::vector<std::string> VmDllsFound;
    std::vector<std::string> VmKeysFound;
 protected: 
     concurrency::concurrent_vector<const char*> DectectionDlls;
     concurrency::concurrent_vector<const char*> DectectionFiles;
private:
    double expected_time;
};
