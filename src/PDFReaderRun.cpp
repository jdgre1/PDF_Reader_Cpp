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
	else { 
		// Need to save one processor for the main thread:
		processor_count--;
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

	// main thread to show status of pdf-reading:
	// Access member-variables of each object here: (struct of roi_rect, currImg, word_found = true/false -> colour of rectangle changes, actual word found, confidence) - display that on screen
	// Stitch all current images together to form one display image: 
	bool not_finished = true;
	while (not_finished) {
		for (size_t i = 0; i < processor_count; i++) {

		}
	}




	//imshow("Image", currImg(roi));
	//waitKey(100);

	for (int m = 0; m < processor_count; m++) {
		frameThreads[m].join();
	}
	delete[] frameThreads;

}


// delete api; (unique pointer used)





