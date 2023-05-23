#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <functional>
#include <iostream>
#include <atomic>
#include <vector>
#include <string>
#include <algorithm>
#include <curl/curl.h>

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

std::string Normalize(const std::string& userInput) {
    std::string normalizedInput(userInput.size(), '\0');
    std::transform(userInput.begin(), userInput.end(), normalizedInput.begin(), ::tolower);
    return normalizedInput;
}

void StartFuzzer() {
    ThreadPool pool(1);
    std::vector<std::string> urls;
    std::mutex urlMutex;

    pool.EnqueueTask({ [&]() {
        std::string userInput;
        std::cout << "Do you wish to start the fuzzer? (yes/no)\n";
        std::cin >> userInput;
        userInput = Normalize(userInput);

        if (userInput == "yes") {
            std::cout << "Starting fuzzer...\n";

            for (int i = 0; i < 3; i++) {
                pool.EnqueueTask({ [&]() {
                  
                }, 1});
            }

            std::cout << "Enter URL:\n";
            std::cin >> userInput;
            userInput = Normalize(userInput);
            {
                std::lock_guard<std::mutex> lock(urlMutex);
                urls.push_back(userInput);
            }

            pool.EnqueueTask({ [&]() {
                CURL* curl = curl_easy_init();
                if (curl) {
                    CURLcode res;
                    curl_easy_setopt(curl, CURLOPT_URL, userInput.c_str());
                    res = curl_easy_perform(curl);
                    if (res != CURLE_OK)
                        std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
                    curl_easy_cleanup(curl);
                }
            }, 1});
        }
    }, 1 });
}
