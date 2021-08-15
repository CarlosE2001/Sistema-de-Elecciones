#include "Includes/Client.h"

const int bufferSize = 16000;
const int responseBufferSize = 100;
const int requestBufferSize = 300;
const int confirmationBufferSize = 300;
const int queueBufferSize = 100;

Client::Client(int port, string ip, string type){
    this -> port = port;
    this -> ip = ip;
    this ->  type = type;
}

Client::Client(string connectionData, string type){
    this -> port = getPort(connectionData);
    this -> ip = getIP(connectionData);
    this -> type = type;
}

int Client::initClient(){

    cout << "[Client]: Making connection with: " << port << " " << ip << endl;
    this -> sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == -1){
        cout << "[Client]: Socket wasn't created" << endl;
    }

    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(this -> port);
    inet_pton(AF_INET, this -> ip.c_str(), &hint.sin_addr);

    int connected = connect(sock, (sockaddr *)&hint, sizeof(hint));
    if(connected == -1){
        close(sock);
    }else{
        sendType(type);
    }

    return connected;
}

void Client::sendCommand(string command){
    char buffer[bufferSize];
    memset(buffer, 0, bufferSize);
    memcpy(buffer, command.c_str(), command.size());
    
    send(sock, buffer, bufferSize, 0);

    memset(buffer, 0, bufferSize);

    int response = recv(sock, buffer, bufferSize, 0);
    
    if(response > 0){
        // cout << string(buffer, response) << endl;
    }else{
        cout << "Couldn't send command" << endl;
    }   
}

void Client::sendContent(string content){
    char buffer[bufferSize];
    memset(buffer, 0, bufferSize);
    
    int bytesSent = 0;
    while (bytesSent < content.size()){
        string package = content.substr(bytesSent, bufferSize);
        memcpy(buffer, package.c_str(), package.size());
        send(sock, buffer, bufferSize, 0);
        memset(buffer, 0, bufferSize);
        bytesSent += package.size();
    }
    
    memset(buffer, 0, bufferSize);

    int response = recv(sock, buffer, bufferSize, 0);
    
    if(response > 0){
        // cout << string(buffer, response) << endl;
    }else{
        cout << "[Client]:Couldn't send content" << endl;
    }   
}

string Client::sendRequest(string request){
    string response;
    char requestBuffer[requestBufferSize];
    memset(requestBuffer, 0, requestBufferSize);
    memcpy(requestBuffer, request.c_str(), request.size());
    char responseBuffer[responseBufferSize];
    memset(responseBuffer, 0, responseBufferSize);

    send(sock, requestBuffer, strlen(requestBuffer), 0);

    int bytesResponse = recv(sock, responseBuffer, responseBufferSize, 0);

    if(bytesResponse > 0){
        response = string(responseBuffer, bytesResponse);
    }else{
        response = "Bad request";
    }
    cout << "[Client]: Response from server: " << response << endl;

    close(sock);

    return response;

}

void Client::sendType(string type){
    string response;
    int typeBufferSize = 50;
    char typeBuffer[typeBufferSize];
    memset(typeBuffer, 0, typeBufferSize);
    memcpy(typeBuffer, type.c_str(), type.size());
    char responseBuffer[responseBufferSize];
    memset(responseBuffer, 0, responseBufferSize);
    cout << "[Client]: Sending type: " << type << endl;
    send(sock, typeBuffer, typeBufferSize, 0);

    int bytesReceived = recv(sock, responseBuffer, responseBufferSize, 0);
    
    if(bytesReceived > 0){
        response = string(responseBuffer, bytesReceived);
    }else{
        response = "-1";
    }

}

void Client::sendPackage(string command, string content){
    sendCommand(command);
    sendContent(content);
}

string Client::sendConfirmation(string confirmation){
    string response;
    char confirmationBuffer[confirmationBufferSize];
    memset(confirmationBuffer, 0, confirmationBufferSize);
    memcpy(confirmationBuffer, confirmation.c_str(), confirmation.size());

    char responseBuffer[responseBufferSize];
    memset(responseBuffer, 0, responseBufferSize);

    send(sock, confirmationBuffer, confirmationBufferSize, 0);
    
    int bytesReceived = recv(sock, responseBuffer, responseBufferSize, 0);
    
    if(bytesReceived > 0){
        response = string(responseBuffer, bytesReceived);
    }else{
        response = "-1";
    }
    cout << "[Client]: Confirmation Server's response: " << response << endl;
    close(sock);

    return response;
}


string Client::sendTableConnection(int port, string ip){
    string response;
    string data = to_string(port) + "-" + ip;
    char connectionBuffer[requestBufferSize];
    memset(connectionBuffer, 0, requestBufferSize);
    memcpy(connectionBuffer, data.c_str(), data.size());

    char responseBuffer[responseBufferSize];
    memset(responseBuffer, 0, responseBufferSize);

    send(sock, connectionBuffer, requestBufferSize, 0);

    int bytesReceived = recv(sock, responseBuffer, responseBufferSize, 0);

    if(bytesReceived > 0){
        response = string(responseBuffer, strlen(responseBuffer));
    }else{
        response = "[Client]: No response from partition controller";
    }

    cout << "[Client]: " << response << endl;

    close(sock);
    return response;   
}

string Client::sendBackVotes(){

    string votes, response;

    char buffer[bufferSize];
    memset(buffer, 0, bufferSize);

    char responseBuffer[responseBufferSize];
    memset(responseBuffer,0,responseBufferSize);

    int bytesReceived = recv(sock, buffer, bufferSize, 0);

    if(bytesReceived> 0){

        votes = string(buffer, strlen(buffer));
        response = "Votes received";
        memcpy(responseBuffer, response.c_str(), response.size());
        send(sock, responseBuffer, responseBufferSize, 0);

    }else{
        votes = "No votes received";
        response = "Votes corrupted";
        memcpy(responseBuffer, response.c_str(), response.size());
        send(sock, responseBuffer, responseBufferSize, 0);
    }

    close(sock);
    return votes;
    
}


int Client::getPort(string connectionData){
    string port;
    int index = 0;
    while(connectionData[index] != '-'){
        port += connectionData[index];
        index++;
    }

    return stoi(port);

}

string Client::getIP(string connectionData){
    string ip;
    int index = 0;

    while(connectionData[index] != '-'){
        index++;
    }
    index++;

    while(index < connectionData.size()){
        ip += connectionData[index];
        index++;
    }

    return ip;
}
