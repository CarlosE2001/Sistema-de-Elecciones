#include <iostream>
#include <string>
#include "Includes/Server.h"


using namespace std;

int main(int argc, char* argv[])
{
    
    Server server(20000, "127.0.0.1", "MAIN_SERVER");
    server.loadVotersToServer();
    server.loadVotesToServer();

    server.addCandidate("Carlos");
    server.addCandidate("Fabricio");

    server.loadConfigFile("MainServer");

    string voters = server.getFileContent("Padron.csv");
    string command = server.createCommand("Padron.csv", "0", "1");

    string results = server.countVotes("Votes.txt");
    // cout << results << endl;

    server.broadcast(command, "");
    server.broadcastVoters("2", "Padron.csv", voters);
    

    return 0;
}
