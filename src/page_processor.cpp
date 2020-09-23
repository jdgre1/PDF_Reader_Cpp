#include <page_processor.h> 
#include <fstream>
using namespace cv;
using namespace std;
using namespace cv::text;


PageProcessor::PageProcessor() {

	m_api = new tesseract::TessBaseAPI();
	if (m_api->Init(NULL, "eng")) {
		fprintf(stderr, "Could not initialize tesseract.\n");
		exit(1);
	}
	m_api->SetDebugVariable("debug_file", "/dev/null");
}

PageProcessor::PageProcessor(const PageProcessor& copy) // Implemented copy constructor
{
	m_api = new tesseract::TessBaseAPI();
	if (m_api->Init(NULL, "eng")) {
		fprintf(stderr, "Could not initialize tesseract.\n");
		exit(1);
	}
	m_api->SetDebugVariable("debug_file", "/dev/null");
	//cout << "m_api was copied to address " << &m_api << endl;
}


PageProcessor::~PageProcessor()
{
	//cout << "Deleted iD: " << iD << endl;
	if (m_iD >= 0)
		delete  m_api;


}

void PageProcessor::setFilesArr(const vector<std::filesystem::path>& files, const int& id)
{
	m_iD = id;
	cout << "object of id = " << m_iD << " created" << endl;

	m_preprocessParams.imgFiles.resize(files.size());// = vector<std::filesystem::path> vec(files.size());
	m_preprocessParams.imgFiles = files;


#if HAS_CUDA
	Mat str_element = getStructuringElement(MORPH_RECT, Size(2 * m_preprocessParams.morphKernelSize1 + 1, 2 * m_preprocessParams.morphKernelSize1 + 1), Point(m_preprocessParams.morphKernelSize1, m_preprocessParams.morphKernelSize1));
	m_morphFilter_1 = cuda::createMorphologyFilter(m_preprocessParams.morphTransformType1, CV_8U, str_element, cv::Size(m_preprocessParams.morphKernelSize1, m_preprocessParams.morphKernelSize1));
	str_element = getStructuringElement(MORPH_RECT, Size(2 * m_preprocessParams.morphKernelSize2 + 1, 2 * m_preprocessParams.morphKernelSize2 + 1), Point(m_preprocessParams.morphKernelSize2, m_preprocessParams.morphKernelSize2));
	m_morphFilter_2 = cuda::createMorphologyFilter(m_preprocessParams.morphTransformType2, CV_8U, str_element, cv::Size(m_preprocessParams.morphKernelSize2, m_preprocessParams.morphKernelSize2));
#endif
}

std::thread PageProcessor::pageThread(PageProcessor::StatusStruct& ss, std::atomic<int>& cntrGuard, mutex& consolePrintGuard, std::atomic<bool>& stopThreads) {
	return std::thread(&PageProcessor::runThread, this, std::ref(ss), std::ref(cntrGuard), std::ref(consolePrintGuard), std::ref(stopThreads));
}

void PageProcessor::runThread(PageProcessor::StatusStruct& ss, std::atomic<int>& cntrGuard, mutex& consolePrintGuard, std::atomic<bool>& stopThreads) {

	// Run through list of images
	for (size_t i = 0; i < m_preprocessParams.imgFiles.size(); i++) {
		//for (size_t i = 0; i < 1; i++) {
		m_preprocessParams.file = m_preprocessParams.imgFiles[i];
		correctOrientation();
		ss.displayReady = true;
		scanPage(ss, consolePrintGuard, stopThreads);
	}

	// Evaluate all recorded data and save most likely data to textfile:
	logResults(ss);

	// Augment counter-guard variable to indicate thread is finished:
	cntrGuard++;

}

void PageProcessor::logResults(const PageProcessor::StatusStruct& ss) {
	
	std::ofstream myfile;
	string fileName = "../../data/terms_ID_" + to_string(m_iD) + ".txt";
	myfile.open(fileName);
	
	int j = 0;
	unordered_map<std::string, std::vector<std::string>>::iterator it;
	std::pair<string, int> newEntry;

	//for (it = ss.termDict.begin(); it != ss.termDict.end(); it++){
	// The syntax with : it's a range based for. It means that loop will parse each element inside unordered map.
    // Inside the for, you need to specify the alias auto& in order to avoid creating a copy of the elements (assignment operator)
	for (auto& it : ss.termDict) { 
		myfile << "-----Term-----: " << it.first << endl;

		std::unordered_map<cv::String, int> termsWeightedDict_temp;
		for (size_t i = 0; i < it.second.size(); i++) {
			
			if (termsWeightedDict_temp.find(it.second[i]) == termsWeightedDict_temp.end()) {
				// not found
				newEntry.first = it.second[i];
				newEntry.second = 1;
				termsWeightedDict_temp.insert(newEntry);
			}
			else {
				// found
				termsWeightedDict_temp[it.second[i]]++;
			}
		}
		j++;

		// lambda expression to convert unordered_map to vector 
		std::vector<std::pair<string, int>> elems;
		for_each(termsWeightedDict_temp.begin(), termsWeightedDict_temp.end(), [&elems](const std::pair<string, int>& entry) {elems.push_back(entry); });

		// Sort hits in descending order
		std::sort(elems.begin(), elems.end(), [](pair<string, int> a, pair<string, int> b) { return a.second > b.second; });

		for (std::vector<pair<string, int>>::iterator it2 = std::begin(elems); it2 != std::end(elems); ++it2) {
			myfile << it2->first << ": " << it2->second << endl;
		}
		myfile.close();
	}

}


