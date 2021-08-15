#include <iostream>
#include <string>
#include "Includes/Client.h"
#include "Includes/Server.h"

using namespace std;

int main(int argc, char* argv[]){

    Server backServer(stoi(argv[1]), argv[2], argv[3]);
    backServer.initServer();
    backServer.listeningConnection();
    backServer.commands("6-0", "");

}