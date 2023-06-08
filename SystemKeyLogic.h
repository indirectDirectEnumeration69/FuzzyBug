#pragma once
#include "SystemEncryption.h"
#include <chrono>

long long Keys::generateRandomNumber()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<long long> distr(1, 999199299923LL);

    return distr(gen);
}

void Keys::setKey(int key)
{
    keys[key] = generateRandomNumber();
    keyTimestamps[key] = std::chrono::system_clock::now();
}

long long Keys::getKey(int key) const
{
    if (keys.find(key) != keys.end())
    {
        return keys.at(key);
    }
    else
    {
        throw std::out_of_range("Key not found");
    }
}

bool Keys::isKeySet(int key) const
{
    return keys.count(key) > 0;
}

bool Keys::isKeyExpired(int key) const
{
    if (keys.find(key) != keys.end())
    {
        auto now = std::chrono::system_clock::now();
        auto keyTime = keyTimestamps.at(key);
        auto duration = std::chrono::duration_cast<std::chrono::minutes>(now - keyTime);
        return duration.count() > KEY_EXPIRATION_MINUTES;
    }
    else
    {
        throw std::out_of_range("Key not found");
    }
}
