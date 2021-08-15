#include <iostream>
#include "Includes/Server.h"
#include "Includes/Client.h"

using namespace std;

int main(int argc, char* argv[]){
    cout << "Buenas" << endl;

    Server server(stoi(argv[1]), argv[2], argv[3]);
    server.initServer();
    server.listeningConnection();

}