#include "Includes/Server.h"

//Buffer Constants
const int bufferSize = 16000;
const int responseBufferSize = 100;
const int requestBufferSize = 300;
const int confirmationBufferSize = 300;
const int queueBufferSize = 300;

//Server Constants
const int MainServerPort = 20000;
const string MainServerIP = "127.0.0.1";

const int ServerPartition1Port = 20001;
const string ServerPartition1IP = "127.0.0.1";

const int ServerPartition1BackupPort = 20002;
const string ServerPartition1BackupIP = "127.0.0.1";

const int ServerPartition2Port = 20003;
const string ServerPartition2IP = "127.0.0.1";

const int ServerPartition2BackupPort = 20004;
const string ServerPartition2BackupIP = "127.0.0.1";

const int ServerPartitionControllerPort = 20005;
const string ServerPartitionControllerIP = "127.0.0.1";

const int MainTableServerPort = 20006;
const string MainTableServerIP = "127.0.0.1";

const int WebClientSimPort = 20007;
const string WebClientSimIP = "127.0.0.1";

//End of Server Constants

//Queue Constants
const int MainTableServerQueue = 65;
const int MainPartitionServerQueue = 66;

Server::Server(int port, string ip, string id){
    this -> port = port;
    this -> ip = ip;
    this -> user = new User(id, 1, 1, 1, 1);
    this -> fs = new FileSystem();
}

Server::Server(int port, string ip, string id, bool p1, bool p2, bool p3, bool p4){
    this -> port = port;
    this -> ip = ip;
    this -> user = new User(id, p1, p2, p3, p4);
    this -> fs = new FileSystem();
}

/**
 * @brief Método que inicializa el servidor, aquí es donde se empieza a escuchar para tener
 * diferentes conexiones 
 */
void Server::initServer(){
    this -> serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    cout << "[Server]: Socket Status: " << serverSocket << endl;

    if(serverSocket == -1){
        cerr << "Can't crete the server" << endl;
        exit(1);
    }else{
        cout << "[Server]: Sock created on port " << port << " " << ip << endl;
    }

    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(this -> port);
    inet_pton(AF_INET, this -> ip.c_str(), &hint.sin_addr);

    cout << "[Server]: Binding..." << endl;
    sleep(3);

    bind(serverSocket, (sockaddr *)&hint, sizeof(hint));
    listen(serverSocket, SOMAXCONN);
    cout << "[Server]: Binding complete\n" << endl;
}

string Server::receivedType(){
    int typeBufferSize = 50;
    string type, response;
    char typeBuffer[typeBufferSize];
    memset(typeBuffer, 0, typeBufferSize);
    char responseBuffer[responseBufferSize];
    memset(responseBuffer, 0, responseBufferSize);
    
    int bytesReceived = recv(clientSocket, typeBuffer, typeBufferSize, 0);

    if(bytesReceived > 0){
        type = string(typeBuffer, strlen(typeBuffer));
    }else{
        type = "-1";
    }

    response = "Good connection";
    memcpy(responseBuffer, response.c_str(), response.size());
    send(clientSocket, responseBuffer, responseBufferSize, 0);

    return type;
}


string Server::listeningConnection(){
    string response = "";
    bool keepAlive = true;
    while(keepAlive){
        acceptConnectionRequest();
        string type = receivedType();
        cout << "[Server]: Connection type: " << type[0] << endl;
        if(type != "-1"){
            
            if(type[0] == '0'){
                receivePackage();
            }else if(type[0] == '1'){
                receiveRequest();
            }else if(type[0] == '2'){
                receiveFromQueue();
            }else if(type[0] == '3'){
                receiveRequestFromController();
            }else if(type[0] == '4'){
                receiveUpdateFromQueue();
            }else if(type[0] == '5'){
                response = receiveConfirmation();
                keepAlive = false;
            }else if(type[0] == '6'){
                receiveTable();
            }else if(type[0] == '7'){
                askForVotes();
                keepAlive = false;
            }else{
                sendVotes();
                keepAlive = false;
            }

        }else{
            cout << "[Server]: Connection Refused" << endl;
        }    
        close(clientSocket);
        cout << "[Server]: Connection terminated\n" << endl;
    }
    
    return response;
}

