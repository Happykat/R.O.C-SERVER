#include "Processing/EyeDetect.hpp"

using namespace cv;
using namespace cv::cuda;

EyeDetect::EyeDetect(int scale) : ImgProcessing(processingType::EYEDETECT) {
    this->_scale = scale;
    std::string cascadeName = "assets/cascades/haarcascades/haarcascade_eye.xml";
    if (!this->_cascade.load(cascadeName)) {
        this->_cascadeIsLoaded = false;
    } else {
        this->_cascadeIsLoaded = true;
    }
    #ifdef ROC_WITH_CUDA

    #endif
}

EyeDetect::~EyeDetect() {

}

void EyeDetect::applyCpu(cv::Mat &image) {
    if (this->_cascadeIsLoaded)
        detectAndDraw(image);
}


void EyeDetect::detectAndDraw(cv::Mat &img) {
    double t = 0;
    std::vector<cv::Rect> faces, faces2;
    const static cv::Scalar colors[] =
            {
                    cv::Scalar(255, 0, 0),
                    cv::Scalar(255, 128, 0),
                    cv::Scalar(255, 255, 0),
                    cv::Scalar(0, 255, 0),
                    cv::Scalar(0, 128, 255),
                    cv::Scalar(0, 255, 255),
                    cv::Scalar(0, 0, 255),
                    cv::Scalar(255, 0, 255)
            };
    cv::Mat gray, smallImg;

    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
    double fx = 1 / this->_scale;
    resize(gray, smallImg, cv::Size(), fx, fx, cv::INTER_LINEAR);
    equalizeHist(smallImg, smallImg);

    this->_cascade.detectMultiScale(smallImg, faces,
                                    1.1, 2, 0
                                            | cv::CASCADE_SCALE_IMAGE,
                                    cv::Size(30, 30));

    t = (double) cvGetTickCount() - t;
    for (size_t i = 0; i < faces.size(); i++) {
        cv::Rect r = faces[i];
        cv::Mat smallImgROI;
        std::vector<cv::Rect> nestedObjects;
        cv::Point center;
        cv::Scalar color = colors[i % 8];
        int radius;

        double aspect_ratio = (double) r.width / r.height;
            cv::rectangle(img, cvPoint(cvRound(r.x * this->_scale), cvRound(r.y * this->_scale)),
                          cvPoint(cvRound((r.x + r.width - 1) * this->_scale), cvRound((r.y + r.height - 1) * this->_scale)),
                          color, 3, 8, 0);
    }
}

void EyeDetect::applyGpu(cv::Mat &image) {
  applyCpu(image);
}


processingType EyeDetect::getID() const {
    return (processingType::EYEDETECT);
}

void EyeDetect::displayType() {
    std::cout << "EYE_DETECT" << std::endl;
}