void PageProcessor::correctOrientation()
{
	//const char* inputfile = (m_preprocessParams.file.string()).c_str();
	string temp = m_preprocessParams.file.string();
	const char* inputfile = temp.c_str();
	m_preprocessParams.image = pixRead(inputfile);

	m_api->SetPageSegMode(tesseract::PSM_AUTO_OSD);
	m_api->SetImage(m_preprocessParams.image);

	//api->Recognize(0); // too time-consuming and not necessary to determine dpi
	m_preprocessParams.it = m_api->AnalyseLayout();
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

	Mat rot_mat = getRotationMatrix2D(src_center, float(m_preprocessParams.deskew_angle) - major_rotate_angle, 1);
	cv::Rect2f bbox = cv::RotatedRect(cv::Point2f(), m_preprocessParams.img_mat.size(), m_preprocessParams.deskew_angle - major_rotate_angle).boundingRect2f(); //https://stackoverflow.com/questions/22041699/rotate-an-image-without-cropping-in-opencv-in-c
	// adjust transformation matrix
	rot_mat.at<double>(0, 2) += bbox.width / 2.0 - m_preprocessParams.img_mat.cols / 2.0;
	rot_mat.at<double>(1, 2) += bbox.height / 2.0 - m_preprocessParams.img_mat.rows / 2.0;

	cv::Mat dst;
	warpAffine(m_preprocessParams.img_mat, dst, rot_mat, bbox.size());
	//auto api = std::make_unique<tesseract::TessBaseAPI>();
	std::string new_filename = m_preprocessParams.file.string().substr(0, m_preprocessParams.file.string().size() - 4) + "_corrected.jpg";
	imwrite(new_filename, dst);
	m_currImg = dst;

}

void PageProcessor::scanPage(PageProcessor::StatusStruct& ss, std::mutex& consolePrintGuard, std::atomic<bool>& stopThreads)
{
	int winH = int(m_currImg.rows * 0.05);
	int winL = int(m_currImg.cols * 0.3);
	int startY = 0; // int(m_currImg.rows * 0.65); //0
	int startX = 0; // int(m_currImg.cols * 0.7);
	int stepSize = 50;
	int disp = int(winH / 2);
	bool lastRow = false;
	int numDigits;
	bool insert_success;

#if HAS_CUDA

	ss.curr_img = m_currImg;
	m_gpuImg.upload(m_currImg);
	cv::Scalar mean_pixel_val;

	// Sliding window across image
	ss.wordFound = false;
	for (size_t j = startY; j <= (m_gpuImg.rows - winH); j += disp) {

		if (m_gpuImg.rows < (j + winH)) {
			j = m_gpuImg.rows - winH;
			lastRow = true;
		}

		//cout << "j = " << j << endl;
		bool lastCol = false;

		for (size_t k = startX; k <= (m_gpuImg.cols - winL); k += stepSize) {

			if (stopThreads) {
				goto stop;
			}
			//cout << "k = " << k << endl;
			if (m_gpuImg.cols < (k + winL)) {
				k = m_gpuImg.cols - winL;
				lastCol = true;
			}
			m_roi = Rect(k, j, winL, winH);
			m_roiImg = m_gpuImg(m_roi);
			cv::cuda::cvtColor(m_roiImg, m_grayImg, COLOR_BGR2GRAY);

			m_grayImg.download(m_roiMat);

			// Check data is actually present in image for reading:
			cv::Scalar mean_pixel_val = cv::mean(m_roiMat);


			if ((mean_pixel_val[0] < 247) & (mean_pixel_val[0] > 180)) {
				m_api->SetImage(mat8ToPix(&m_roiMat));
				m_outText = m_api->GetUTF8Text();
				ss.struct_roi = m_roi;
				ss.wordFound = false;
				// Only analyse text if more than 3 digits were found: 
				if (detectAndCountNumDigits() > 1) {
					m_confidence = m_api->MeanTextConf();

					for (size_t l = 0; l < sizeof(m_terms) / sizeof(m_terms[0]); l++) {

						if (m_outText.find(m_terms[l]) != std::string::npos) {
							{
								const std::lock_guard<mutex> lock(consolePrintGuard);
								std::cout << "id - " << m_iD << " found term: " << m_terms[l] << "! With confidence: " << m_confidence << endl;
							}

							ss.wordFound = true;
							ss.confidence = m_confidence;
							ss.actual_word = m_terms[l];
							// Function to improve image quality at this given spot: (Perhaps improve quality of read)
							saveAndCleanText(ss, m_terms[l]);
							break;
						}
					}
				}
			}
			if (lastCol)
				break;
		}

		if (lastRow)
			break;
	}
#endif
stop:; // goto if quitting program 
}


