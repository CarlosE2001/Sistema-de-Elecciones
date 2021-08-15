#include <iostream>
#include <string>
#include "Includes/Client.h"
#include "Includes/Server.h"

using namespace std;

int main(int argc, char* argv[]){

    int port = stoi(argv[1]);
    string ip = argv[2];
    
    Client client(20005, "127.0.0.1", "6");
    client.initClient();
    client.sendTableConnection(port, ip);

    cout << "Starting table server " << argv[3] << endl;

    Server mainTableServer(port, ip, argv[3]);
    mainTableServer.initServer();
    mainTableServer.commands("1Votes.txt-0", "");
    mainTableServer.listeningConnection();
    
}