void Server::acceptConnectionRequest(){
    sockaddr_in client;
    socklen_t clientSize = sizeof(client);
    this -> clientSocket = accept(serverSocket, (sockaddr *)&client, &clientSize);

    char host[NI_MAXHOST];
    char service[NI_MAXSERV];

    memset(host, 0, NI_MAXHOST);
    memset(service, 0, NI_MAXSERV);

    
    inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
    cout << "[Server]: " << host << " connected on port " << ntohs(client.sin_port) << endl;
}



/*
 * ******************************************************************
 * ******************* RECEIVE FUNCTIONS ****************************
 * ******************************************************************
*/ 

/**
 * @brief Método que recibe un comando desde un cliente 
 * @return string comando recibido
 */
string Server::receiveCommand(){
    string command, response;
    char buffer[bufferSize];
    memset(buffer, 0 , bufferSize);
    char responseBuffer[responseBufferSize];
    memset(responseBuffer, 0, responseBufferSize);

    int commandReceived = recv(clientSocket, buffer, bufferSize, 0);
    if(commandReceived > 0){ 
        command = string(buffer, commandReceived);
        response = "----------------\nCommand Received\n----------------";
        memcpy(responseBuffer, response.c_str(), response.size());
        send(clientSocket, responseBuffer, responseBufferSize, 0);
    }else{
        command = "-1";
        response = "---------------\nCommand Corrupt\n---------------";
        memcpy(responseBuffer, response.c_str(), response.size());
        send(clientSocket, responseBuffer, responseBufferSize, 0);
    }
    
    return command;
}

/**
 * @brief Método que recibe los datos de un cliente
 * @return string retorna los datos recibidos
 */
string Server::receiveDataPackage(int fileSize){
    string content, response;
    char buffer[bufferSize];
    memset(buffer, 0, bufferSize);
    char responseBuffer[responseBufferSize];
    memset(responseBuffer, 0, responseBufferSize);
    int fileSizeReceived = 0;

    while(fileSizeReceived < fileSize){
        int bytesReceived = recv(clientSocket, buffer, bufferSize, 0);
        if(bytesReceived > 0){
            content += string(buffer, bytesReceived);
            memset(buffer, 0, bufferSize);
            fileSizeReceived = content.size();
        }
    }

    response = "Receive Completed " + to_string(content.size()) + "/" + to_string(fileSize);
    memcpy(responseBuffer, response.c_str(), response.size());
    send(clientSocket, responseBuffer, responseBufferSize, 0);

    return content;
}

/**
 * @brief Método que recibe un paquete completo (comando y contenido) 
 */
void Server::receivePackage(){
    string command = receiveCommand();
    int fileSize = getFileSize(command);
    string content = receiveDataPackage(fileSize);
    commands(command, content);
}

void Server::receiveRequest(){
    string request, response;
    char requestBuffer[requestBufferSize];
    memset(requestBuffer, 0, requestBufferSize);
    char responseBuffer[responseBufferSize];
    memset(responseBuffer, 0, responseBufferSize);

    int bytesReceived = recv(clientSocket, requestBuffer, requestBufferSize, 0);

    if(bytesReceived > 0){
        request = string(requestBuffer, bytesReceived);
        processRequest(request);
        //Se le responde al cliente que la información llegó bien
        response = "Request Received";
        memcpy(responseBuffer, response.c_str(), response.size());
        send(clientSocket, responseBuffer, responseBufferSize, 0);


    }else{
        //Se le responde al cliente que la información llegó mal
        response = "Request failed";
        memcpy(responseBuffer, response.c_str(), response.size());
        send(clientSocket, responseBuffer, responseBufferSize, 0);
    }

}

void Server::receiveFromQueue(){
    string request, response;
    char requestBuffer[requestBufferSize];
    memset(requestBuffer, 0, requestBufferSize);
    char responseBuffer[responseBufferSize];
    memset(responseBuffer, 0, responseBufferSize);

    int bytesReceived = recv(clientSocket, requestBuffer, requestBufferSize, 0);

    if(bytesReceived > 0){
        request = string(requestBuffer, bytesReceived);
        string id = request.substr(1,9);
        cout << "Node: " << user -> username << " received from queue: " << request << " with request type: " << (int) request[0] -'0' << endl;
        response = findClientHost(id, (int) request[0]-'0');
        cout << "Sending response: " << response << endl;
        memcpy(responseBuffer, response.c_str(), response.size());
        send(clientSocket, responseBuffer, responseBufferSize, 0);
    }else{
        request = "-1";
        response = "Client's Data Corrupted?";
        memcpy(responseBuffer, response.c_str(), response.size());
        send(clientSocket, responseBuffer, responseBufferSize, 0);
    }

}

