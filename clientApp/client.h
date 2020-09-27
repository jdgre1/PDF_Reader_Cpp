#pragma once
#ifndef carTracker__H__
#define carTracker__H__
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/utils/trace.hpp>
#include <opencv2/tracking/tracker.hpp>
#include <atomic>
#include <vector>
#include <iostream>
#include <mutex>

class CarTracker {

public:
	void declareTracker(const std::string& trackerType);
	void initTracker(const cv::Mat& frame, const cv::Rect2d& trackbox);
	bool updateTracker(const cv::Mat& frame, cv::Rect2d& trackBox);

	void setTracker(const cv::Ptr<cv::Tracker>& tracker_);
	void setTrackerType(const std::string& tracker_type);
	void setId(const int& iD);
	void setCurrImg(const cv::Mat& curr_Img);
	void setTrackersVec(const std::vector<cv::Ptr<cv::Tracker>>& newTrackersVec);

	cv::Mat getCurrImg() const;
	std::string getTrackerType() const;
	cv::Ptr<cv::Tracker> getTracker() const;
	std::vector<cv::Ptr<cv::Tracker>> getTrackersVec() const;


private:

	cv::Ptr<cv::Tracker> m_tracker;
	bool m_trackerExists;
	int m_trackingStatus;
	std::string m_trackerType;
	int m_id;
	cv::Mat m_currImg;

	// Cuda variables:
	std::vector<cv::Ptr<cv::Tracker>> m_trackersVec;








};
#endif 