#include "Fuzzer.h"
#include <concurrent_vector.h>
#include <concurrent_unordered_map.h>
#include <unordered_set>
#include "pugixml.hpp"

extern std::mutex responseMutex;

struct datasortType {
    datasortType() {
        std::lock_guard<std::mutex> lock(responseMutex);
        for (const auto& pair : urlResponses) {
            URL.push_back(pair.first);
            UrlResponses_requests[pair.first] = pair.second;
        }
        urlResponses.clear();

        for (const auto& url : URL) {
            processResponse(url, UrlResponses_requests[url]);
        }
    }

    void processResponse(const std::string& url, const std::string& response) {
        std::thread t([this, url, response]() {
            pugi::xml_document doc;
            pugi::xml_parse_result result = doc.load_string(response.c_str());

            if (result) {
                std::unordered_set<std::string> tags;

                for (pugi::xml_node node = doc.first_child(); node; node = node.next_sibling()) {
                    tags.insert(node.name());
                }

                std::lock_guard<std::mutex> lock(responseMutex);
                tagsInResponses[url] = tags;
            }
            else {
                std::cout << "Error parsing response from " << url << std::endl;
            }
            });

        t.detach();
    }

public:
    Concurrency::concurrent_vector<std::string> URL;
    Concurrency::concurrent_unordered_map<std::string, std::string> UrlResponses_requests;
    Concurrency::concurrent_unordered_map<std::string, std::unordered_set<std::string>> tagsInResponses;

private:

    //will soon add the privates. 
};

class DataSorter {
public:
    DataSorter() : dataSorter() {}

    void printData() const {
        for (const auto& url : dataSorter.URL) {
            std::cout << "URL: " << url << std::endl;
            std::cout << "Response: " << dataSorter.UrlResponses_requests.at(url) << std::endl;
            std::cout << "Tags: ";
            for (const auto& tag : dataSorter.tagsInResponses.at(url)) {
                std::cout << tag << ' ';
            }
            std::cout << std::endl;
        }
    }

private:
    datasortType dataSorter;
};