void Server::receiveUpdateFromQueue(){
    string request, response;
    char requestBuffer[requestBufferSize];
    memset(requestBuffer, 0, requestBufferSize);
    char responseBuffer[responseBufferSize];
    memset(responseBuffer, 0, responseBufferSize);

    int bytesReceived = recv(clientSocket, requestBuffer, requestBufferSize, 0);

    if(bytesReceived > 0){
        request = string(requestBuffer, bytesReceived);
        cout << "Node: " << user -> username << " received from queue: " << request << endl;
        changeVotersCondition(request.substr(0,9), "1");
        response = getVoterById(request.substr(0,9));
        memcpy(responseBuffer, response.c_str(), response.size());
        send(clientSocket, responseBuffer, responseBufferSize, 0);
    }else{
        request = "-1";
        response = "Client's Data Corrupted?";
        memcpy(responseBuffer, response.c_str(), response.size());
        send(clientSocket, responseBuffer, responseBufferSize, 0);
    }
    cout << "Result of the change: " << response << endl;
}

void Server::receiveRequestFromController(){
    string data, response;
    char buffer[queueBufferSize];
    memset(buffer, 0, queueBufferSize);
    char responseBuffer[responseBufferSize];
    memset(responseBuffer, 0, responseBufferSize);

    int bytesReceived = recv(clientSocket, buffer, queueBufferSize, 0);

    if(bytesReceived > 0){
        data = string(buffer, strlen(buffer));
        cout << "Request from Controller: " << data << endl;

        response = processRequestFromController(data);

        memcpy(responseBuffer, response.c_str(), response.size());
        send(clientSocket, responseBuffer, responseBufferSize, 0);
    }else{
        data = "-1";
        response = "Client's Data Corrupted?";
        memcpy(responseBuffer, response.c_str(), response.size());
        send(clientSocket, responseBuffer, responseBufferSize, 0);
    }

}

string Server::receiveConfirmation(){
    string data, response;
    char buffer[queueBufferSize];
    memset(buffer, 0, queueBufferSize);
    char responseBuffer[responseBufferSize];
    memset(responseBuffer, 0, responseBufferSize);
    cout << "Esperando confirmación" << endl;

    int bytesReceived = recv(clientSocket, buffer, queueBufferSize, 0);

    if(bytesReceived > 0){
        data = string(buffer, bytesReceived);
        cout << "Voter info from Main Server: " << data << endl;
        response = "Data received";
        memcpy(responseBuffer, response.c_str(), response.size());
        send(clientSocket, responseBuffer, responseBufferSize, 0);
    }else{
        data = "-1";
        response = "Client's Data Corrupted?";
        memcpy(responseBuffer, response.c_str(), response.size());
        send(clientSocket, responseBuffer, responseBufferSize, 0);
    }

    return data;
}


void Server::receiveTable(){
    string data, response;
    char buffer[queueBufferSize];
    memset(buffer, 0, queueBufferSize);
    char responseBuffer[responseBufferSize];
    memset(responseBuffer, 0, responseBufferSize);

    int bytesReceived = recv(clientSocket, buffer, bufferSize, 0);

    if(bytesReceived > 0){

        string connection = string(buffer, strlen(buffer));
        int port = getPort(connection);
        string ip = getIP(connection);
        addTable(port, ip);

        response = "Table on port: " + to_string(port) + " IP: " + ip;
        memcpy(responseBuffer, response.c_str(), response.size());
        send(clientSocket, responseBuffer, responseBufferSize, 0);

    }else{

        response = "Bad table connection";
        memcpy(responseBuffer, response.c_str(), response.size());
        send(clientSocket, responseBuffer, responseBufferSize, 0);

    }

    cout << "SERVER:" << response << endl;
}





