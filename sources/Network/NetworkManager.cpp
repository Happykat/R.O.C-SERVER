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

  if (this->_server->runServer() == false) {
    logger::log(ERROR_TCP , logger::logType::FAILURE);
    return;
  }

  logger::log(SUCCESS_TCP , logger::logType::SUCCESS);

  while (this->_run)
  {
    read = this->_server->Read(this->_buffer , TCP_BUFFER_SIZE);

    if (read < 0) {
      logger::log("SOCKET ERROR !" , logger::logType::FAILURE);
      return;
    }

    if (read == 0) {
      logger::log("CONNECTION CLOSED ON THE OTHER SIDE" , logger::logType::INFO);
      break;
    }

    {
      logger::log("Readed : " + std::to_string(read) , logger::logType::INFO);
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
