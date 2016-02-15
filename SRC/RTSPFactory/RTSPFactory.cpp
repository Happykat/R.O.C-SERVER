#include "RTSPFactory/RTSPFactory.hpp"

RTSPFactory::RTSPFactory()
{
	this->watcher = NULL;
}

RTSPFactory::~RTSPFactory()
{
	if (this->watcher)
		*(this->watcher) = 1;
}


int RTSPFactory::createServer(int cameraId, int port)
{
	threadArguments * args = new threadArguments;

	args->cameraId = cameraId;
	args->port = port;
	args->watcher = 0; 

	this->watcher = &(args->watcher);

	pthread_t thread;

	if (pthread_create(&thread, NULL,  &RTSPFactory::createRTSPServer , args) == -1)
	{
		this->watcher = NULL;
		return (1);
	}

	return (0);
}

bool RTSPFactory::deleteServer()
{
	if (this->watcher)
	{
		*(this->watcher) = 1;
		this->watcher = NULL;
		return (true);
	}
	else
		return (false);
}

void * RTSPFactory::createRTSPServer(void * args_void)
{
	threadArguments * args = static_cast<threadArguments *>(args_void);

	TaskScheduler* taskSchedular = BasicTaskScheduler::createNew();
	BasicUsageEnvironment* usageEnvironment = BasicUsageEnvironment::createNew(*taskSchedular);
	RTSPServer* rtspServer = RTSPServer::createNew(*usageEnvironment, args->port, NULL);

	if(rtspServer == NULL)
	{
		*usageEnvironment << "Failed to create rtsp server ::" << usageEnvironment->getResultMsg() <<"\n";
		exit(1);
	}

	std::string streamName = "camera_" + std::to_string(args->cameraId);
	ServerMediaSession* sms = ServerMediaSession::createNew(*usageEnvironment, streamName.c_str(), streamName.c_str(), "Live H264 Stream");
	H264LiveServerMediaSession *liveSubSession = H264LiveServerMediaSession::createNew(*usageEnvironment, true);
	sms->addSubsession(liveSubSession);
	rtspServer->addServerMediaSession(sms);
	char* url = rtspServer->rtspURL(sms);
	*usageEnvironment << "Play the stream using url "<<url << "\n";
	delete[] url;
	taskSchedular->doEventLoop(&args->watcher);
	return 0;
}