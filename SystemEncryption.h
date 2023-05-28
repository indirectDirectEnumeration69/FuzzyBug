#pragma once
#include "SystemKeyLogic.h"

class Keys {
public:
    bool keys[1024] = { false };
	
	Keys() {

	}
	~Keys() {


	}

	void setKey(int key, bool state);
	bool getKey(int key);

private: 

};

