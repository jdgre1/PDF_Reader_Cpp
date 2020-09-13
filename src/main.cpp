#include <pch.h>
#include <fstream>
using namespace std;
using namespace cv;

void RunPDFReader();


int main(int argc, char* argv[])
{
 
	RunPDFReader();

}



void printCWD(char* argv[]) {
    char the_path[256];

    getcwd(the_path, 255);
    strcat(the_path, "/");
    strcat(the_path, argv[0]);

    printf("%s\n", the_path);

}