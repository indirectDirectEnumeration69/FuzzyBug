#pragma once
#include "SystemKeyLogic.h"
#include <unordered_map>
#include <random>
#include <chrono>
#include <string>
#include <sstream>
#include <vector>
#include <array>
#include <algorithm>

class Keys {
public:

    std::unordered_map<int, long long> keys;
    std::unordered_map<int, std::chrono::system_clock::time_point> keyTimestamps;
    static const int KEY_EXPIRATION_MINUTES = 60;

    Keys() = default;
    ~Keys() = default;

    void setKey(int key);
    long long getKey(int key) const;
    bool isKeySet(int key) const;
    bool isKeyExpired(int key) const;

private:
    long long generateRandomNumber();
};

class KeyLogic {
public:

    void setKeys(int key) {

        UseKeys.setKey(key);
    }

    long long getKey(int key) {
        return UseKeys.getKey(key);
    }

    bool isKeySet(int key) {
        return UseKeys.isKeySet(key);
    }

    bool isKeyExpired(int key) {
        return UseKeys.isKeyExpired(key);
    }

    void setPassword(const std::string& password) {
        this->password = password;
    }

    bool checkPassword(const std::string& password) const {
        return this->password == password;
    }
    void generateSessionID() {
        auto now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> distr(0, 999999999);
        std::array<int, 4> random_numbers;
        for (int& num : random_numbers) {
            num = distr(gen);
        }
        std::string str("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz?<>/~][^¬|'!@#$-%^&*()_+/`'':");
        std::shuffle(str.begin(), str.end(), gen);
        std::string random_string = str.substr(0, 33);

        std::ostringstream oss;
        oss << now;

        for (const int& num : random_numbers) {
            oss << "_" << num;
        }

        oss << "_" << random_string;

        sessionID = oss.str();
    }

    std::string getSessionID() const {

        return sessionID;
    }

private:
    Keys UseKeys;
    std::string password;
    std::string sessionID;
};

class checkintegration {
public:
    checkintegration() {
    if (!Key.isKeySet(1)) {
        Key.setKeys(1);
    }

    long long keyValue = Key.getKey(1);

    if (keyValue == 0 || Key.isKeyExpired(1)) {
        KeyValid = false;
        KeyIntegrated = false;
    }
    else if(keyValue != 0 && (Key.getSessionID() != "" && Key.getSessionID().size() > 20)){
        KeyValid = true;
        KeyIntegrated = true;
    }
}
    bool isKeyValid() const {
        return KeyValid;
    }

    bool isKeyIntegrated() const {
        return KeyIntegrated;
    }
private:
    bool KeyValid = false;
    bool KeyIntegrated = false;
    KeyLogic Key;
};