int PageProcessor::detectAndCountNumDigits() {

	int numDigits = 0;

	for (auto it = m_outText.begin(); it != m_outText.end(); ++it) {
		numDigits += std::isdigit(static_cast<unsigned char>(*it)) ? 1 : 0;
		if (numDigits > 1)
			return numDigits;
	}
	return 0;
}

void PageProcessor::saveAndCleanText(PageProcessor::StatusStruct& ss, const std::string& term) {

	// 1. Save all digits just as they appear
	extractDigitsfromText(ss, term);

	// 2. Image-processing Methodology (Open->Close->Open->Close) ---> AND THEN see if digits easier to read
	m_morphFilter_1->apply(m_grayImg, m_mask);
	m_morphFilter_2->apply(m_mask, m_mask);
	//m_morphFilter_1->apply(m_mask, m_mask);
	//m_morphFilter_2->apply(m_mask, m_mask);
	m_mask.download(m_roiMat);

#define SHOW_FILTER_OUTPUT 0
#if SHOW_FILTER_OUTPUT
	imshow("Mast output", m_roiMat);
	waitKey();
	destroyWindow("Mask output");
#endif // FILTER_OUTPUT

	// 3. Now see if above image processing has improved image quality:
	m_api->SetImage(mat8ToPix(&m_roiMat));
	m_outText = m_api->GetUTF8Text();

	// 4. Only analyse text if at least 2 digits were found: 
	if (detectAndCountNumDigits() > 1) {
		m_confidence = m_api->MeanTextConf();
		ss.struct_roi = m_roi;
		extractDigitsfromText(ss, term);
	}

}


void PageProcessor::extractDigitsfromText(PageProcessor::StatusStruct& ss, const std::string& term) {

	std::size_t locTerm = m_outText.find(term);
	string term_digits;
	int nonDigitCounter = 0;
	bool firstDigitSeen = false;

	if (locTerm != std::string::npos) {
		string m_outText_ = m_outText.substr(locTerm, m_outText.length() - locTerm);
		for (auto it = m_outText_.cbegin(); it != m_outText_.cend(); ++it) {

			if (std::isdigit(static_cast<unsigned char>(*it))) {
				firstDigitSeen = true;
				term_digits += *it;
				nonDigitCounter = 0;
			}

			else if (firstDigitSeen & (*it != ' ') & (*it != '-'))
				nonDigitCounter++;

			if ((term_digits.length() > 3) & (nonDigitCounter > 0)) {
				addTermToDict(ss, term, term_digits);
				break;
			}
		}

	}
}


void PageProcessor::addTermToDict(PageProcessor::StatusStruct& ss, const std::string& term, const std::string& term_digits) {

	std::pair<std::string, std::vector<std::string>> newEntry;
	newEntry.first = term;
	string str_to_add = (term_digits.length() > 0) ? term_digits : m_outText;
	newEntry.second.push_back(str_to_add);

	// 1. Add term without filtering/preprocessing
	if (ss.termDict.find(term) == ss.termDict.end()) {
		// not found
		ss.termDict.insert(newEntry);
	}
	else {
		// found
		ss.termDict[term].push_back(term_digits);
	}

}

// Following 2 methods obtained from "https://stackoverflow.com/questions/39293922/convert-between-opencv-mat-and-leptonica-pix"
Pix* PageProcessor::mat8ToPix(cv::Mat* mat8)
{
	Pix* pixd = pixCreate(mat8->size().width, mat8->size().height, 8);
	for (int y = 0; y < mat8->rows; y++) {
		for (int x = 0; x < mat8->cols; x++) {
			pixSetPixel(pixd, x, y, (l_uint32)mat8->at<uchar>(y, x));
		}
	}
	return pixd;
}

cv::Mat PageProcessor::pix8ToMat(Pix* pix8)
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