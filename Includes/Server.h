#pragma once
#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <fstream>
#include "Client.h"
#include "FileSystem.h"
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <map>


using namespace std;

struct MessageQueue{
    long type;
    char queueMsg[300];
};

class Server{
private:
    //Sockets
    int serverSocket;
    int clientSocket;

    //Server port
    int port, clientPort;

    //Server ip
    string ip, clientIP;

    //User with id and permissions
    User* user;

    //Dedicated fileSystem
    FileSystem* fs;

    //Candidates map [name, votes]
    map<string, int> candidates;

    //Child Servers [port, ip]
    map<int, string> distributedServers;

    //Tables [port, ip]
    map<int, string> tables;

public:
    //Constructors
    Server(int port, string ip, string id);
    Server(int port, string ip, string id, bool p1, bool p2, bool p3, bool p4);

    //Initial commands
    void initServer();
    void listeningServer();
    void listeningTableServer();
    void listeningWebClient();
    string listeningConnection();
    void acceptConnectionRequest();

    
    
    //Server Receiving Functions
    string receivedType();
    string receiveCommand();
    string receiveDataPackage(int fileSize);
    // string receiveClientData();
    void receiveRequest();
    void receiveFromQueue();
    void receiveUpdateFromQueue();
    void receiveRequestFromController();
    void receivePackage();
    string receiveConfirmation();
    void receiveTable();
    void askForVotes();
    void sendVotes();

    //Client Partition Functions
    void sendPackageToServer(int port, string ip, string command, string content);
    void broadcast(string command, string content);
    void broadcastVoters(string command, string fileName, string content);
    string findClientHost(string requestFromQueue, int type);

    //Data treatment methods
    string getFileName(string message);
    int getFileSize(string message);
    int getPort(string connection);
    string getIP(string connection);
    void saveToExternalDrive(string fileName, string data);
    
    string createCommand(string fileName, string size, string command);
    string getFileContent(string fileName);
    string getVoterById(string id);
    vector<string> makeContentPartition(int numberOfNodes, string content);
    void commands(string command, string content);
    void changeVotersCondition(string id, string status);
    void processRequest(string request);
    string processRequestFromController(string request);
    void addToQueue(string request);
    string updatePartitions(string requestID);
    //CLosing Socket 
    void closeSocket();

    //Final commands
    string countVotes(string votesFileName);

    //Loading Data Methods
    void loadVotersToServer();
    void loadVotesToServer();
    void loadCandidates();
    void loadConfigFile(string configType);
    void addCandidate(string candidate);
    void addServer(int port, string ip);
    void addTable(int port, string ip);

};