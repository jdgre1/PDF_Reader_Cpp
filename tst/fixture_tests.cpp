#include <fixture_tests.h>


SetupTest::SetupTest(): m_api() {
	m_api = new tesseract::TessBaseAPI();
	
}

SetupTest::~SetupTest() {
	delete m_api;
}
bool SetupTest::initialiseWithEnglish()
{
	if (m_api->Init(NULL, "eng")) {
		fprintf(stderr, "Could not initialize tesseract.\n");
		exit(1);
		return false;
	}
	return true;
}

TEST_F(SetupTest, initializeWithEnglish) {

	ASSERT_EQ(SetupTest::initialiseWithEnglish(), true);
}


