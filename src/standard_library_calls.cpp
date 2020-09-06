#include <standard_library_calls.h>
#include <thread>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;
using namespace std;

int STANDARD_LIB_CALLS::check_concurrency_method()
{
	int processor_count = std::thread::hardware_concurrency();
	return processor_count;
}

int STANDARD_LIB_CALLS::check_images_present()
{
	std::string path = "../../data/PDF_imgs/";
	int num_files = 0;
	try{
		for (const auto& entry : fs::directory_iterator(path)) {
			//std::cout << entry.path() << std::endl;
			num_files++;
		}
	
		}
	catch (fs::filesystem_error) {
		cout << "Filepath does not exist" << endl;
	}

	return num_files;
}














