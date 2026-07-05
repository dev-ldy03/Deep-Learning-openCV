#include <iostream>
#include <opencv2/opencv.hpp>

// output: input, output image

#define IM_TYPE	CV_8UC3

using namespace cv;
using namespace std;

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

int main() {
	// 이미지를 정상적으로 읽어 왔는지 확인하고, 이미지가 invalid 하면 에러 메시지 출력한다.
	Mat input = imread("lena.jpg", IMREAD_COLOR);	// IMREAD_COLOR
	Mat input_gray;
	Mat output;

	cvtColor(input, input_gray, COLOR_RGB2GRAY); // Converting image to gray	COLOR_RGB2GRAY
	// imread() 함수와 IMREAD_COLOR flag 를 사용해 
	// "lena.jpg" 이미지를 읽어 와 input에 저장하고, 
	// Mat형 input_gray와 output 객체를 선언한 후 input을 grayscale로 변환하여 input_gray에 저장한다.

	if (!input.data)
	{
		cout << "Could not open" << std::endl;
		return -1;
	}

	namedWindow("Grayscale", WINDOW_AUTOSIZE);
	imshow("Grayscale", input_gray);

	output = meanfilter(input_gray, 3, "mirroring"); //Boundary process: zero-paddle, mirroring, adjustkernel
	//Boundary process: zero-paddle, mirroring, adjustkernel

	// output = meanfilter(input_gray, 3, "zero-paddle"); 
	// output = meanfilter(input_gray, 3, "adjustkernel");

	namedWindow("Mean Filter", WINDOW_AUTOSIZE);
	imshow("Mean Filter", output);

	waitKey(0);

	return 0;
}


Mat meanfilter(const Mat input, int n, const char* opt) {

	Mat kernel;
	// 함수정의 부분
	int row = input.rows;
	int col = input.cols;
	int kernel_size = (2 * n + 1);
	// tempa와 tempb 변수는 이미지 경계에 위치한 픽셀을 처리할 때 인덱스 계산에 사용한다.
	int tempa; 
	int tempb;
 // Initialiazing Kernel Matrix 
	kernel = Mat::ones(kernel_size, kernel_size, CV_32F) / (float)(kernel_size * kernel_size);
	float kernelvalue=kernel.at<float>(0, 0);  // To simplify, as the filter is uniform. All elements of the kernel value are same.
	
	// 결과 이미지를 저장하기 위해 input과 동일한 Mat형 output 객체 생성 + 0으로 초기화
	Mat output = Mat::zeros(row, col, input.type());
	
	// 반복문을 돌면서 이미지의 모든 픽셀(i,j)에 대해 uniform mean filtering 진행한다. 
	for (int i = 0; i < row; i++) { //for each pixel in the output
		for (int j = 0; j < col; j++) {
			
			if (!strcmp(opt, "zero-paddle")) { // 이미지 경계 바깥 픽셀 값을 0으로 처리한다.
				float sum1 = 0.0;
				for (int a = -n; a <= n; a++) { // for each kernel window
					for (int b = -n; b <= n; b++) {

						if ((i + a <= row - 1) && (i + a >= 0) && (j + b <= col - 1) && (j + b >= 0)) { //if the pixel is not a border pixel
							sum1 += kernelvalue * (float)(input.at<G>(i + a, j + b));
						}
					}
				}
				output.at<G>(i, j) = (G)sum1; // 이중 for문 종료시, sum1은 픽셀에 매칭될 커널의 산술평균값을 가짐
			}
			
			// mirroring은 커널 내 이미지 범위를 초과한 값이 존재할 때 해당 픽셀 주변의 다른 픽셀 값을 할당한다.
			else if (!strcmp(opt, "mirroring")) { 
		
				float sum1 = 0.0;
				for (int a = -n; a <= n; a++) { // for each kernel window
					for (int b = -n; b <= n; b++) {
						//mirroring for the border pixels
						// 수직 방향
						if (i + a > row - 1) {  // a>0일 때, 이미지 아래 경계 초과
							tempa = i - a;		// (i, j)에서 a만큼 위쪽 픽셀 값
						}
						else if (i + a < 0) {	// a<0일 때, 이미지 위쪽 경계 초과
							tempa = -(i + a);	// (i, j)에서 a만큼 아래쪽 픽셀 값
						}
						else {
							tempa = i + a;	// 범위 내 -> 그대로 사용
						}
						// 수평 방향
						if (j + b > col - 1) {	// b>0일 때, 이미지 우측 경계 초과
							tempb = j - b;		// (i, j)에서 b만큼 왼쪽 픽셀 값
						}
						else if (j + b < 0) {	// b<0일 때, 이미지 좌측 경계 초과
							tempb = -(j + b);	// (i, j)에서 b만큼 오른쪽 픽셀 값
						}
						else {
							tempb = j + b;	// 범위 내 -> 그대로 사용
						}
						sum1 += kernelvalue*(float)(input.at<G>(tempa, tempb));	// sum에 포함
					}
				}
				output.at<G>(i, j) = (G)sum1;	// 결과 이미지에 uniform mean filtering한 (i, j)값 할당
			}
			
			else if (!strcmp(opt, "adjustkernel")) {
				float sum1 = 0.0;
				float sum2 = 0.0;
				for (int a = -n; a <= n; a++) { // for each kernel window
					for (int b = -n; b <= n; b++) {
						if ((i + a <= row - 1) && (i + a >= 0) && (j + b <= col - 1) && (j + b >= 0)) {
							sum1 += kernelvalue*(float)(input.at<G>(i + a, j + b));	
							sum2 += kernelvalue;	// 분모
						}
					}
				}
				output.at<G>(i, j) = (G)(sum1/sum2);
			}
		}
	}
	return output;
}
