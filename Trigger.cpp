#include <iostream>
#include "Includes/Client.h"

using namespace std;

int main(int argc, char* argv[]){
    
    Client trigger(stoi(argv[1]), argv[2], "7");
    trigger.initClient();
    
    return 0;
}