#include "hist_func.h"

void hist_eq(Mat& input, Mat& equalized, G* trans_func, float* CDF);

int main() {
	// 1. 이미지 로드 (OpenCV는 기본적으로 BGR 순서임)
	Mat input = imread("input.jpg", IMREAD_COLOR);
	if (input.empty()) return -1;

	Mat equalized_YUV;

	// BGR -> YUV로 변환
	cvtColor(input, equalized_YUV, COLOR_BGR2YUV);

	// split each channel(Y, U, V)
	Mat channels[3];
	split(equalized_YUV, channels);
	Mat Y = channels[0];						// Y(밝기) = channels[0], U = channels[1], V = channels[2]

	// PDF or transfer function txt files
	FILE* f_equalized_PDF_YUV, * f_PDF_YUV; 
	FILE* f_trans_func_eq_YUV;

	// equalization 전 Y 채널의 PDF와 CDF 계산
	float* PDF_Y = cal_PDF(Y);		            // 원본 Y 채널의 PDF
	float* CDF_YUV = cal_CDF(Y);				// Y 채널의 CDF

	fopen_s(&f_PDF_YUV, "PDF_YUV.txt", "w+");
	fopen_s(&f_equalized_PDF_YUV, "equalized_PDF_YUV.txt", "w+");
	fopen_s(&f_trans_func_eq_YUV, "trans_func_eq_YUV.txt", "w+");

	G trans_func_eq_YUV[L] = { 0 };			// transfer function 배열

	// 2. Y 채널에 대해서만 Histogram Equalization 수행 
	hist_eq(Y, channels[0], trans_func_eq_YUV, CDF_YUV);

	// merge Y(equalized), U, V channels
	merge(channels, 3, equalized_YUV);

	// YUV -> BGR (다시 화면에 보여주기 위해 변환)
	Mat result_BGR;
	cvtColor(equalized_YUV, result_BGR, COLOR_YUV2BGR);

	// 3. equalization 후의 Y 채널 PDF 계산
	float* equalized_PDF_Y = cal_PDF(channels[0]);

	for (int i = 0; i < L; i++) {
		// 원본 Y PDF, 결과 Y PDF, 변환 함수값 저장
		fprintf(f_PDF_YUV, "%d\t%f\n", i, PDF_Y[i]);
		fprintf(f_equalized_PDF_YUV, "%d\t%f\n", i, equalized_PDF_Y[i]);
		fprintf(f_trans_func_eq_YUV, "%d\t%d\n", i, trans_func_eq_YUV[i]);
	}

	// 시각화
	plot(PDF_Y, "PDF", "Original Y PDF");
	plot(equalized_PDF_Y, "PDF", "Equalized Y PDF");

	// 4. 메모리 해제 로직 (2차원 배열은 반복문으로 해제해야 함)
	free(PDF_Y);
	free(equalized_PDF_Y);
	free(CDF_YUV);
	fclose(f_PDF_YUV);
	fclose(f_equalized_PDF_YUV);
	fclose(f_trans_func_eq_YUV);

	////////////////////// histogram ///////////////////////
	// 원본/결과 RGB 히스토그램 추출 
	FILE* f_hist_YUV;
	FILE* f_equalized_hist_YUV;

	fopen_s(&f_hist_YUV, "hist_YUV.txt", "w+");
	fopen_s(&f_equalized_hist_YUV, "equalized_hist_YUV.txt", "w+");

	int histSize = L;
	float ran[] = { 0, 256 };
	const float* range = { ran };

	// 원본 BGR 히스토그램 계산
	vector<Mat> bgr_orig;
	split(input, bgr_orig);

	Mat histB, histG, histR;
	calcHist(&bgr_orig[0], 1, 0, Mat(), histB, 1, &histSize, &range);
	calcHist(&bgr_orig[1], 1, 0, Mat(), histG, 1, &histSize, &range);
	calcHist(&bgr_orig[2], 1, 0, Mat(), histR, 1, &histSize, &range);

	// 결과(Equalized) BGR 히스토그램 계산
	vector<Mat> bgr_res;
	split(result_BGR, bgr_res);

	Mat histB_eq, histG_eq, histR_eq;
	calcHist(&bgr_res[0], 1, 0, Mat(), histB_eq, 1, &histSize, &range);
	calcHist(&bgr_res[1], 1, 0, Mat(), histG_eq, 1, &histSize, &range);
	calcHist(&bgr_res[2], 1, 0, Mat(), histR_eq, 1, &histSize, &range);

	for (int i = 0; i < histSize; i++) {
		// R, G, B 순서로 텍스트 파일에 기록
		fprintf(f_hist_YUV, "%d\t%f\t%f\t%f\n", i, histR.at<float>(i), histG.at<float>(i), histB.at<float>(i));
		fprintf(f_equalized_hist_YUV, "%d\t%f\t%f\t%f\n", i, histR_eq.at<float>(i), histG_eq.at<float>(i), histB_eq.at<float>(i));
	}

	fclose(f_hist_YUV);
	fclose(f_equalized_hist_YUV);

	////////////////////// Show each image ///////////////////////

	namedWindow("Original RGB", WINDOW_AUTOSIZE);
	imshow("Original RGB", input);

	namedWindow("Equalized YUV result", WINDOW_AUTOSIZE);
	imshow("Equalized YUV result", result_BGR);

	//////////////////////////////////////////////////////////////

	waitKey(0);
	return 0;
}

void hist_eq(Mat& input, Mat& equalized, G* trans_func, float* CDF) {
	for (int i = 0; i < L; i++)
		trans_func[i] = (G)((L - 1) * CDF[i]);

	for (int i = 0; i < input.rows; i++)
		for (int j = 0; j < input.cols; j++)
			equalized.at<G>(i, j) = trans_func[input.at<G>(i, j)];
}