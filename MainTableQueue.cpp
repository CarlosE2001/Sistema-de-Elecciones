#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <iostream>
#include "Includes/Client.h"
using namespace std;


struct MessageQueue{
    long type;
    char queueMsg[300];
};

const int PORT = 20005;
const string IP = "127.0.0.1";

int main(){
    
    while(true){
        bool sendConf = true;
        MessageQueue message;
        key_t key = ftok("Datos/Queue.txt", 65);
        int msgid = msgget(key, 0666 | IPC_CREAT);
        memset(message.queueMsg, 0, 300);
        int bytesReceived = msgrcv(msgid, &message, sizeof(message), 1, 0);
        string request = string(message.queueMsg, strlen(message.queueMsg));
        cout << "Request pulled from queue: " << request << endl;

        if(request[request.size() - 1] == '*'){
            sendConf = false;
            request = request.substr(0, request.size() - 1);
        }
        
        cout << "Request to be sent: " << request << endl;
        //Partition Controller
        Client client(PORT, IP, "2");
        client.initClient();
        string partitionResponse = client.sendRequest(request);
        cout << partitionResponse << endl;
        //Return to client
        if(sendConf){
            cout << "Connection" << endl;
            Client client2(request.substr(11,request.size()), "5");
            client2.initClient();
            client2.sendConfirmation(partitionResponse);
        }
        
    }
}