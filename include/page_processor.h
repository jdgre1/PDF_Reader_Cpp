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

		// opencv variables
		cv::Mat img_mat;
		cv::Mat rot_mat;
		cv::Mat dst;
		
		cv::Point2f src_center;
		cv::Rect2f bbox;
		
		//auto api = std::make_unique<tesseract::TessBaseAPI>();
		
	};

	struct StatusStruct { //struct of roi_rect, currImg, word_found = true/false -> colour of rectangle changes, actual word found, confidence) -
		cv::Rect struct_roi;
		cv::Mat curr_img;
		bool wordFound = false;
		std::string actual_word;
		int confidence;
	};

	PageProcessor(std::filesystem::path imgs[], int& num_imgs);
	PageProcessor();
	~PageProcessor();
	//void setFilesArr(std::filesystem::path imgs[], int& num_imgs); vector<std::filesystem::path>
	void setFilesArr(const std::vector<std::filesystem::path>&, const int& id);
	void correctOrientation();

	void scanPage(StatusStruct& ss, std::mutex& consolePrintGuard);
	std::thread pageThread(StatusStruct& ss, std::atomic<int>& cntrGuard, std::atomic<int>& dispReadyGuard, std::mutex& consolePrintGuard);
	void runThread(StatusStruct& ss, std::atomic<int>& cntrGuard, std::atomic<int>& dispReadyGuard, std::mutex& consolePrintGuard);

private:
	PreprocessParams m_preprocessParams;
	//std::unique_ptr<tesseract::TessBaseAPI> api;
	tesseract::TessBaseAPI* api;  // https://stackoverflow.com/questions/49454843/different-tesseract-result-for-mat-and-pix - can also create a stack object!!
	int iD;
	bool notDeleted = true;
	cv::Mat currImg, roiMat;
	cv::cuda::GpuMat gpuImg, grayImg, roiImg;
	std::string m_outText; // = api->GetUTF8Text()
	int m_confidence; // = api->MeanTextConf();
	cv::Rect roi;
	std::string m_terms[19] = { "HTS", "Harmonized Tarif", "Harmonised Tarif", "Harmonized classification", "Customs", "Zolltarif",
				"Classification", "douane", "Douane", "Commodity", "Tarif", "tarif", "Harmonised", "Harmonized", "H.S", "H S", "HS", "Code", "Douanier" };
	//bool m_orientation_corrected = false;




};


#endif