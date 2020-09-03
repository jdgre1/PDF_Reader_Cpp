#include "opencv_api_calls.h"


using namespace cv;
using namespace std;

bool OPENCV_API_CALLS::check_VideoCaptureWorks()
{
	string video_path = "../../data/dashboardVid.mp4";
	int startFrame = 10650;
	VideoCapture vCap;
	vCap.open(video_path);
	bool success = 0;

	if (!vCap.isOpened()) {
		cout << "Error reading file: Check filepath." << endl;
	}
	else {
		success = vCap.set(CAP_PROP_POS_FRAMES, startFrame);
	}

	return success;
}

bool OPENCV_API_CALLS::check_ObjectTrackerInitialisation(int tracker_index)
{
	string trackerTypes[8] = { "BOOSTING", "MIL", "KCF", "TLD", "MEDIANFLOW", "GOTURN", "MOSSE", "CSRT" };
	string trackerType = trackerTypes[tracker_index];
	cv::Ptr<cv::Tracker> tracker;

	if (trackerType == "BOOSTING")
		tracker = TrackerBoosting::create();
	if (trackerType == "MIL")
		tracker = TrackerMIL::create();
	if (trackerType == "KCF")
		tracker = TrackerKCF::create();
	if (trackerType == "TLD")
		tracker = TrackerTLD::create();
	if (trackerType == "MEDIANFLOW")
		tracker = TrackerMedianFlow::create();
	if (trackerType == "GOTURN")
		tracker = TrackerGOTURN::create();
	if (trackerType == "MOSSE")
		tracker = TrackerMOSSE::create();
	if (trackerType == "CSRT")
		tracker = TrackerCSRT::create();

	return tracker.empty() ? false : true;
}

bool OPENCV_API_CALLS::check_ObjectDetectorInitialisation()
{
	std::string modelTxt = "../../models/MobileNetSSD_deploy.prototxt.txt";
	std::string modelBin = "../../models/MobileNetSSD_deploy.caffemodel";

	try {
		cv::dnn::Net dnnNet = dnn::readNetFromCaffe(modelTxt, modelBin);
		cerr << "loaded successfully" << endl;
		return true;
	}
	catch (cv::Exception& e)
	{
		std::cerr << "loading failed: " << e.what() << std::endl;
		return false;
	}

	return false;
}








