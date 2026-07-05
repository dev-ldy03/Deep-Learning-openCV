#include <iostream>
#include <opencv2/opencv.hpp>

// output: input, output image

#define IM_TYPE	CV_8UC3

using namespace cv;

// Image Type
// "G" for GrayScale Image, "C" for Color Image
#if (IM_TYPE == CV_8UC3)
typedef uchar G;
typedef cv::Vec3b C;
#elif (IM_TYPE == CV_16SC3)
typedef short G;
typedef Vec3s C;
#elif (IM_TYPE == CV_32SC3)
typedef int G;
typedef Vec3i C;
#elif (IM_TYPE == CV_32FC3)
typedef float G;
typedef Vec3f C;
#elif (IM_TYPE == CV_64FC3)
typedef double G;
typedef Vec3d C;
#endif

Mat meanfilter(const Mat input, int n, const char* opt);
/*전체적인 코드 흐름은 MeanFilterGray.cpp와 유사하나 입력으로 컬러 이미지를 사용한다.*/ 

// 메인 함수는 MeanFilterGray.cpp와 동일하다. 
// 다만 input_gray 한 부분만 제외되었다. 
int main() {

	Mat input = imread("lena.jpg", IMREAD_COLOR);	// IMREAD_COLOR
	Mat output;
	

	if (!input.data)
	{
		std::cout << "Could not open" << std::endl;
		return -1;
	}

	namedWindow("Original", WINDOW_AUTOSIZE);
	imshow("Original", input);
	output = meanfilter(input, 3, "zero-paddle"); //Boundary process: zero-paddle, mirroring, adjustkernel

	
	namedWindow("Mean Filter", WINDOW_AUTOSIZE);
	imshow("Mean Filter", output);


	waitKey(0);

	return 0;
}


Mat meanfilter(const Mat input, int n, const char* opt) {

	Mat kernel;

	int row = input.rows;
	int col = input.cols;
	int kernel_size = (2 * n + 1);
	int tempa;
	int tempb;
	// Initialiazing Kernel Matrix 
	kernel = Mat::ones(kernel_size, kernel_size, CV_32F) / (float)(kernel_size * kernel_size);
	float kernelvalue = kernel.at<float>(0, 0);

	Mat output = Mat::zeros(row, col, input.type());


	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			/*zero padding, mirroring, adjust kernel 별 처리 메커니즘은 동일하지만, 각 옵션 안에 R, G, B 채널을 분리해 각각 처리하는 부분이 포함되어 있다.*/
			if (!strcmp(opt, "zero-paddle")) {
				/*R, G, B 각각의 총합을 분리하여 저장하기 위해
				sum1_r, sum1_g, sum1_b 3개의 변수를 선언하고 0으로 초기화한다.*/
				float sum1_r = 0.0;
				float sum1_g = 0.0;
				float sum1_b = 0.0;
				for (int a = -n; a <= n; a++) { // for each kernel window
					for (int b = -n; b <= n; b++) {

						if ((i + a <= row - 1) && (i + a >= 0) && (j + b <= col - 1) && (j + b >= 0)) { //if the pixel is not a border pixel
							sum1_r += kernelvalue*(float)(input.at<C>(i + a, j + b)[0]);
							sum1_g += kernelvalue*(float)(input.at<C>(i + a, j + b)[1]);
							sum1_b += kernelvalue*(float)(input.at<C>(i + a, j + b)[2]);
						}
					}
				}
				output.at<C>(i, j)[0] = (G)sum1_r;
				output.at<C>(i, j)[1] = (G)sum1_g;
				output.at<C>(i, j)[2] = (G)sum1_b;
			}

			else if (!strcmp(opt, "mirroring")) {
				float sum1_r = 0.0;
				float sum1_g = 0.0;
				float sum1_b = 0.0;
				for (int a = -n; a <= n; a++) { // for each kernel window
					for (int b = -n; b <= n; b++) {

						if (i + a > row - 1) {  //mirroring for the border pixels
							tempa = i - a;
						}
						else if (i + a < 0) {
							tempa = -(i + a);
						}
						else {
							tempa = i + a;
						}
						if (j + b > col - 1) {
							tempb = j - b;
						}
						else if (j + b < 0) {
							tempb = -(j + b);
						}
						else {
							tempb = j + b;
						}
						sum1_r += kernelvalue*(float)(input.at<C>(tempa, tempb)[0]);
						sum1_g += kernelvalue*(float)(input.at<C>(tempa, tempb)[1]);
						sum1_b += kernelvalue*(float)(input.at<C>(tempa, tempb)[2]);
					}
				}
				output.at<C>(i, j)[0] = (G)sum1_r;
				output.at<C>(i, j)[1] = (G)sum1_g;
				output.at<C>(i, j)[2] = (G)sum1_b;
			}

			else if (!strcmp(opt, "adjustkernel")) {
				float sum1_r = 0.0;
				float sum1_g = 0.0;
				float sum1_b = 0.0;
				float sum2 = 0.0;
				for (int a = -n; a <= n; a++) { // for each kernel window
					for (int b = -n; b <= n; b++) {
						if ((i + a <= row - 1) && (i + a >= 0) && (j + b <= col - 1) && (j + b >= 0)) {
							sum1_r += kernelvalue*(float)(input.at<C>(i + a, j + b)[0]);
							sum1_g += kernelvalue*(float)(input.at<C>(i + a, j + b)[1]);
							sum1_b += kernelvalue*(float)(input.at<C>(i + a, j + b)[2]);
							sum2 += kernelvalue;
						}
					}
				}
				output.at<C>(i, j)[0] = (G)(sum1_r / sum2);
				output.at<C>(i, j)[1] = (G)(sum1_g / sum2);
				output.at<C>(i, j)[2] = (G)(sum1_b / sum2);
			}
		}
	}
	return output;
}