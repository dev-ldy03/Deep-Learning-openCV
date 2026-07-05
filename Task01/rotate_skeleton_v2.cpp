#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

// 함수 프로토타입
template <typename T>
Mat myrotate(const Mat input, float angle, const char* opt);

int main()
{
	Mat input;

	// Read each image
	input = imread("lena.jpg");

	// Check for invalid input
	if (!input.data) {
		std::cout << "Could not open or find the image" << std::endl;
		return -1;
	}

	// [추가코드] 여러 각도를 테스트 하기 위한 반복문을 구현
	float angle;
	while (true) {
		// 각도 입력 받는 로직
		cout << "type in the angle you want to rotate(type 0 to terminate):";
		cin >> angle;
		// while 문 탈출 코드. 각도 입력을 0으로 하면 종료시킴
		if (angle == 0) break;

		// 두 가지 보간법 결과를 한번에 생성해서 비교할 수 있도록 구현
		Mat nearest_result = myrotate<Vec3b>(input, angle, "nearest");
		Mat bilinear_result = myrotate<Vec3b>(input, angle, "bilinear");

		// 원본 이미지와 함께 두 가지 보간법 결과 창 띄우기
		imshow("original", input);
		imshow("Nearest_Result", nearest_result);
		imshow("Bilinear_Result", bilinear_result);

		cout << angle << "degrees rotated";
		// 각도 확인 후 다음 입력을 위해 대기 
		waitKey(0);

	}

	return 0;
}

template <typename T>
Mat myrotate(const Mat input, float angle, const char* opt) {
	int row = input.rows;
	int col = input.cols;

	float radian = angle * CV_PI / 180;

	float sq_row = ceil(row * abs(cos(radian)) + col * abs(sin(radian)));
	float sq_col = ceil(col * abs(cos(radian)) + row * abs(sin(radian)));

	Mat output = Mat::zeros(sq_row, sq_col, input.type()); // 새로운 캔버스 검정색으로 초기화

	for (int i = 0; i < sq_row; i++) {
		for (int j = 0; j < sq_col; j++) {
			float x = (j - sq_col / 2.0f) * cos(radian) - (i - sq_row / 2.0f) * sin(radian) + col / 2.0f;
			float y = (j - sq_col / 2.0f) * sin(radian) + (i - sq_row / 2.0f) * cos(radian) + row / 2.0f;

			if ((y >= 0) && (y <= (row - 1)) && (x >= 0) && (x <= (col - 1))) {
				if (!strcmp(opt, "nearest")) {
					int nx = cvRound(x);
					int ny = cvRound(y);

					// 새로 계산된 x,y 값이 사진의 범위를 벗어나지 않게 하기 위해 방어하는 코드
					nx = (nx < 0) ? 0 : (nx >= col ? col - 1 : nx);
					ny = (ny < 0) ? 0 : (ny >= row ? row - 1 : ny);

					output.at<T>(i, j) = input.at<T>(ny, nx);
				}
				else if (!strcmp(opt, "bilinear")) {
					// x, y 에 대한 주변 네개의 픽셀 구하기
					int x0 = std::max(0, (int)floor(x)); // 음수가 되지 않도록 max조건 적용
					int x1 = std::min(x0 + 1, col - 1); // 이미지의 크기를 넘지 않도록 min 조건 적용
					int y0 = std::max(0, (int)floor(y)); // 음수가 되지 않도록 max조건 적용
					int y1 = std::min(y0 + 1, row - 1); // 이미지의 크기를 넘지 않도록 min 조건 적용

					// x, y 각각의 소수 부분 구하기(람다, 뮤)
					float dx = x - x0;
					float dy = y - y0;

					// 4개의 픽셀로 OUTPUT 구하기
					output.at<T>(i, j) =
						(1 - dx) * (1 - dy) * input.at<T>(y0, x0) +
						dx * (1 - dy) * input.at<T>(y0, x1) +
						(1 - dx) * dy * input.at<T>(y1, x0) +
						dx * dy * input.at<T>(y1, x1);




				}
			}
		}
	}

	return output;
}