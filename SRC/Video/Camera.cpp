#include "Video/Camera.hpp"
#include <iostream>

//====================================================
// CONSTRUCTEUR / DESTRUCTEUR
//====================================================

Camera::Camera(int id)
{
	this->_id = id;
	this->_camera = new cv::VideoCapture(this->_id);
}

Camera::~Camera()
{
	if (this->_camera->isOpened())
		this->_camera->release();
	delete (this->_camera);
}

//====================================================
// GETTERS
//====================================================

int Camera::getId() const 
{
	return (this->_id);
}


Mat & Camera::getFrame()
{
	return (this->_frame);
}

//====================================================
// SETTERS
//====================================================


//====================================================
// FONCTIONS GENERALES POUR LA CAMERA
//====================================================

bool Camera::initCamera()
{
	this->_camera->open(this->_id);
	if (this->_camera->isOpened() == false)
		 {
		 	std::cout << "Cannot opent camera" << std::endl;
		 	return (false);

		 }
	this->_camera->set(cv::CAP_PROP_FOURCC, VideoWriter::fourcc('M', 'J', 'P', 'G'));
	this->initResolutions();
	
}

bool Camera::captureNewFrame()
{
	this->_camera->read(this->_frame);
	return (true);
}



bool Camera::reOpenCamera()
{
	delete (this->_camera);
	this->_camera = new cv::VideoCapture(this->_id);
}

//===================================================
// FONCTIONS MASQUEES D'INITIALISATION
//===================================================

void Camera::initResolutions()
{

	YAMLParser parser = YAMLParser(CAMERA_CONFIGURATION_FILE , FileStorage::READ);

	if (parser.isOpened() == false)
	{
		std::cout << "Cannot open resolutions file : " << CAMERA_CONFIGURATION_FILE << std::endl;
		std::cout << "Using default values." << std::endl;
	}
	else
	{
		int width  	= parser.getValueOf("width");
		int height 	= parser.getValueOf("height");
		int fps 	= parser.getValueOf("fps");

		this->_camera->set(cv::CAP_PROP_FRAME_WIDTH, width);
		this->_camera->set(cv::CAP_PROP_FRAME_HEIGHT,height);
		this->_camera->set(cv::CAP_PROP_FPS, fps);
	}

	this->_width = this->_camera->get(cv::CAP_PROP_FRAME_WIDTH);
	this->_height = this->_camera->get(cv::CAP_PROP_FRAME_HEIGHT);
	this->_fps = this->_camera->get(cv::CAP_PROP_FPS);

	std::cout << "Camera parameters are : " << std::endl;
	std::cout << "FPS    : " << this->_fps << std::endl;
	std::cout << "Width  : " << this->_width << std::endl; 
	std::cout << "Height : " << this->_height << std::endl << std::endl; 
}


int Camera::getWidth()
{
	return (this->_width);
}

int Camera::getHeight()
{
	return (this->_height);
}

int Camera::getFps()
{
	return (this->_fps);
}