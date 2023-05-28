#ifndef FUZZER_H
#define FUZZER_H

#include <queue>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>
#include <thread>
#include <functional>
#include <iostream>
#include <atomic>
#include <vector>
#include <string>
#include <algorithm>
#include <curl/curl.h>
#include <unordered_map>

extern std::unordered_map<std::string, std::string> urlResponses;
extern std::shared_mutex responseMutex;

struct Task {
    std::function<void()> func;
    int priority;

    Task(std::function<void()> func = {}, int priority = 0) : func(func), priority(priority) {}

    bool operator<(const Task& j) const {
        return priority < j.priority;
    }
};

class ThreadPool {
public:
    ThreadPool(int numThreads) {
        for (int i = 0; i < numThreads; ++i) {
            threads_.emplace_back(&ThreadPool::WorkerThread, this);
        }
    }

    ~ThreadPool() {
        {
            std::lock_guard<std::mutex> lock(queueMutex_);
            stop_ = true;
        }
        condVar_.notify_all();
        for (auto& thread : threads_) {
            thread.join();
        }
    }

    void EnqueueTask(Task task) {
        {
            std::lock_guard<std::mutex> lock(queueMutex_);
            taskQueue_.push(task);
        }
        condVar_.notify_one();
    }
private:
    void WorkerThread() {
        while (true) {
            Task task;
            {
                std::unique_lock<std::mutex> lock(queueMutex_);
                condVar_.wait(lock, [this]() { return stop_ || !taskQueue_.empty(); });

                if (stop_ && taskQueue_.empty()) {
                    return;
                }

                task = std::move(taskQueue_.top());
                taskQueue_.pop();
            }
            task.func();
        }
    }
    std::vector<std::thread> threads_;
    std::priority_queue<Task> taskQueue_;
    std::mutex queueMutex_;
    std::condition_variable condVar_;
    bool stop_ = false;
};
bool isValidUrl(const std::string& url) {return (url.find("http://") == 0 || url.find("https://") == 0);}

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    size_t totalSize = size * nmemb;
    userp->append((char*)contents, totalSize);
    return totalSize;
}
void StartFuzzer() {
    ThreadPool pool(10);
    std::vector<std::string> urls;

    pool.EnqueueTask({ [&]() {
        std::string userInput;
        std::cout << "Do you wish to start the fuzzer? (yes/no)\n";
        std::cin >> userInput  ;

        if (userInput == "yes" || userInput == "Yes") {
            std::cout << "Starting fuzzer...\n";
            std::cout << "Enter URL:\n";
            std::cin >> userInput;

            if (!isValidUrl(userInput)) {
                std::cerr << "Invalid URL!" << std::endl;
                return;
            }
            std::string url = userInput;
            urls.push_back(userInput);

            pool.EnqueueTask({ [url]() {
               CURL* curl = curl_easy_init();
               if (curl) {
                   CURLcode res;
                   std::string response;
                   curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
                   curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
                   curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
                   curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);
                   curl_easy_setopt(curl, CURLOPT_TCP_KEEPIDLE, 120L);
                   curl_easy_setopt(curl, CURLOPT_TCP_KEEPINTVL, 60L);
                   res = curl_easy_perform(curl);
                   if (res != CURLE_OK) {
                       std::cerr << "Couldn't connect, error occurred: " << curl_easy_strerror(res) << "\n";
                       curl_easy_cleanup(curl);
                       return;
                   }
                   responseMutex.lock();
                   urlResponses[url] = response;
                   responseMutex.unlock();
                   curl_easy_cleanup(curl);
               }
           }, 1 });
        }
    }, 1 });
}

#endif
