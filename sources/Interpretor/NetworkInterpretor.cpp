#include "Interpretor/NetworkInterpretor.hpp"

NetworkInterpretor::NetworkInterpretor(Manager * parent)
{
  this->_parent = parent;
  this->_keepAlive = false;
  this->_isAsyncRunning = false;
}

NetworkInterpretor::~NetworkInterpretor()
{
    if (this->_isAsyncRunning == true)
    {
      this->_keepAlive = false;
      waitRunner();
    }
}

void NetworkInterpretor::run()
{
  this->_keepAlive = true;

  std::thread * reader = new std::thread(&NetworkInterpretor::runner , this);
  reader->detach();
}

void NetworkInterpretor::waitRunner()
{
  std::unique_lock<std::mutex> _lock(this->_lockMain);
  while (this->_isAsyncRunning == true)
    this->_conditionMain.wait(_lock);
}

void NetworkInterpretor::runner()
{
  this->_isAsyncRunning = true;
  protocol::Packet * message;

  while (this->_keepAlive == true)
  {
      while (this->_parent->isInputAvailable())
      {
        message = this->_parent->popInput();
        if (this->isValid(message))
        {
          if (this->needRerouting(message))
            this->_parent->pushOutput(message);
          else
            this->handlePacket(message);
        }
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }
  this->_keepAlive = false;
  this->_conditionMain.notify_all();
}


bool NetworkInterpretor::isValid(protocol::Packet * message)
{
    return message->magic() == 0xAF;
}

bool NetworkInterpretor::needRerouting(protocol::Packet * message)
{
  unsigned int destination;
  destination = (this->createMask(0 , 3) & message->header());
  return destination != 2;

}


unsigned NetworkInterpretor::createMask(unsigned a, unsigned b)
{
   unsigned r = 0;
   for (unsigned i=a; i<=b; i++)
       r |= 1 << i;

   return r;
}

void NetworkInterpretor::handlePacket(protocol::Packet * message)
{

  switch (message->id()) {
    case 0x30:
        logger::log("Connection data query" , logger::logType::INFO);
        this->connectionQuery();
      break;
    case 0x31:
        logger::log("Canny query" , logger::logType::INFO);
        this->cannyQuery(message);
    break;
    case 0x32:
      logger::log("Face query" , logger::logType::INFO);
      this->faceQuery(message);
    break;
  }
  delete message;
}

void NetworkInterpretor::connectionQuery()
{
  protocol::Packet * response = new protocol::Packet();

  if (response == NULL)
    return;



}

void NetworkInterpretor::cannyQuery(protocol::Packet * message)
{
  protocol::Processing payload;

  message->payload().UnpackTo(&payload);

  if (payload.action() == protocol::Processing_Action::Processing_Action_ACTIVATE)
    std::cout << "Activate canny" << std::endl;
  else if (payload.action() == protocol::Processing_Action::Processing_Action_DESACTIVATE)
    std::cout << "Desactivate canny" << std::endl;

}

void NetworkInterpretor::faceQuery(protocol::Packet * message)
{

}
