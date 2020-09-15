#include "tesseract_api_calls.h"


using namespace std;

bool TESSERACT_API_CALLS::checkTesseractObjectInitialisation()
{
	tesseract::TessBaseAPI* api = new tesseract::TessBaseAPI();

	if (api->Init(NULL, "eng")) {
		fprintf(stderr, "Could not initialize tesseract.\n");
		exit(1);
		return false;
	}

	delete api;
	return true;
}








