#pragma once
#ifndef PAGE_PROCESSOR__H__
#define PAGE_PROCESSOR__H__

#include <pch.h>

#define HAS_CUDA 1

class PageProcessor {

public:
	struct PreprocessParams {

		// tesseract variables
		
		tesseract::Orientation orientation;
		tesseract::WritingDirection direction;
		tesseract::TextlineOrder order;
		tesseract::PageIterator* it;
		float deskew_angle;
		PIX* image;

		// reading/writing variables
		const std::string folder_dir = "../../data/PDF_imgs/";
		int numImgs;
		std::filesystem::path* imgs_ptr;
		std::filesystem::path file;
		std::string new_filename;
		std::vector<std::filesystem::path> imgFiles;

		// OpenCV variables
		cv::Mat img_mat;
		cv::Mat rot_mat;
		cv::Mat dst;
		
		cv::Point2f src_center;
		cv::Rect2f bbox;
		
		// OpenCV-Cuda morphological operations
		int morph_elem_shape = 0;
		int morphTransformType1 = 0;
		int	morphKernelSize1 = 2;
		int morphTransformType2 = 1;
		int	morphKernelSize2 = 1;
		
		//auto api = std::make_unique<tesseract::TessBaseAPI>();
		
	};

	struct StatusStruct { //struct of roi_rect, currImg, word_found = true/false -> colour of rectangle changes, actual word found, confidence) -
		cv::Rect struct_roi;
		cv::Mat curr_img;
		bool wordFound = false;
		std::string actual_word;
		int confidence;
		bool displayReady = false;
		std::unordered_map <std::string, std::vector<std::string>> termDict;
	};


	// Constructor
	PageProcessor();

	// Rule of 3
	// 1. Destructor
	~PageProcessor();
	// 2. Copy assignment operator
	PageProcessor& operator=(PageProcessor&) = default;
	// 3. Copy constructor
	PageProcessor(const PageProcessor&);        //PageProcessor(const PageProcessor&) = delete; // (if to disable copy constructor) 
	
	void setFilesArr(const std::vector<std::filesystem::path>&, const int& id);
	void correctOrientation();

	void scanPage(StatusStruct& ss, std::mutex& consolePrintGuard, std::atomic<bool>& stopThreads);
	std::thread pageThread(StatusStruct& ss, std::atomic<int>& cntrGuard, std::mutex& consolePrintGuard, std::atomic<bool>& stopThreads);
	void runThread(StatusStruct& ss, std::atomic<int>& cntrGuard, std::mutex& consolePrintGuard, std::atomic<bool>& stopThreads);
	void logResults(const PageProcessor::StatusStruct& ss);
	void saveAndCleanText(PageProcessor::StatusStruct& ss, const std::string& term);
	int detectAndCountNumDigits();
	void extractDigitsfromText(PageProcessor::StatusStruct& ss, const std::string& term);
	void addTermToDict(PageProcessor::StatusStruct& ss, const std::string& term, const std::string& term_digits = NULL);
	Pix* PageProcessor::mat8ToPix(cv::Mat* mat8);
	cv::Mat PageProcessor::pix8ToMat(Pix* pix8);


private:
	PreprocessParams m_preprocessParams;
	//std::unique_ptr<tesseract::TessBaseAPI> api;
	tesseract::TessBaseAPI* m_api;  // https://stackoverflow.com/questions/49454843/different-tesseract-result-for-mat-and-pix - can also create a stack object!!
	int  m_iD;
	bool  m_notDeleted = true;
	cv::Mat m_currImg, m_roiMat;
	cv::cuda::GpuMat  m_gpuImg, m_grayImg, m_roiImg, m_mask;
	std::string m_outText; // = api->GetUTF8Text()
	int m_confidence; // = api->MeanTextConf();
	cv::Rect  m_roi;
	std::string m_terms[19] = { "HTS", "Harmonized Tarif", "Harmonised Tarif", "Harmonized classification", "Customs", "Zolltarif",
				"Classification", "douane", "Douane", "Commodity", "Tarif", "tarif", "Harmonised", "Harmonized", "H.S", "H S", "HS", "Code", "Douanier" };
	cv::Ptr<cv::cuda::Filter> m_morphFilter_1;
	cv::Ptr<cv::cuda::Filter> m_morphFilter_2;
	//bool m_orientation_corrected = false;




};


#endif