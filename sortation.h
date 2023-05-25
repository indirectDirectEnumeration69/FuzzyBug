#pragma once
#include "Fuzzer.h"
#include <concurrent_vector.h>;
#include <concurrent_unordered_map.h>


int count = 0;
struct clockCount {
	clockCount() {
		count++;
	}
	~clockCount() {
    
    
    };
};


struct datasortType {
    char characters = 'a' - 'Z';
    std::string wordsfromChars;
    int Numbers = 0 - 9;

    datasortType() {
        std::lock_guard<std::mutex> lock(responseMutex);
        for (const auto& pair : urlResponses) {
            url = pair.first;
            response = pair.second;
            std::cout << "This is the response stored: " + response + "\n" << std::endl;
            URL.push_back(url);
            UrlResponses_requests[url] = response;//stores the key into the headers local map
            Response.push_back(response);
        }
       urlResponses.clear();//urlresponse is cleared as its better to store in one place at a time.


    }
    ~datasortType() {}

    private: 
        clockCount cc;
        std::string url;
        std::string response;
        Concurrency::concurrent_vector<std::string> URL;
        Concurrency::concurrent_vector<std::string> Response;
        Concurrency::concurrent_unordered_map<std::string, std::string> UrlResponses_requests;//change for ordered storage of url+responses.
};


