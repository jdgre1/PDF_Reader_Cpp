// Code derived from Sloan Kelly tutorial: "https://www.youtube.com/watch?v=y4_mFrTeD_A&t=1084s"
#pragma once
#ifndef server__H__
#define server__H__

#include <ws2tcpip.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/utils/trace.hpp>
#include <chrono>
#include <string>
#include <vector>

//#pragma comment(lib, "ws2_32.lib") //"https://stackoverflow.com/questions/3484434/what-does-pragma-comment-mean"


class Server {



public:  // Prefix "FD" indicates FileDescriptor because all sockets in Unix are file-descriptors

	//typedef void (*MessageReceivedCallback)(Server serverListener, int& socketID, std::string& msg);

	Server(const std::string& ipAddress, const int& port);// , MessageReceivedCallback callBack);

	~Server();

	// return ipAddress:
	static std::string getIpAddress();

	// Initialise winsock
	bool init();

	// The main processing loop
	void run(std::atomic<bool>&);

	// Share frame with client
	void shareFrame(const int& clientSocket, cv::Mat sharedFrame); // copied val of shared frame?

	std::thread serverThread(std::atomic<bool>&);

	void setFrame(const cv::Mat&);

	void setNewFrameReady(bool&&);

	// Cleanup
	void cleanup();

private:

	std::string m_ipAddress;
	int m_port;
	cv::Mat m_frame;
	cv::Mat m_sentFrame;
	bool m_newFrameReady;
	//MessageReceivedCallback m_messageReceived;
	std::vector<SOCKET> m_clients;

	// --- Private Methods ---

	// Create a socket
	SOCKET createSocket();

	// Wait for a connection
	SOCKET waitForConnection(const SOCKET& listener);










};
#endif 