void Server::processRequest(string request){
    cout << "Request received from client: " << request << endl;

    if(request[request.size()-1] == '*'){

        string actualizacionPadron = "4" + request;
        addToQueue(actualizacionPadron); 

    }else if((int) request[request.size()-1]  < 97){

        addToQueue("3"+request);

    }else{    

        cout << "Se almacena un voto " << request << endl;
        commands("3Votes.txt-0", request);

    }
}

void Server::addToQueue(string request){
    key_t key = ftok("Datos/Queue.txt", MainTableServerQueue);
    int msgid = msgget(key, 0666 | IPC_CREAT);
    MessageQueue message;
    message.type = 1;
    memset(message.queueMsg, 0, queueBufferSize);
    memcpy(message.queueMsg, request.c_str(), request.size());
    msgsnd(msgid, &message, sizeof(message), 0);
}





/**
 * @brief Se conecta al controlador de partición
 * 
 * @param requestFromQueue 
 * @return true 
 * @return false 
 */
string Server::findClientHost(string requestFromQueue, int type){
    string idMatch;
    bool found = false;
    int port = 20001;
    for(int i = 0; i < 2 && !found; i++){
        
        Client c(port, ServerPartitionControllerIP, to_string(type));
        c.initClient();
        cout << "Id to find: " << requestFromQueue << endl;
        idMatch = c.sendRequest(requestFromQueue);
        if(idMatch.substr(0,9) == requestFromQueue){
            found = true;
        }
        port += 2;

    }
    cout << "Id returned: " << idMatch << endl;
    return idMatch;
}



string Server::updatePartitions(string requestID){
    string idMatch;
    bool found = false;
    int port = 20001;
    for(int i = 0; i < 2 && !found; i++){
        
        Client c(port, ServerPartitionControllerIP, "3");
        c.initClient();
        idMatch = c.sendRequest(requestID);
        if(idMatch.substr(0,9) == requestID){
            found = true;
        }
        port += 2;

    }
    cout << "Id returned: " << idMatch << endl;
    return idMatch;
}



string Server::processRequestFromController(string request){
    string voterInfo = getVoterById(request);
    if(voterInfo.substr(0,9) == request && voterInfo[17] != '1'){
        changeVotersCondition(request, "2");
    }else{
        cout << "Vote already been placed" << endl;
    }

    return voterInfo;
    
}





void Server::askForVotes(){
    string data, response;
    char buffer[bufferSize];
    memset(buffer, 0, bufferSize);
    char responseBuffer[responseBufferSize];
    memset(responseBuffer, 0, responseBufferSize);

    for(map<int, string>::iterator it = tables.begin(); it != tables.end(); it++){
        Client client(it ->first, it->second, "8");
        client.initClient();
        string votesReceived = client.sendBackVotes();
        data += votesReceived;
    }

    cout << "Sending to Main Server:" << endl;
    cout << data << endl;
}

void Server::sendVotes(){
	string data, response;
	char votesBuffer[bufferSize];
	memset(votesBuffer, 0, bufferSize);
	char responseBuffer[responseBufferSize];
	memset(responseBuffer, 0, responseBufferSize);

	data = fs -> getFileContent(this -> user, "Votes.txt");

	// Envia los votos 
	memcpy(votesBuffer, data.c_str(), data.size());
	send(clientSocket, votesBuffer, bufferSize, 0);

	// Recibe respuesta
	int bytesReceived = recv(clientSocket, responseBuffer, responseBufferSize, 0);

	if(bytesReceived > 0){
		response = "Votes transferred correctly";
	}
	else{
		response = "Error transferring votes";
	}

	cout << response << endl;
}



/**
 * @brief Método que interactúa con el FileSystem, recibe un comando y un contenido 
 * @param command comando recibido
 * @param content contenido recibido
 */
