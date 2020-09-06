#pragma once
#ifndef PAGE_PROCESSOR__H__
#define PAGE_PROCESSOR__H__

#include <pch.h>
#include <memory.h>
#include <leptonica/allheaders.h>
#include <tesseract/baseapi.h>
#include <filesystem>

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


	PageProcessor(std::filesystem::path imgs[], int& num_imgs);
	PageProcessor();
	~PageProcessor();
	//void setFilesArr(std::filesystem::path imgs[], int& num_imgs); vector<std::filesystem::path>
	void setFilesArr(const std::vector<std::filesystem::path>&, const int& id);
	void correctOrientation();

	void scanPage();
	std::thread pageThread();
	void runThread();

private:
	PreprocessParams m_preprocessParams;
	//std::unique_ptr<tesseract::TessBaseAPI> api;
	tesseract::TessBaseAPI* api;
	int iD;
	bool notDeleted = true;





};


#endif