#include <unistd.h>
#include <iostream>
#include "opencv2/opencv.hpp"
#include "opencv2/core/cuda.hpp"
#include "RTSPFactory/RTSPFactory.hpp"
#include "Parser/YAMLParser.hpp"

int main(int argc, char**argv)
{
	RTSPFactory * factory = new RTSPFactory();
	int serverId = 0;

	if (argc < 2)
		return (1);
	
	serverId = factory->createServer(0, atoi(argv[1]));
	if (serverId == -1)
	{
		std::cout << "Error while spawning RTSP server" << std::endl;
		exit(0);
	}

	sleep(300);
	
	std::cout << "Attempting to kill server " << serverId << std::endl;
	factory->deleteServer(serverId);

	return (0);
}
