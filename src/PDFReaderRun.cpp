#include <pch.h> 
#include <page_processor.h>
using namespace cv;
using namespace std;
using namespace cv::text;
#include <filesystem>


namespace fs = std::filesystem;

cv::Mat pix8ToMat(Pix* pix8);
bool getDesktopResolution(int& screenHeight, int& screenWidth);
void stitchImgTogether(size_t& i, Mat& aggImg, Mat& temp, int& vertStack, int& horizStack, vector<Mat>& row2_imgs, bool lastRow = false);

void RunPDFReader() {
	int maxThreads = 6;
	int processor_count = std::thread::hardware_concurrency();
	std::string load_path = "../../data/PDF_imgs/";

	int remainder_files = 0;
	vector<std::filesystem::path> files;
	for (const auto& entry : fs::directory_iterator(load_path)) {

		// Avoid previously processed data 
		if ((entry.path().string()).find("_corrected") == std::string::npos) {
			files.push_back(entry.path());
		}
	}

	int num_files = files.size();
	if (processor_count > maxThreads) {
		processor_count = maxThreads;
	}

	if (num_files < processor_count) {
		processor_count = num_files;
	}


	vector < vector<std::filesystem::path>> processor_files(processor_count);

	// Add files to each processor depending on number of files to be processed
	int j = 0;
	for (size_t i = 0; i < num_files; i++) {
		processor_files[j].push_back(files[i]);
		j++;
		j = (j == processor_count) ? 0 : j;
	}

	std::filesystem::path* filesArr[5];
	PageProcessor pp;// = DashboardTracker();
	vector<PageProcessor> PageProcessors(processor_count, pp);
	thread* frameThreads{ new thread[processor_count] };
	PageProcessor::StatusStruct ss;
	vector<PageProcessor::StatusStruct> statusStructs(processor_count, ss);
	int finishedCounter = 0;
	std::atomic<int> counterGuard;
	std::atomic<int> dispReadyGuard;
	mutex consolePrintGuard;

	for (size_t i = 0; i < processor_count; i++) {
		//std::filesystem::path*  filesArr = &processor_files[i];
		//int arrSize = processor_files[i].size();
		PageProcessors[i].setFilesArr(processor_files[i], int(i));

		frameThreads[i] = PageProcessors[i].pageThread(std::ref(statusStructs[i]), std::ref(counterGuard), std::ref(dispReadyGuard), std::ref(consolePrintGuard));
	}

	// main thread to show status of pdf-reading:
	// Access member-variables of each object here: (struct of roi_rect, currImg, word_found = true/false -> colour of rectangle changes, actual word found, confidence) - display that on screen
	// Stitch all current images together to form one display image: 
	bool not_finished = true;
	string display_string;
	Mat currImg_;
	bool wordFound_;
	int confidence_;
	Rect roi_;
	string id_string;
	int screenWidth = 0;
	int screenHeight = 0;
	getDesktopResolution(screenWidth, screenHeight);
	int area;
	int area_per_img;
	//int num_imgs = processor_count;

	// Determine image layout
	int verticalStack = (processor_count > 2) ? 2 : 1;
	int horizontalStack = (processor_count > 4) ? 3 : 2;
	int maxHeight = (verticalStack == 1) ? screenHeight : int(screenHeight / 2);
	int maxWidth = (horizontalStack == 2) ? int(screenWidth / 2) : int(screenWidth / 3);
	Mat temp;
	float ratioHeight;
	float ratioWidth;

	// Determine image display-size:
	
	float resizeFactor = (maxThreads < 7 & maxThreads > 3) ? 0.1 : 0.4;
	
	//int area_per_img = (screenWidth * screenHeight / verticalStack * horizontalStack);
	bool lastRow = false;
	bool displayReady;
	bool firstRound = true;
	while (not_finished) {
		if (firstRound) {
			displayReady = true;
			for (size_t h = 0; h < statusStructs.size(); h++) {
				displayReady = (displayReady & statusStructs[h].displayReady);
			}
		}

		if (displayReady) {
			Mat aggregateImg;
			vector<Mat> row2_imgs;
			firstRound = false;
			for (size_t i = 0; i < processor_count; i++) {
				currImg_ = statusStructs[i].curr_img.clone();
				if (statusStructs[i].struct_roi.x != 0 | statusStructs[i].struct_roi.y != 0) {
					roi_ = statusStructs[i].struct_roi;
					id_string = to_string(i);

					if (wordFound_ = statusStructs[i].wordFound) {
						confidence_ = statusStructs[i].confidence;
						display_string = "ID = " + id_string + ", " + statusStructs[i].actual_word + ": Confidence = " + to_string(confidence_);
						putText(currImg_, display_string, Point(roi_.x, roi_.y), FONT_HERSHEY_SIMPLEX, 2, Scalar(0, 255, 0), 2);
						rectangle(currImg_, roi_, Scalar(0, 255, 0), 3);

					}
					else {
						display_string = "ID = " + id_string + ", No word found.";
						putText(currImg_, display_string, Point(roi_.x, roi_.y), FONT_HERSHEY_SIMPLEX, 2, Scalar(0, 0, 255), 2);
						rectangle(currImg_, roi_, Scalar(0, 0, 255), 3);
					}
				}
				if (currImg_.rows < currImg_.cols) {
					float major_rotate_angle = 90.0;
					Point2f src_center(currImg_.cols / 2.0F, currImg_.rows / 2.0F);
					Mat rot_mat = getRotationMatrix2D(src_center, major_rotate_angle, 1);
					cv::Rect2f bbox = cv::RotatedRect(cv::Point2f(), currImg_.size(), major_rotate_angle).boundingRect2f(); //https://stackoverflow.com/questions/22041699/rotate-an-image-without-cropping-in-opencv-in-c
					// adjust transformation matrix
					rot_mat.at<double>(0, 2) += bbox.width / 2.0 - currImg_.cols / 2.0;
					rot_mat.at<double>(1, 2) += bbox.height / 2.0 - currImg_.rows / 2.0;

					cv::Mat dst;
					warpAffine(currImg_, dst, rot_mat, bbox.size());
					currImg_ = dst;
				}

				float ratioHeight = currImg_.rows / maxHeight;
				float ratioWidth = currImg_.cols / maxWidth;

				(ratioHeight < ratioWidth) ? cv::resize(currImg_, temp, cv::Size(), ratioHeight, ratioHeight) : cv::resize(currImg_, temp, cv::Size(), ratioWidth, ratioWidth);

				lastRow = (i == processor_count - 1) ? true : false;
				if (i > 2) {
					row2_imgs.push_back(temp);
				}

				stitchImgTogether(i, aggregateImg, temp, verticalStack, horizontalStack, row2_imgs, lastRow);

			}

	
			cv::resize(aggregateImg, temp, cv::Size(), resizeFactor, resizeFactor);
			imshow("Aggregate Image", temp);
			waitKey(100);
			this_thread::sleep_for(chrono::milliseconds(10));
			not_finished = (counterGuard == processor_count) ? false : true;
		}

		else {
			this_thread::sleep_for(chrono::milliseconds(100));
		}
	}

	for (int m = 0; m < processor_count; m++) {
		frameThreads[m].join();
	}
	delete[] frameThreads;
}



