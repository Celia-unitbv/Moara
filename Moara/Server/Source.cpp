#include "GameServer.h"

#include <iostream>

int main(int argc, char** argv) {

    GameServer server;
	int port = 53000;

	if (!server.Init(port))
	{
		std::cerr << "Error starting server on port 53000" << std::endl;
		
		return -1;
	}

	std::cout << "Server started on port 53000" << std::endl;
		
	server.Run();
	
    return 0;
}
