#include <iostream>
#include <shared_mutex>
#include <unordered_map>
#include "Fuzzer.h"
#include "sortation.h"
#include "Awareness.h"

std::unordered_map<std::string, std::string> urlResponses;
std::shared_mutex responseMutex;
int main()
{
    VMDetector vmDetector; 
    VMStatus vmStatus = vmDetector.is_in_vm(); 
    switch (vmStatus) {


        case VMStatus::inVM:
			std::cout << "VM HAS BEEN DETECTED EXITING" << std::endl;
            exit(0);
            //will add log files for investigation here , break out of vm logic? 
			break;

            case VMStatus::notVM:
                std::cout << "VM HAS NOT BEEN DETECTED" << std::endl;
                StartFuzzer(); 
                DataSorter sorter; 
                sorter.printData(); 
    }
}
