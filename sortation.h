#include "Fuzzer.h"
#include <concurrent_vector.h>
#include <concurrent_unordered_map.h>
#include <unordered_set>
#include <shared_mutex>
#include "gumbo.h"

extern std::shared_mutex responseMutex;

struct datasortType {
    datasortType() {
        std::shared_lock<std::shared_mutex> lock(responseMutex);
        for (const auto& pair : urlResponses) {
            URL.push_back(pair.first);
            UrlResponses_requests[pair.first] = pair.second;
        }
        urlResponses.clear();

        for (const auto& url : URL) {
            processResponse(url, UrlResponses_requests[url]);
        }
    }

    void parse_html(const std::string& html, std::unordered_set<std::string>& tags) {
        GumboOutput* output = gumbo_parse(html.c_str());

        traverse(output->root, tags);

        gumbo_destroy_output(&kGumboDefaultOptions, output);
    }

    void traverse(GumboNode* node, std::unordered_set<std::string>& tags) {
        if (node->type != GUMBO_NODE_ELEMENT) {
            return;
        }

        tags.insert(gumbo_normalized_tagname(node->v.element.tag));

        GumboVector* children = &node->v.element.children;
        for (unsigned int i = 0; i < children->length; ++i) {
            traverse(static_cast<GumboNode*>(children->data[i]), tags);
        }
    }

    void processResponse(const std::string& url, const std::string& response) {
        std::thread t([this, url, response]() {
            std::unordered_set<std::string> tags;
            parse_html(response, tags);

            std::lock_guard<std::shared_mutex> lock(responseMutex);
            tagsInResponses[url] = tags;
            });

        t.detach();
    }

public:
    Concurrency::concurrent_vector<std::string> URL;
    Concurrency::concurrent_unordered_map<std::string, std::string> UrlResponses_requests;
    Concurrency::concurrent_unordered_map<std::string, std::unordered_set<std::string>> tagsInResponses;
};

class DataSorter {
public:
    DataSorter() : dataSorter() {}

    void printData() const {
        for (const auto& url : dataSorter.URL) {
            std::cout << "URL: " << url << std::endl;

            auto responseIter = dataSorter.UrlResponses_requests.find(url);
            if (responseIter != dataSorter.UrlResponses_requests.end()) {
                std::cout << "Response: " << responseIter->second << std::endl;
            }

            auto tagIter = dataSorter.tagsInResponses.find(url);
            if (tagIter != dataSorter.tagsInResponses.end()) {
                std::cout << "Tags: ";
                for (const auto& tag : tagIter->second) {
                    std::cout << tag << ' ';
                }
            }
            std::cout << std::endl;
        }
    }

private:
    datasortType dataSorter;
};
