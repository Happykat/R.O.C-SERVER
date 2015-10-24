#include <iostream>
#include <ctime>
#include "Network/TcpServer.hpp"
#include "Network/UdpServer.hpp"
#include "Video/Camera.hpp"

int main(int argc, char**argv)
{
  std::vector<unsigned char> data;
  std::vector<UdpPacket> * packets;
  
  UdpServer server = UdpServer(1250);
  Camera    camera = Camera(0, 30);

  char a;

  if (camera.initCamera() == false)
    exit(0);
  if (server.initServer() == false)
    exit(0);
  
  server.runServer();

  std::cout << "Waiting client ping..." << std::endl;

  server.Read(&a, 1);

  std::cout << "Streaming..." << std::endl;

  while (1)
     {
     
      camera.captureNewFrame();
      camera.compressFrame();
      server.bundleUpData(camera.getCompressedFrame(), 1);
      packets = server.getBundledUpData();
      for (int i = 0; i < packets->size() ; i++)
        server.Send((char *) &((*packets)[i]), sizeof(UdpPacket));
      std::cout << "Sended frame of : " << packets->size() << " packets" << std::endl;
      }
  
  return (0);
}
