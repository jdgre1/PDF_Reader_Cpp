#include <pch.h> 
#include <page_processor.h>
using namespace cv;
using namespace std;
using namespace cv::text;
#include <filesystem>
//#include "C:/Users/gregg/Documents/ImageMagick-Windows-master/ImageMagick-Windows-master/ImageMagick/Magick++/lib/Magick++.h"

//#include <MagickCore/MagickCore.h>
#include <Magick++/lib/Magick++.h> 
namespace fs = std::filesystem;
using namespace Magick;
cv::Mat pix8ToMat(Pix* pix8);

void RunPDFReader(){





	int processor_count = std::thread::hardware_concurrency();
	//vector<PageProcessor> PageProcessor(processor_count);
	std::string load_path = "../../data/";
	std::string save_path = "../../data/PDF_imgs/";
	int num_files = 0;
	for (const auto& entry : fs::directory_iterator(load_path)) {
		//std::cout << entry.path() << std::endl;
		num_files++;
	}

	int files_per_thread = int(processor_count / num_files);
	if (files_per_thread < processor_count) {
		processor_count = files_per_thread;
	}


	int remainder_files = processor_count % num_files;

	// Create threads based on number of processors:
	for (size_t i = 0; i < processor_count; i++) {


	}


	//tesseract::TessBaseAPI* api = new tesseract::TessBaseAPI(); // No need to delete with unique pointer assigned 
	//auto api = std::make_unique<tesseract::TessBaseAPI>();
	unique_ptr<tesseract::TessBaseAPI> api = std::make_unique<tesseract::TessBaseAPI>();
	// Initialize tesseract-ocr with English, without specifying tessdata path
	if (api->Init(NULL, "eng")) {
		cerr << "Could not initialize tesseract.\n";
		exit(2);
	}

	//const char* inputfile = argv[1];
	tesseract::Orientation orientation;
	tesseract::WritingDirection direction;
	tesseract::TextlineOrder order;
	tesseract::PageIterator* it;
	float deskew_angle;
	std::string file;
	std::string folder_dir = "../../data/PDF_imgs/";
	std::string imgs[6] = { folder_dir + "collections-0.jpg", folder_dir + "collections-1.jpg", folder_dir + "collections-2.jpg", folder_dir + "collections-3.jpg", folder_dir + "collections-4.jpg", folder_dir + "collections-5.jpg" };
	float major_rotate_angle;
	Mat img_mat;
	Mat rot_mat;
	PIX* image;
	std::string new_filename;
	// 1. Correct orientation
	for (int i = 0; i < sizeof(imgs) / sizeof(imgs[0]); i++) {

		std::string file = imgs[i];
		const char* inputfile = file.c_str();
		image = pixRead(inputfile);

		api->SetPageSegMode(tesseract::PSM_AUTO_OSD);
		api->SetImage(image);
		//api->Recognize(0); // too time-consuming and not necessary to determine dpi
		it = api->AnalyseLayout();
		it->Orientation(&orientation, &direction, &order, &deskew_angle);

		cout << "Orientation: " << orientation <<
			"\nWritingDirection: " << direction <<
			"\nTextlineOrder: " << order <<
			"\nDeskew angle: " << deskew_angle << "\n";

		// Use OpenCV to read-image and correct rotation:
		major_rotate_angle = 0;
		img_mat = imread(imgs[i], IMREAD_COLOR);
		Point2f src_center(img_mat.cols / 2.0F, img_mat.rows / 2.0F);

		if (orientation == 1) {
			major_rotate_angle = -90;
		}
		else if (orientation == 2) {
			major_rotate_angle = -180;
		}

		else if (orientation == 3) {
			major_rotate_angle = 90;
		}

		Mat rot_mat = getRotationMatrix2D(src_center, deskew_angle - major_rotate_angle, 1);
		cv::Rect2f bbox = cv::RotatedRect(cv::Point2f(), img_mat.size(), deskew_angle - major_rotate_angle).boundingRect2f(); //https://stackoverflow.com/questions/22041699/rotate-an-image-without-cropping-in-opencv-in-c
		// adjust transformation matrix
		rot_mat.at<double>(0, 2) += bbox.width / 2.0 - img_mat.cols / 2.0;
		rot_mat.at<double>(1, 2) += bbox.height / 2.0 - img_mat.rows / 2.0;

		cv::Mat dst;
		warpAffine(img_mat, dst, rot_mat, bbox.size());
		new_filename = imgs[i].substr(0, imgs[i].size() - 4) + "_corrected.jpg";
		imwrite(new_filename, dst);
		
		Size size(int(dst.cols/2), int(dst.rows / 2));//the dst image size,e.g.100x100
		resize(dst, dst, size);//resize image

		imshow("img_mat", img_mat);
		imshow("dst", dst);
		waitKey();
	}
	return;
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


