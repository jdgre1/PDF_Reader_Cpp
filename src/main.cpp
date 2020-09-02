#include <iostream>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <direct.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/utils/trace.hpp>
#include <opencv2/text/ocr.hpp>
#include <memory.h>
#include <leptonica/allheaders.h>
#include <tesseract/baseapi.h>

using namespace cv;
using namespace std;
using namespace cv::text;

cv::Mat pix8ToMat(Pix* pix8);


int main(int argc, char* argv[])
{
	//tesseract::TessBaseAPI* api = new tesseract::TessBaseAPI(); // No need to delete with unique pointer assigned 
	auto api = std::make_unique<tesseract::TessBaseAPI>();

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
		Size size(int(dst.cols/2), int(dst.rows / 2));//the dst image size,e.g.100x100
		resize(dst, dst, size);//resize image

		imshow("img_mat", img_mat);
		imshow("dst", dst);
		waitKey();

	}

	return 0;
}

/*
// delete api; (unique pointer used)


for (int i = 0; i < sizeof(imgs)/ sizeof(imgs[0]); i++) {

	Mat image;
	image = imread(imgs[i], IMREAD_COLOR);
	int winH = int(image.rows * 0.05);
	int winL = int(image.cols * 0.3);
	int startY = 0;// int(image.rows * 0.67);
	int stepSize = 50;
	int disp = int(winH / 2);
	bool lastRow = false;
	for (size_t j = startY; j < image.rows; j += disp) {
		if (image.rows < (j + winH)) {
			j = image.rows - winH;
			lastRow = true;
		}

		cout << "j = " << j << endl;
		bool lastCol = false;

		for (size_t k = 0; k <= (image.cols - winL); k += stepSize) {
			cout << "k = " << k << endl;
			if (image.cols < (k + winL)) {
				k = image.cols - winL;
				lastCol = true;
			}
			cv::Rect roi = Rect(k, j, winL, winH);
			imshow("Image", image(roi));
			waitKey(100);

			if (lastCol)
				break;
		}

		if (lastRow)
			break;
	}

	//imshow("Image", image);
	//waitKey();
}
//delete api;
return 0;
}
*/


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