void Server::commands(string command, string content){
    int flag = command[0] - '0';
    string fileName = getFileName(command);
    int fd = -1;

    switch (flag) {
	case 1: //Create
		fs -> create(this -> user, fileName);
		break;

	case 2: //Write
		fd = fs -> openFile(user, fileName, "w");
        fs -> writeFile(user, fileName, content);
        fs -> closeFile(user, fileName);
		break;

	case 3:	//Append
		fd = fs -> openFile(user, fileName, "w");
		fs -> append(user, fileName, content);
		fs -> closeFile(user, fileName);
		break;

	case 4: //Read
		fd = fs -> openFile(user, fileName, "r");
		fs -> readFile(user, fileName);
		fs -> closeFile(user, fileName);
		break;

	case 5: //Delete
		fs -> deleteFile(user, fileName);
		break;

	case 6: //View Server State
		fs -> memoryStatus();
        fs -> HDcontents();
		break;

	case 7: //Vote count
		break;

	case 8: //End
		//The command was to stop the server
		cout << "TCP SOCKET: The Server has finished" << endl;
		exit(EXIT_SUCCESS);
		//fs -> memoryStatus();
		break;

	default:
		break;
	}
    

    // saveToExternalDrive(user -> username + ".txt", "Prueba de Datos");
}

/**
 * @brief Método que crea un cliente y se conecta con un servidor para mandar información (commando y contenido) 
 * @param port puerto del servidor destino
 * @param ip ip del servidor destino
 * @param command comando a ser enviado
 * @param content contenido a ser enviado
 */
void Server::sendPackageToServer(int port, string ip, string command, string content){
    Client client(port, ip, "0");
    client.sendPackage(command, content);
    closeSocket();
}

/**
 * @brief Método que se encarga de distribuir el padrón 
 * @param command commando para los servidores
 * @param fileName nombre del archivo
 * @param content contenido a enviar
 */
void Server::broadcastVoters(string command, string fileName, string content){
    int i = 0;
    int succeeds = 0;
    int fails = 0;
    int connection = -1;
    vector<string> partitions = makeContentPartition(distributedServers.size()/2, content);

    cout << "\033[1;34m" << "-----------------------------------------BROADCAST VOTERS-----------------------------------------" << "\033[0m" << endl;
    
    for(map<int, string>::iterator it = distributedServers.begin(); it != distributedServers.end(); it++ ){
        Client clientAux(it -> first, it -> second, "0");
        connection = clientAux.initClient();
        if (connection >= 0){
            clientAux.sendPackage(createCommand(fileName, to_string(partitions[i].size()), command), partitions[i]);
            succeeds++;
        }else{
            fails++;
        }
        
        it++;

        Client clientAux2(it -> first, it -> second, "0");
        connection = clientAux2.initClient();
        if(connection >= 0){
            clientAux2.sendPackage(createCommand(fileName, to_string(partitions[i].size()), command), partitions[i]);
            succeeds++;
        }else{
            fails++;
        }
        
        i++;
    }
    cout << "\033[1;32m" << "Succeeds: " << succeeds <<"\033[0m" << endl;
    cout << "\033[1;31m" << "Fails: " << fails <<"\033[0m" << endl;

    cout << "\033[1;34m" << "-----------------------------------------BROADCAST VOTERS-----------------------------------------" << "\033[0m" << endl;
}


/**
 * @brief Método que se encarga de enviar un paquete a todos los servidores hijos conectados 
 * @param command comando a enviar
 * @param content contenido a enviar
 */
void Server::broadcast(string command, string content){
    int fails = 0;
    int succeeds = 0;
    int connection;
    cout << "\033[1;34m" << "\n\n--------------------------------------------BROADCAST---------------------------------------------" << "\033[0m" << endl;
    for(map<int, string>::iterator server = distributedServers.begin(); server != distributedServers.end(); server++){
        Client c(server -> first, server -> second, "0");
        cout << "\033[1;33m" <<  "Making connection with port " << server -> first << " at " << server -> second << "\033[0m" << endl;
        connection = c.initClient();
        if (connection >= 0){
            c.sendPackage(command, content);
            succeeds++;
        }else{
            fails++;
        }
    }

    cout << "\033[1;32m" << "Succeeds: " << succeeds <<"\033[0m" << endl;
    cout << "\033[1;31m" << "Fails: " << fails <<"\033[0m" << endl;
    cout << "\033[1;34m" << "--------------------------------------------BROADCAST---------------------------------------------\n\n" << "\033[0m" << endl;
}

/**
 * @brief Método que realiza una partición para ser distribuida entre los nodos 
 * @param numberOfNodes numero de nodos o particiones
 * @param content contenido a partir
 * @return vector<string> retorna un vector de tamaño numberOfNodes con las particiones
 */
