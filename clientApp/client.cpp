#include <server.h>
#include <string>
// https://answers.opencv.org/question/197414/sendreceive-vector-mat-over-socket-cc/
// https://stackoverflow.com/questions/41637438/opencv-imencode-buffer-exception

void runClientApp();

int main(int argc, char* argv[])
{
	runClientApp();
	return 0;
}

void runClientApp() {
	// Define reused variables
	cv::Mat frame;
	std::string ipAddress = "192.server.ipAddress.here";
	int port = 54000;

	std::vector <uchar> buffer;
	size_t buffer_size = 0;
	int len = 0;
	size_t remain = 0;
	size_t offset = 0;

	// Startup
	WSADATA data;
	WORD ver = MAKEWORD(2, 2);
	int wsResult = WSAStartup(ver, &data);
	if (wsResult != 0) {
		std::cerr << "Can't start Winsock, Err #" << wsResult << std::endl;
		return;
	}

	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket == INVALID_SOCKET) {
		std::cerr << "Can't create socket, Err #" << WSAGetLastError() << std::endl;
		WSACleanup();
		return;
	}

	// Fill in a hint structure
	sockaddr_in hints;
	hints.sin_family = AF_INET;
	hints.sin_port = htons(port);
	inet_pton(AF_INET, ipAddress.c_str(), &hints.sin_addr);

	// Connect to server
	int connection_result = connect(clientSocket, (sockaddr*)&hints, sizeof(hints));

	if (connection_result == SOCKET_ERROR) {
		std::cerr << "Can't connect to server, Err #" << WSAGetLastError() << std::endl;
		WSACleanup();
		return;
	}

	while (true) {
		buffer.clear();
		buffer_size = 0;
		recv(clientSocket, (char*)(&buffer_size), sizeof(size_t), 0);
		buffer.resize(buffer_size);
		remain = buffer_size;
		offset = 0;
		len = 0;
		std::cout << "Received package of " << buffer_size << " bytes." << std::endl;

		while ((remain > 0) && ((len = recv(clientSocket, (char*)(buffer.data()) + offset, remain, 0)) > 0))
		{
			remain -= len;
			offset += len;
		}
		if (len <= 0)
		{
			std::cout << "Error processing receipt of packet" << std::endl;
			return;
		}

		frame = cv::imdecode(cv::Mat(buffer), 1);
		cv::imshow("received frame", frame);
		char quitProgram = cv::waitKey(100);

		if (quitProgram == 'q') {
			closesocket(clientSocket);
			WSACleanup();
			break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10));

	}
	WSACleanup();
}