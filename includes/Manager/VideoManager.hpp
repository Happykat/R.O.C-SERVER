#ifndef VIDEO_MANAGER_HPP
#define VIDEO_MANAGER_HPP

#include <iostream>
#include <sys/time.h>
#include <ctime>
#include <vector>
#include <thread>
#include <mutex>
#include "Video/Camera.hpp"
#include "Sync/LockList.hpp"
#include "Logger/Logger.hpp"
#include "Parser/ConfigParser.hpp"
#include "Processing/ImgProcessingWrapper.hpp"

class VideoManager {

    public:

        VideoManager();
        ~VideoManager();
        bool isReady();

        cv::Mat & queryFrame(unsigned int id);
        cv::Mat & getFrame(unsigned int id);

        void waitSync();
        timeval getPresentationTime();

        int getWidthById(unsigned int id);
        int getHeightById(unsigned int id);
        int getFpsById(unsigned int id);

        bool setAll(int propId, double value);
        bool setOne(int propId, double value , unsigned int id);

        bool run();
        bool loop();

        ImgProcessingWrapper & getProcessingWrapper();

    private:

        bool init();
        bool uninit();
        bool spawn();

        bool grab(unsigned int id);
        bool retrieve(unsigned int id);

        void grabAll();
        void retrieveAll();
        void flipAll();


    private:

        unsigned int            _camerasCount;
        unsigned int            _timeout;
        unsigned int            _treshold;

        bool                    _ready;
        bool                    _isAlive;
        bool                    _isAsyncRunning;
        timeval                 _presentationTime;

        std::vector<Camera *>   _cameras;
        LockList *              _lockList;
        ImgProcessingWrapper    _processingWrapper;

        std::mutex              _lock;
        std::condition_variable _condition;

};

#endif
