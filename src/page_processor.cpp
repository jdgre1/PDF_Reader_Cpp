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
	//cout << "Deleted iD: " << iD << endl;
	if (iD >= 0)
		delete api;


}

//void PageProcessor::setFilesArr(std::filesystem::path imgs_[], int& num_img_s)  
void PageProcessor::setFilesArr(const vector<std::filesystem::path>& files, const int& id)
{
	//tesseract::TessBaseAPI* api = new tesseract::TessBaseAPI();
	api = new tesseract::TessBaseAPI();
	if (api->Init(NULL, "eng")) {
		fprintf(stderr, "Could not initialize tesseract.\n");
		exit(1);
	}

	iD = id;
	cout << "object of id = " << iD << " created" << endl;
	//api = std::make_unique<tesseract::TessBaseAPI>();
	//delete api;
	//vector<std::filesystem::path> vec(files.size());
	m_preprocessParams.imgFiles.resize(files.size());// = vector<std::filesystem::path> vec(files.size());
	m_preprocessParams.imgFiles = files;
	//m_preprocessParams->imgs_ptr = imgs_;
	//m_preprocessParams->numImgs = num_img_s;
}

std::thread PageProcessor::pageThread(PageProcessor::StatusStruct& ss, std::atomic<int>& cntrGuard, std::atomic<int>& dispReady) {
	return std::thread(&PageProcessor::runThread, this, std::ref(ss), std::ref(cntrGuard), std::ref(dispReady));
}

void PageProcessor::runThread(PageProcessor::StatusStruct& ss, std::atomic<int>& cntrGuard, std::atomic<int>& dispReady) {

	for (size_t i = 0; i < m_preprocessParams.imgFiles.size(); i++) {
		m_preprocessParams.file = m_preprocessParams.imgFiles[i];
		correctOrientation();
		dispReady++;
		scanPage(ss);
	}
	// Augment counter-guard variable to indicate thread is finished.
	cntrGuard++;

}

void PageProcessor::correctOrientation()
{
	//const char* inputfile = (m_preprocessParams.file.string()).c_str();
	string temp = m_preprocessParams.file.string();
	const char* inputfile = temp.c_str();
	m_preprocessParams.image = pixRead(inputfile);

	api->SetPageSegMode(tesseract::PSM_AUTO_OSD);
	api->SetImage(m_preprocessParams.image);

	//api->Recognize(0); // too time-consuming and not necessary to determine dpi
	m_preprocessParams.it = api->AnalyseLayout();
	m_preprocessParams.it->Orientation(&m_preprocessParams.orientation, &m_preprocessParams.direction, &m_preprocessParams.order, &m_preprocessParams.deskew_angle);

	// Use OpenCV to read-image and correct rotation:
	float major_rotate_angle = 0.0;
	m_preprocessParams.img_mat = imread(m_preprocessParams.file.string(), IMREAD_COLOR);
	Point2f src_center(m_preprocessParams.img_mat.cols / 2.0F, m_preprocessParams.img_mat.rows / 2.0F);

	if (m_preprocessParams.orientation == 1) {
		major_rotate_angle = -90.0;
	}
	else if (m_preprocessParams.orientation == 2) {
		major_rotate_angle = -180.0;
	}

	else if (m_preprocessParams.orientation == 3) {
		major_rotate_angle = 90.0;
	}

	Mat rot_mat = getRotationMatrix2D(src_center, m_preprocessParams.deskew_angle - major_rotate_angle, 1);
	cv::Rect2f bbox = cv::RotatedRect(cv::Point2f(), m_preprocessParams.img_mat.size(), m_preprocessParams.deskew_angle - major_rotate_angle).boundingRect2f(); //https://stackoverflow.com/questions/22041699/rotate-an-image-without-cropping-in-opencv-in-c
	// adjust transformation matrix
	rot_mat.at<double>(0, 2) += bbox.width / 2.0 - m_preprocessParams.img_mat.cols / 2.0;
	rot_mat.at<double>(1, 2) += bbox.height / 2.0 - m_preprocessParams.img_mat.rows / 2.0;

	cv::Mat dst;
	warpAffine(m_preprocessParams.img_mat, dst, rot_mat, bbox.size());
	//auto api = std::make_unique<tesseract::TessBaseAPI>();
	std::string new_filename = m_preprocessParams.file.string().substr(0, m_preprocessParams.file.string().size() - 4) + "_corrected.jpg";
	imwrite(new_filename, dst);
	currImg = dst;

	/*Size size(int(dst.cols / 2), int(dst.rows / 2));//the dst image size,e.g.100x100
	resize(dst, dst, size);//resize image

	imshow("img_mat", m_preprocessParams.img_mat);
	imshow("dst", dst);
	waitKey();*/

}



void PageProcessor::scanPage(PageProcessor::StatusStruct& ss)
{
	int winH = int(currImg.rows * 0.05);
	int winL = int(currImg.cols * 0.3);
	int startY = 0;// int(image.rows * 0.67);
	int stepSize = 50;
	int disp = int(winH / 2);
	bool lastRow = false;

#if HAS_CUDA

	gpuImg.upload(currImg);
	cv::Scalar mean_pixel_val;
	for (size_t j = startY; j < gpuImg.rows; j += disp) {
		if (gpuImg.rows < (j + winH)) {
			j = gpuImg.rows - winH;
			lastRow = true;
		}

		//cout << "j = " << j << endl;
		bool lastCol = false;

		for (size_t k = 0; k <= (gpuImg.cols - winL); k += stepSize) {
			//cout << "k = " << k << endl;
			if (gpuImg.cols < (k + winL)) {
				k = gpuImg.cols - winL;
				lastCol = true;
			}
			roi = Rect(k, j, winL, winH);
			roiImg = gpuImg(roi);
			cv::cuda::cvtColor(roiImg, grayImg, COLOR_BGR2GRAY);
			grayImg.download(roiMat);
			
			// Check data is actually present in image for reading:
			cv::Scalar mean_pixel_val = cv::mean(roiMat);	
			bool wordFound = false;
			if ((mean_pixel_val[0] < 247) & (mean_pixel_val[0] > 180)) {
				api->SetImage(roiMat.data, roiMat.cols, roiMat.rows, 1, roiMat.step);
				m_outText = string(api->GetUTF8Text());
				m_confidence = api->MeanTextConf();

				for (size_t l = 0; l < sizeof(m_terms) / sizeof(m_terms[0]); l++) {
					
					if (string(m_outText).find(m_terms[l]) != std::string::npos) {
						std::cout << "found term: " << m_terms[l] << "! With confidence: " << m_confidence << endl;
						ss.confidence = m_confidence;
						ss.actual_word = m_terms[l];
						break;
					}
				}

				
				//cout << "id = " << iD << " found m_outText: " << m_outText << " with m_confidence: " << m_confidence << endl;

			}

			/*else {
				cout << "id = " << iD << " has no content to inspect here. " << endl;
			}*/

			
			ss.curr_img = currImg;
			ss.struct_roi = roi;
			ss.wordFound = wordFound;
			

			if (lastCol)
				break;
		}

		if (lastRow)
			break;
	}
#endif

}





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