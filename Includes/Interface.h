#pragma once
#include <iostream>
#include "FileSystem.h"

using namespace std;

class Interface {
private:
    FileSystem* fs;
public:
    Interface();
    int runInterface();
};