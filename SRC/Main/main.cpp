#include <unistd.h>
#include <iostream>
#include "RTSPFactory/RTSPFactory.hpp"

int main(int argc, char**argv)
{
	RTSPFactory * factory = new RTSPFactory();
	int serverId = 0;

	serverId = factory->createServer(0, atoi(argv[1]));
	if (serverId == -1)
	{
		std::cout << "Error while spawning RTSP server" << std::endl;
		exit(0);
	}

	sleep(20);

	return (0);
}
