#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;

int main() {

	Mat input = imread("lena.jpg", IMREAD_COLOR);

	// Checking logic if image loaded successfully
	if (!input.data)
	{
		std::cout << "Could not open" << std::endl;
		return -1;
	}

	Mat input_gray, output_gray, output;

	// Convert to grayscale, then back to 3-channel for pyrMeanShiftFiltering (requires 3-channel input)
	cvtColor(input, input_gray, COLOR_RGB2GRAY);
	cvtColor(input_gray, input_gray, COLOR_GRAY2RGB);

	imshow("Original Gray", input_gray);
	namedWindow("Original", WINDOW_AUTOSIZE);
	imshow("Original", input);

	// 1. Mean Shift filtering on grayscale image (sp=31: spatial window, sr=20: color window, maxLevel=1)
	pyrMeanShiftFiltering(input_gray, output_gray, 31, 20, 1);
	imshow("Meanshift Gray", output_gray);

	// 2. Mean Shift filtering on color image
	pyrMeanShiftFiltering(input, output, 31, 20, 3);
	imshow("Meanshift", output);

	waitKey();

	return 0;
}