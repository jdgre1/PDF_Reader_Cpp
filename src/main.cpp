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

#include <leptonica/allheaders.h>
#include <tesseract/baseapi.h>

using namespace cv;
using namespace std;
using namespace cv::text;


int main(int argc, char* argv[])
{
	//Ptr<OCRTesseract> ocr = OCRTesseract::create();
	
	tesseract::TessBaseAPI* api = new tesseract::TessBaseAPI();
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

	for (int i = 0; i < sizeof(imgs) / sizeof(imgs[0]); i++) {

		std::string file = imgs[i];
		const char* inputfile = file.c_str();
		PIX* image = pixRead(inputfile);

		api->SetPageSegMode(tesseract::PSM_AUTO_OSD);
		api->SetImage(image);
		api->Recognize(0);
		it = api->AnalyseLayout();
		it->Orientation(&orientation, &direction, &order, &deskew_angle);

		cout << "Orientation: " << orientation <<
			"\nWritingDirection: " << direction <<
			"\nTextlineOrder: " << order <<
			"\nDeskew angle: " << deskew_angle << "\n";
	}

	delete api;
	
	
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
	delete api;
	return 0;
}


/* def sliding_window(image, stepSize, windowSize):
	# slide a window across the image
    startY = 0 #int(image.shape[0] * 0.67) #0
    endY = int(image.shape[0]) # int(image.shape[0]*0.46)

    for y in range(startY, endY, stepSize):
        for x in range(0, image.shape[1], stepSize):
            # yield the current window
            yield (x, y, image[y:y + windowSize[1], x:x + windowSize[0]])


for (x, y, window) in sliding_window(img, stepSize=50, windowSize=(winW, winH)):

            # if the window does not meet our desired window size, ignore it
            if window.shape[0] != winH or window.shape[1] != winW:
                continue
*/