vector<string> Server::makeContentPartition(int numberOfNodes, string content){
    int partitionSize = content.size() / numberOfNodes;
    vector<string> partitions;
    string partition;
    int lastIndex = 0;
    int nodesReady = 0;
    while(nodesReady < numberOfNodes){
        partition = content.substr(lastIndex, partitionSize);
        lastIndex += partition.size();
        if(lastIndex < content.size() && partition[lastIndex] != '\n'){
            while(lastIndex < content.size() && content[lastIndex] != '\n'){
                partition += content[lastIndex];
                lastIndex++;
            }
            lastIndex++;
        }
        partitions.push_back(partition);
        partition = "";
        nodesReady++;
    }
    return partitions;
}

void Server::changeVotersCondition(string id, string condition){
    string voter = getFileContent("Padron.csv");
    int statePos = -1;
    bool idFound = false;
    int index = 0;
    if(id.size() == 9){ //Id valid
        while(!idFound && index < voter.size()){
            string idCheck = voter.substr(index, 9);
            if(idCheck == id){
                idFound = true;
                statePos = index + 17;
                fs -> overwriteData(user, "Padron.csv", statePos, statePos+1, condition);
                continue;
            }
            index += 18;
            while(index < voter.size() && voter[index] != '\n'){
                index++;
            }
            index++;
        }
        if(!idFound){
            voter = "No voter was found with id " + id;
        }
    }else{
        voter = "Id provided is invalid";
    }

}


string Server::getVoterById(string id){
    string voter = getFileContent("Padron.csv");
    bool idFound = false;
    int index = 0;
    if(id.size() == 9){ //Id valid
        while(!idFound && index < voter.size()){
            string idCheck = voter.substr(index, 9);
            if(idCheck == id){
                int lastChar = index + 20;
                while(lastChar < voter.size() && voter[lastChar] != '\n'){
                    lastChar++;
                }
                voter = voter.substr(index, lastChar-index);
                idFound = true;
                continue;
            }
            index += 18;
            while(index < voter.size() && voter[index] != '\n'){
                index++;
            }
            index++;
        }
        if(!idFound){
            voter = "No voter was found with id " + id;
        }
    }else{
        voter = "Id provided is invalid";
    }
    voter = voter.substr(0, voter.size()-1);
    return voter;
}

/**
 * @brief Método que lee la memoria y obtiene el contenido de un archivo 
 * @param fileName Nombre del archivo
 * @return string contenido del archivo
 */
string Server::getFileContent(string fileName){
    string content;
    fs -> openFile(user, fileName, "r");
    content = fs -> getFileContent(user, fileName);
    fs -> closeFile(user, fileName);
    return content;
}

/**
 * @brief Método que crea un comando 
 * @param fileName nombre del archivo
 * @param size tamaño del archivo
 * @param flag bandera del archivo
 * @return string retorna el comando creado
 */
string Server::createCommand(string fileName, string size, string flag){
    string command = flag + fileName + "-" + size;
    return command;
}

/**
 * @brief Retorna el nombre del archivo proveniente del mensaje de cabecera 
 * @param message Mensaje recibido en el servidor
 * @return string nombre del archivo que se va a recibir
 */
string Server::getFileName(string message){
	int index = 1;
	string fileName;
	while(message[index] != '-'){
		fileName += message[index];
		index++;
	}
	return fileName;
}

/**
 * @brief Retorna el tamaño del archivo que se va a recibir
 * @param message Mensaje recibido en el servidor
 * @return int tamaño del archivo que se va a recibir;
 */
int Server::getFileSize(string message){
	int size;
	int index = 1;
	string fileSize;
	while(message[index] != '-'){ //Looking for the start of the fileSize part
		index++;
	}

	index++;
	
	while(index < message.size()){ 
		fileSize += message[index];
		index++;
	}
	size = stoi(fileSize);

	return size;
}

/**
 * @brief Método que cierra la conexión con un cliente 
 */
void Server::closeSocket(){
    close(clientSocket);
}

/**
 * @brief Método que permite el conteo de votos 
 * @param votesFileName nombre del archivo guardado en la memoria virtual
 * @return string  conteo final de los votos
 */