void stitchImgTogether(size_t& i, Mat& aggImg, Mat& temp, int& vertStack, int& horizStack, vector<Mat>& row2_imgs, bool lastRow) {
	if (i == 0) {
		aggImg = temp;
		return;
	}

	else if (i < 3) {

		if (aggImg.rows != temp.rows) {
			int numRows = min(aggImg.rows, temp.rows);
			aggImg.resize(numRows);
			temp.resize(numRows);
		}
		hconcat(aggImg, temp, aggImg);//Syntax-> hconcat(source1,source2,destination);
		return;
	}

	Mat lowerImg;
	if (lastRow & row2_imgs.size() > 0)
	{
		lowerImg = row2_imgs[0];
		for (int j = 1; j < row2_imgs.size(); j++) {
			hconcat(lowerImg, row2_imgs[j], lowerImg);
		}
		Mat new_image;
		if (i == 4) {
			Mat new_image(Size(row2_imgs[0].cols, row2_imgs[0].rows), lowerImg.type(), Scalar(255, 255, 255));
			hconcat(lowerImg, new_image, lowerImg);
		}



		if (aggImg.cols != lowerImg.cols) {
			int numCols = min(aggImg.cols, lowerImg.cols);
			resize(aggImg, aggImg, Size(numCols, aggImg.rows));
			resize(lowerImg, lowerImg, Size(numCols, lowerImg.rows));
		}

		vconcat(aggImg, lowerImg, aggImg);

	}

}



bool getDesktopResolution(int& screenWidth, int& screenHeight) {

	RECT desktop;
	// Get a handle to the desktop window
	const HWND hDesktop = GetDesktopWindow();
	// Get the size of screen to the variable desktop
	GetWindowRect(hDesktop, &desktop);
	// The top left corner will have coordinates (0,0)
	// and the bottom right corner will have coordinates
	// (horizontal, vertical)
	screenWidth = desktop.right;
	screenHeight = desktop.bottom;

	return (desktop.right > 0 & desktop.bottom > 0) ? true : false;
}



