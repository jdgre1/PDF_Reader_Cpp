#include <pch.h> 
#include <page_processor.h>
using namespace cv;
using namespace std;
using namespace cv::text;
#include <filesystem>

namespace fs = std::filesystem;

cv::Mat pix8ToMat(Pix* pix8);

void RunPDFReader(){

	int processor_count = std::thread::hardware_concurrency();
	//vector<PageProcessor> PageProcessor(processor_count);
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

	for (size_t i = 0; i < processor_count; i++) {
		//std::filesystem::path*  filesArr = &processor_files[i];
		//int arrSize = processor_files[i].size();
		PageProcessors[i].setFilesArr(processor_files[i], int(i));

		//frameThreads[i] = DashboardTrackers[i].dashboardThread(std::ref(imgAvailable), std::ref(stop_threading), std::ref(trackBoxVec), std::ref(trackingStatus), std::ref(lines), std::ref(imgAvailGuard), std::ref(trackStatusGuard), std::ref(trackBoxGuard), std::ref(lanesGuard));
		frameThreads[i] = PageProcessors[i].pageThread();
	}

	for (int m = 0; m < processor_count; m++) {
		frameThreads[m].join();
	}
	delete[] frameThreads;

}


// delete api; (unique pointer used)



Pix* mat8ToPix(cv::Mat* mat8)
{
	Pix* pixd = pixCreate(mat8->size().width, mat8->size().height, 8);
	for (int y = 0; y < mat8->rows; y++) {
		for (int x = 0; x < mat8->cols; x++) {
			pixSetPixel(pixd, x, y, (l_uint32)mat8->at<uchar>(y, x));
		}
	}
	return pixd;
}

cv::Mat pix8ToMat(Pix* pix8)
{
	cv::Mat mat(cv::Size(pix8->w, pix8->h), CV_8UC1);
	uint32_t* line = pix8->data;
	for (uint32_t y = 0; y < pix8->h; ++y) {
		for (uint32_t x = 0; x < pix8->w; ++x) {
			mat.at<uchar>(y, x) = GET_DATA_BYTE(line, x);
		}
		line += pix8->wpl;
	}
	return mat;
}