string Server::countVotes(string votesFileName){
    fs -> openFile(user, votesFileName, "r");
    string votes = fs -> getFileContent(user, votesFileName);
    fs -> closeFile(user, votesFileName);
    string voter, candidate;
    vector<string> votesCounted;
    for(int i = 0; i < votes.size(); i++){
        if(votes[i] == '\n' || votes[i] == '\r'){
            int index = voter.size() - 1;
            while(voter[index] != '-' && index > 0){
                candidate = voter[index] + candidate;
                index--;
            }
            votesCounted.push_back(candidate);
            candidate = "";
            voter = "";
            
            candidate = "";
        }else{
            voter += votes[i];
        }
    }
    for(map<string, int>::iterator it = candidates.begin(); it != candidates.end(); it++){
        string candidateByIndex = it -> first;
        for(vector<string>::iterator it2 = votesCounted.begin(); it2 != votesCounted.end(); it2++){
            if(*it2 == candidateByIndex){
                it -> second += 1;
            }
        }
    }

    string finalCount = "\n----------------------------\nResultados de las elecciones\n----------------------------\n";

    for(map<string, int>::iterator it = candidates.begin(); it != candidates.end(); it++){
        finalCount += it -> first + ": " + to_string(it -> second) + "\n";
    }
    return finalCount;
}

void Server::saveToExternalDrive(string fileName, string data){
    ofstream file;
    file.open("../State/" + fileName);
    file << data;
    file.close();
}


int Server::getPort(string connectionData){
    string port;
    int index = 0;
    while(connectionData[index] != '-'){
        port += connectionData[index];
        index++;
    }

    return stoi(port);

}

string Server::getIP(string connectionData){
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

/**
 * @brief Método que permite cargar el padrón electoral  
 * @param fileName nombre del archivo para cargar el padrón electoral
 */
void Server::loadVotersToServer(){
    string fileName = "Padron.csv";
    ifstream votersFile("../Datos/" + fileName);
    string line, voters;
    int maxSize = 0;
    int lineNumber = 0;
    if(votersFile.is_open()){
        while(getline(votersFile, line)){
            if(lineNumber > 0){
                voters += line + "\n";
                if(line.size() > maxSize){
                    maxSize = line.size();
                }
            }
            lineNumber++;
        }
        votersFile.close();
    }else{
        cout << "Unable to open file" << endl;
    }
    fs -> create(user, fileName);
    int fd = fs -> openFile(user, fileName, "w");
    fs -> writeFile(user, fileName, voters);
    fs -> closeFile(user, fileName);

}
/**
 * @brief Método que se encarga de cargar los archivos de votos hacia el servidor 
 * @param fileName nombre del archivo para extraer los datos
 */
void Server::loadVotesToServer(){
    string fileName = "Votes.txt";
    ifstream votesFile("../Datos/" + fileName);
    string line, votes;
    if(votesFile.is_open()){
        while(getline(votesFile, line)){
            votes += line + "\n";
        }

        votesFile.close();
    }else{
        cout << "Couldn't load the votes" << endl;
    }

    fs -> create(user, fileName);
    int fd = fs -> openFile(user, fileName, "w");
    fs -> writeFile(user, fileName, votes);
    fs -> closeFile(user, fileName);
}


void Server::loadCandidates(){
    //Read the config file
}


/**
 * @brief Método que agrega los canditos al arreglo de candidatos  
 * @param candidate candidato para agregar
 */
void Server::addCandidate(string candidate){
    this -> candidates.insert(make_pair(candidate, 0));
}

/**
 * @brief Método que agrega el puerto y la ip de un servidor a su arreglo de servidores 
 * @param port puerto del servidor
 * @param ip  ip del servidor
 */
void Server::addServer(int port, string ip){
    this -> distributedServers.insert(make_pair(port, ip));
}

/**
 * @brief Método que agrega el puerto y la ip de una mesa al arreglo de mesas
 * @param port puerto del servidor
 * @param ip  ip del servidor
 */
void Server::addTable(int port, string ip){
    this -> tables.insert(make_pair(port, ip));
}

void Server::loadConfigFile(string configType){
    string line, port, ip;
    string fileName = "../Datos/" + configType + ".txt";
    ifstream file(fileName);
    getline(file, line);
    while(getline(file, line)){
        port = line.substr(0, line.find(" "));
        ip = line.substr(line.find(" ") + 1, line.size());
        addServer(stoi(port), ip);
    }
    file.close();
}