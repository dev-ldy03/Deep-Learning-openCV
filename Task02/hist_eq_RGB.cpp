#include "hist_func.h"

void hist_eq_Color(Mat& input, Mat& equalized, G(*trans_func)[3], float** CDF);

int main() {

	Mat input = imread("input.jpg", IMREAD_COLOR);
	if (input.empty()) return -1;

	Mat equalized_RGB = input.clone();

	// PDF or transfer function txt files
	FILE* f_equalized_PDF_RGB, * f_PDF_RGB;
	FILE* f_trans_func_eq_RGB;

	fopen_s(&f_PDF_RGB, "PDF_RGB.txt", "w+");
	fopen_s(&f_equalized_PDF_RGB, "equalized_PDF_RGB.txt", "w+");
	fopen_s(&f_trans_func_eq_RGB, "trans_func_eq_RGB.txt", "w+");

	float** PDF_RGB = cal_PDF_RGB(input);	// PDF of Input image(RGB) : [L][3]
	float** CDF_RGB = cal_CDF_RGB(input);	// CDF of Input image(RGB) : [L][3]

	G trans_func_eq_RGB[L][3] = { 0 };		// transfer function

	// histogram equalization on RGB image
	// 3채널 equalization 함수 호출
	hist_eq_Color(input, equalized_RGB, trans_func_eq_RGB, CDF_RGB);

	// equalized PDF (RGB)
	// equalization 결과 이미지의 PDF 계산
	float** equalized_PDF_RGB = cal_PDF_RGB(equalized_RGB);

	for (int i = 0; i < L; i++) {
		// write PDF
		// B, G, R 순서대로 PDF 기록
		fprintf(f_PDF_RGB, "%d\t%f\t%f\t%f\n", i, PDF_RGB[i][0], PDF_RGB[i][1], PDF_RGB[i][2]);
		fprintf(f_equalized_PDF_RGB, "%d\t%f\t%f\t%f\n", i, equalized_PDF_RGB[i][0], equalized_PDF_RGB[i][1], equalized_PDF_RGB[i][2]);

		// write transfer functions
		// 채널별 변환 함수 값 기록
		fprintf(f_trans_func_eq_RGB, "%d\t%d\t%d\t%d\n", i, trans_func_eq_RGB[i][0], trans_func_eq_RGB[i][1], trans_func_eq_RGB[i][2]);
	}

	// 그래프 생성 코드 
	// plot 함수는 float*를 받으므로, 시각화를 위해 Green 채널(인덱스 1)을 대표로 출력한다.
	float plot_PDF_orig[L], plot_PDF_eq[L];
	for (int i = 0; i < L; i++) {
		plot_PDF_orig[i] = PDF_RGB[i][1];      // 원본 Green 채널
		plot_PDF_eq[i] = equalized_PDF_RGB[i][1]; // 결과 Green 채널
	}
	plot(plot_PDF_orig, "PDF", "Original PDF (Green)");
	plot(plot_PDF_eq, "PDF", "Equalized PDF (Green)");

	// memory release
	// 2차원 배열 해제를 위한 루프 추가
	for (int i = 0; i < L; i++) {
		free(PDF_RGB[i]);
		free(CDF_RGB[i]);
		free(equalized_PDF_RGB[i]);
	}
	free(PDF_RGB);
	free(CDF_RGB);
	free(equalized_PDF_RGB);

	fclose(f_PDF_RGB);
	fclose(f_equalized_PDF_RGB);
	fclose(f_trans_func_eq_RGB);

	////////////////////// Show each image ///////////////////////

	namedWindow("RGB", WINDOW_AUTOSIZE);
	imshow("RGB", input);

	namedWindow("Equalized_RGB", WINDOW_AUTOSIZE);
	imshow("Equalized_RGB", equalized_RGB);

	//////////////////////////////////////////////////////////////

	waitKey(0);

	return 0;
}

// histogram equalization on 3 channel image
void hist_eq_Color(Mat& input, Mat& equalized, G(*trans_func)[3], float** CDF) {

	////////////////////////////////////////////////
	//											  //
	// How to access multi channel matrix element //
	//											  //
	// if matrix A is CV_8UC3 type,				  //
	// A(i, j, k) -> A.at<Vec3b>(i, j)[k]		  //
	//											  //
	////////////////////////////////////////////////

	// 1. 각 채널별 변환 함수 계산
	for (int k = 0; k < 3; k++) {
		for (int j = 0; j < L; j++) {
			// 각 채널의 CDF를 이용해 equalization 값 계산
			trans_func[j][k] = (G)((L - 1) * CDF[j][k]);
		}
	}

	// 2. 픽셀 단위 매핑 (원본 밝기를 인덱스로 결과값 대입)
	for (int i = 0; i < input.rows; i++) {
		for (int j = 0; j < input.cols; j++) {
			for (int k = 0; k < 3; k++) {
				equalized.at<Vec3b>(i, j)[k] = trans_func[input.at<Vec3b>(i, j)[k]][k];
			}
		}
	}
}