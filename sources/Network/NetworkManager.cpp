#include "Network/NetworkManager.hpp"

NetworkManager::NetworkManager(Manager * parent , unsigned short port) {
    this->_server = new TcpServer(port);
    this->_parent = parent;
    this->_run = false;
    this->_isAsyncRunning = false;
}

NetworkManager::~NetworkManager() {
  this->waitRunner();
  logger::log(STOP_NETWORK_JOB , logger::logType::SUCCESS);
  if (this->_server)
    delete this->_server;
}

bool NetworkManager::init() {
  return (this->_server != NULL && this->_parent != NULL && this->_server->initServer() == true);
}

void NetworkManager::run() {
  this->_run = true;
  std::thread * thread = new std::thread(&NetworkManager::runner , this);
	thread->detach();
}

void NetworkManager::runner() {
  this->_isAsyncRunning = true;
    size_t read;

    while (this->_run) {
        if (this->_server->runServer() == false) {
            logger::log(ERROR_TCP , logger::logType::FAILURE);
            return;
        }

        logger::log(SUCCESS_TCP_CLIENT , logger::logType::SUCCESS);
        while (this->_run)
        {
            read = this->_server->Read(this->_buffer , TCP_BUFFER_SIZE);
            if (read <= 0)
            {
                logger::log(WARNING_TCP_DISCONNECTED , logger::logType::WARNING);
                this->_server->discardClient();
                break;
            }

            this->_parent->pushInput(NetworkInterface::get(this->_buffer , read));
            logger::log("Added a message !" , logger::logType::SUCCESS);
            while (this->_parent->isOutputAvailable())
            {
              protocol::Packet * elem = this->_parent->popOutput();
              NetworkInterface::put(elem , this->_buffer);
              this->_server->Send(this->_buffer  , elem->ByteSize());
            }

            }
    }
    this->_isAsyncRunning = false;
  }

void NetworkManager::waitRunner() {

  std::unique_lock<std::mutex> lock(this->_lock);

  if (this->_run)
    return;

  this->_run = false;

  while (this->_isAsyncRunning)
    this->_condition.wait(lock);

}
