#pragma once
#ifndef PAGE_PROCESSOR__H__
#define PAGE_PROCESSOR__H__

#include <pch.h>

class PageProcessor {

public:
	struct PreprocessParams {

		// tesseract variables
		unique_ptr<tesseract::TessBaseAPI> api; 
		tesseract::Orientation orientation;
		tesseract::WritingDirection direction;
		tesseract::TextlineOrder order;
		tesseract::PageIterator* it;
		PIX* image;

		// reading/writing variables
		const std::string folder_dir = "../../data/PDF_imgs/";
		int numImgs;
		std::string* imgs_ptr;
		std::string file;
		std::string new_filename;

		// opencv variables
		cv::Mat img_mat;
		cv::Mat rot_mat;
		cv::Mat dst;
		cv::Point2f src_center;
		cv::Rect2f bbox;
		//auto api = std::make_unique<tesseract::TessBaseAPI>();

	};


	PageProcessor(std::string imgs[], int& num_imgs);
	void correctOrientation();
	void scanPage();

private:
	PreprocessParams* m_preprocessParams;







};















#endif