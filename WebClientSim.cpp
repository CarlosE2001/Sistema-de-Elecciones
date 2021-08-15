#include <iostream>
#include <string>
#include "Includes/Client.h"
#include "Includes/Server.h"

using namespace std;

int main(int argc, char* argv[]){

    int mainTableServerPort = stoi(argv[1]);
    string mainTableServerIP = argv[2];
    
    Client client(mainTableServerPort, mainTableServerIP, "1");
    client.initClient();
    client.sendRequest("118100340-30000-127.0.0.1");

    Server server(30000, "127.0.0.1", "WEBCLIENT");
    server.initServer();
    string confirmation = server.listeningConnection();
    server.closeSocket();

    int condicion = 17;
    cout << "Condicion del votante: " << confirmation[condicion] << endl;

    if(confirmation != "-1" && confirmation[condicion] == '0'){
        Client client2(mainTableServerPort, mainTableServerIP, "1");
        client2.initClient();
        client2.sendRequest("002-GAN-GRI-Carlos");

        Client client3(mainTableServerPort, mainTableServerIP, "1");
        client3.initClient();
        client3.sendRequest("118100340-30000-127.0.0.1*");

        // Server* server2 = new Server(30000, "127.0.0.1", "WEB");
        // server2 -> initServer();
        // confirmation = server2 -> listeningConnection();
        // server2 -> closeSocket();
        // cout << "Confirmation: " << confirmation << endl; 
    }else{
        cout << "No puede votar" << endl;
    }

    

    

}