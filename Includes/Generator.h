#pragma once
#include <iostream>
#include <string>

using namespace std;

class Generator {
private:

public:
    Generator();
    string generateCode(int votingTable);
    int countDigit(long long n);
};