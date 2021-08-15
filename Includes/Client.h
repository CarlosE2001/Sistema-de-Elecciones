#pragma once
#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "Generator.h"

using namespace std;

class Client{
private:
    int port;
    int sock;
    string ip;
    string type;
    Generator* genCode;
    int portClient;
    string ipClient;

public:

    Client(int port, string ip, string type);
    Client(string connectionData, string type);
    int initClient(); //Return the socket fd

    //Basic Functions
    void sendCommand(string command);
    void sendContent(string content);
    string sendRequest(string request);
    void sendType(string type);
    void sendPackage(string command, string content);
    string sendRequestToController(string request);
    string sendConfirmation(string confirmation);
    string sendTableConnection(int port, string ip);
    string sendBackVotes();

    int getPort(string connectionData);
    string getIP(string connectionData);
};