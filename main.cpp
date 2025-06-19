#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <thread>

int main(int argc, char* argv[]) {

	using namespace std::chrono_literals;

	if (argv[1] == NULL) {
		printf("\n You need to pass the location of your desired video file as an argument when running the program in a terminal!\n");
		return -1;
	}

	cv::VideoCapture video;

	try
	{
		video = cv::VideoCapture(argv[1]); //this should be the path passed to the program to point to the video
	}
	catch (const std::exception&)
	{
		printf("\n You need to pass the location of your desired video file as an argument when running the program in a terminal!\n");
		return -1;
	}

	int currentFrame = 0;
	while (video.isOpened()) {;

		std::chrono::steady_clock::time_point startTime = std::chrono::high_resolution_clock::now();

		cv::Mat image;
		
		//this checks if the current frame of the video can be read, while also storing the frame in the "image" variable
		//while also making sure we can iterate through it if it is continuous
		//if none of that can happen, then just end the loop as either the video is broken, or we are finished
		if (!video.read(image) || !image.isContinuous()) {
			break;
		}
		
		//store here for added simplicity later on and miniscule, insignificant "performance" benefits
		int totalColumns = image.cols * image.rows * image.channels(); //the amount of all available columns
		int sizeOfRow = image.cols * image.channels(); //how long each row is
		
		//how many rows and collumns are in the ascii image. rows are much smaller due to the 2:1 height:width ratio of ascii characters
		int imgRows = ((image.rows - (image.rows % 20)) / 20); //basically integer division by 20
		int imgCols = ((image.cols - (image.cols % 10)) / 10); //basically integer division by 10

		char* finalImage = (char*)malloc(imgRows * imgCols);

		int disPerRow = sizeOfRow*20; //the distance we should step for each row
		int disPerCol = image.channels()*10; //the distance we should step for each column

		int i, j, k, l;
		for (i = disPerRow-1; i < totalColumns; i+=disPerRow) {
			int start = imgCols * i / disPerRow; //the point from which we will start iterating over the current row
			finalImage[start] = '\n';
			for (j = i-disPerCol; j > i - sizeOfRow; j-=disPerCol) {
				int totalColours = 0;//the total values of all the colours we iterated over
				int amtOfColours = 0;//how many colours we iterated over
				for (k = j; k < j+disPerCol; k++) { //iterates over 10 collumns
					totalColours += image.ptr(0)[k];
					amtOfColours++;
					for (l = k; l > k-disPerRow; l-=sizeOfRow) { //iterates over 20 rows
						totalColours += image.ptr(0)[l];
						amtOfColours++;
					}
				}
				if (totalColours/amtOfColours > 125) finalImage[start - ((i - j) / disPerCol)] = '-';
				else finalImage[start - ((i - j) / disPerCol)] = ' ';
			}
		}

		printf("\n\n%s", finalImage);
		free(finalImage);
		
		currentFrame++;
		std::this_thread::sleep_for(30ms - (std::chrono::high_resolution_clock::now() - startTime));
	}

	video.release();

	return 0;
}