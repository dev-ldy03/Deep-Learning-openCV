#include "hist_func.h"

// 함수 선언: histogram equalization을 수행하는 함수
void hist_eq(Mat& input, Mat& equalized, G* trans_func, float* CDF);

int main() {

	Mat input = imread("input.jpg", IMREAD_COLOR);

	// 이미지 로드 실패 시 프로그램 종료 보호 코드 추가
	if (input.empty()) return -1;

	Mat input_gray;
	cvtColor(input, input_gray, COLOR_BGR2GRAY);	// convert RGB to Grayscale (8비트: 0~255 범위)

	Mat equalized = input_gray.clone();

	// PDF or transfer function txt files
	FILE* f_PDF;
	FILE* f_equalized_PDF_gray;
	FILE* f_trans_func_eq;

	fopen_s(&f_PDF, "PDF.txt", "w+");
	fopen_s(&f_equalized_PDF_gray, "equalized_PDF_gray.txt", "w+");
	fopen_s(&f_trans_func_eq, "trans_func_eq.txt", "w+");

	// 각 히스토그램 계산 (PDF/CDF 값은 확률이므로 0.0 ~ 1.0 범위)
	float* PDF = cal_PDF(input_gray);	// PDF of Input image(Grayscale) : [L]
	float* CDF = cal_CDF(input_gray);	// CDF of Input image(Grayscale) : [L]

	G trans_func_eq[L] = { 0 };			// transfer function (매핑될 밝기값 저장용: 0~255)

	// histogram equalization on grayscale image
	hist_eq(input_gray, equalized, trans_func_eq, CDF);								
	float* equalized_PDF_gray = cal_PDF(equalized);									// equalized PDF (grayscale)

	for (int i = 0; i < L; i++) {
		// write PDF
		fprintf(f_PDF, "%d\t%f\n", i, PDF[i]);
		fprintf(f_equalized_PDF_gray, "%d\t%f\n", i, equalized_PDF_gray[i]);

		// write transfer functions
		fprintf(f_trans_func_eq, "%d\t%d\n", i, trans_func_eq[i]);
	}

	// 그래프 시각화 (0~255 밝기 축에 대해 확률 분포를 그림)
	plot(PDF, "PDF", "PDF");
	plot(equalized_PDF_gray, "PDF", "PDF(Hist_eq)");

	// memory release
	free(PDF);
	free(CDF);
	fclose(f_PDF);
	fclose(f_equalized_PDF_gray);
	fclose(f_trans_func_eq);

	////////////////////// histogram ///////////////////////
	FILE* f_hist;
	FILE* f_equalized_hist;

	fopen_s(&f_hist, "hist.txt", "w+");
	fopen_s(&f_equalized_hist, "equalized_hist.txt", "w+");

	int histSize = L; // 256개 빈(bin)
	float ran[] = { 0, 256 };
	const float* range = { ran };

	
	Mat hist, equalized_hist;
	calcHist(&input_gray, 1, 0, Mat(), hist, 1, &histSize, &range);
	calcHist(&equalized, 1, 0, Mat(), equalized_hist, 1, &histSize, &range);

	for (int i = 0; i < histSize; i++) {
		fprintf(f_hist, "%d\t%f\n", i, hist.at<float>(i));
		fprintf(f_equalized_hist, "%d\t%f\n", i, equalized_hist.at<float>(i));
	}

	fclose(f_hist);
	fclose(f_equalized_hist);

	////////////////////// Show each image ///////////////////////

	namedWindow("Grayscale", WINDOW_AUTOSIZE);
	imshow("Grayscale", input_gray);

	namedWindow("Equalized", WINDOW_AUTOSIZE);
	imshow("Equalized", equalized);

	//////////////////////////////////////////////////////////////

	waitKey(0);

	return 0;
}

// [함수 구현] 히스토그램 평활화 로직
// 입력: 원본, 결과 Mat, 변환함수 배열, 원본의 CDF
void hist_eq(Mat& input, Mat& equalized, G* trans_func, float* CDF) {

	// 1. 변환 함수(Transfer Function) 계산
	// 공식: s = (L-1) * CDF(r) -> 
	// 결과값(s)은 0~255 사이의 정수(G/uchar)로 반올림되어 저장됨
	for (int i = 0; i < L; i++)
		trans_func[i] = (G)((L - 1) * CDF[i]);

	// 2. 실제 픽셀 매핑 연산
	// 원본 이미지의 모든 픽셀을 순회하며 변환 함수에 따라 값을 치환함
	for (int i = 0; i < input.rows; i++) {
		for (int j = 0; j < input.cols; j++) {
			// input.at<G>(i, j)는 현재 픽셀의 밝기값(r), 이를 인덱스로 변환값(s)을 가져옴
			equalized.at<G>(i, j) = trans_func[input.at<G>(i, j)];
		}
	}
}