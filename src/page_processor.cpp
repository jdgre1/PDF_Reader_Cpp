#include <page_processor.h> 
using namespace cv;
using namespace std;
using namespace cv::text;

PageProcessor::PageProcessor(std::filesystem::path imgs_[], int& num_imgs)
{
	//api = std::make_unique<tesseract::TessBaseAPI>();
	api = new tesseract::TessBaseAPI();
	//m_preprocessParams->imgs_ptr = imgs_;
	//m_preprocessParams->numImgs = num_imgs;
}
PageProcessor::PageProcessor() {
	//m_preprocessParams->api = std::make_unique<tesseract::TessBaseAPI>();
	//m_preprocessParams = PageProcessor::PreprocessParams();
}

PageProcessor::~PageProcessor()
{
	delete api;
}

//void PageProcessor::setFilesArr(std::filesystem::path imgs_[], int& num_img_s)  
void PageProcessor::setFilesArr(vector<std::filesystem::path>& files)
{
	//tesseract::TessBaseAPI* api = new tesseract::TessBaseAPI();
	api = new tesseract::TessBaseAPI();
	//api = std::make_unique<tesseract::TessBaseAPI>();
	//delete api;
	//vector<std::filesystem::path> vec(files.size());
	m_preprocessParams.imgFiles.resize(files.size());// = vector<std::filesystem::path> vec(files.size());
	m_preprocessParams.imgFiles = files;
	//m_preprocessParams->imgs_ptr = imgs_;
	//m_preprocessParams->numImgs = num_img_s;
}

std::thread PageProcessor::pageThread() {
	return std::thread(&PageProcessor::runThread, this);
}

void PageProcessor::runThread() {
	this->~PageProcessor();
}

void PageProcessor::correctOrientation()
{
	/*
	string imgs[6] = { m_preprocessParams->folder_dir + "collections-0.jpg", m_preprocessParams->folder_dir + "collections-1.jpg", m_preprocessParams->folder_dir + "collections-2.jpg", 
		m_preprocessParams->folder_dir + "collections-3.jpg", m_preprocessParams->folder_dir + "collections-4.jpg", m_preprocessParams->folder_dir + "collections-5.jpg" };
	
	//string imgs[] = &(m_preprocessParams->imgs_ptr);

	for (int i = 0; i < m_preprocessParams->numImgs; i++) {

		m_preprocessParams->file = *(m_preprocessParams->imgs_ptr + i);
		*/


		/*
		std::string file = imgs[i];
		const char* inputfile = file.c_str();
		m_preprocessParams->image = pixRead(inputfile);

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
		auto api = std::make_unique<tesseract::TessBaseAPI>();new_filename = imgs[i].substr(0, imgs[i].size() - 4) + "_corrected.jpg";
		imwrite(new_filename, dst);

		Size size(int(dst.cols / 2), int(dst.rows / 2));//the dst image size,e.g.100x100
		resize(dst, dst, size);//resize image

		imshow("img_mat", img_mat);
		imshow("dst", dst);
		waitKey();*/
	//}


}

void PageProcessor::scanPage()
{
	/*
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
}


