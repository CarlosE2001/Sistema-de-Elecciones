clean:
	rm ./Executables/server ./Executables/child ./Executables/webClientSim ./Executables/trigger ./Executables/mainTableServer ./Executables/mainTableQueue ./Executables/mainPartitionController

server:
	g++ FileSystem.cpp Unit.cpp Server.cpp Client.cpp MainServer.cpp -o ./Executables/server


universalClient:
	g++ FileSystem.cpp Unit.cpp Client.cpp Server.cpp MainServerChild.cpp -o ./Executables/child

mainPartitionController:
	g++ FileSystem.cpp Unit.cpp Client.cpp Server.cpp MainPartitionController.cpp -o ./Executables/mainPartitionController

mainTableServer:
	g++ FileSystem.cpp Unit.cpp Client.cpp Server.cpp MainTableServer.cpp -o ./Executables/mainTableServer

mainTableQueue:
	g++ FileSystem.cpp Unit.cpp Client.cpp Server.cpp MainTableQueue.cpp -o ./Executables/mainTableQueue

webClientSim:
	g++ FileSystem.cpp Unit.cpp Client.cpp Server.cpp WebClientSim.cpp -o ./Executables/webClientSim

trigger:
	g++ FileSystem.cpp Unit.cpp Client.cpp Server.cpp Trigger.cpp -o ./Executables/trigger

all:
	make server
	make universalClient
	make mainTableServer
	make mainTableQueue
	make mainPartitionController
	make webClientSim
	make trigger