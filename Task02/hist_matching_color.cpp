#include "hist_func.h"

void hist_eq(Mat& input, Mat& equalized, G* trans_func, float* CDF);
// 함수 인자를 수정하여 입력과 참조의 변환 함수를 모두 사용하도록 함
void hist_matching(Mat& input, Mat& matched, G* trans_func_z, G* T, G* G_map);

int main() {
	// 이미지 로드 
	Mat input = imread("input_image_flower.jpg", IMREAD_COLOR);
	Mat reference = imread("input.jpg", IMREAD_COLOR);

	if (input.empty() || reference.empty()) return -1;

	Mat equalized_input, equalized_ref;
	// OpenCV 기본 순서인 BGR을 YUV로 변환
	cvtColor(input, equalized_input, COLOR_BGR2YUV);
	cvtColor(reference, equalized_ref, COLOR_BGR2YUV);

	Mat channels_input[3], channels_ref[3];
	split(equalized_input, channels_input);
	split(equalized_ref, channels_ref);

	Mat Y_input = channels_input[0];
	Mat Y_ref = channels_ref[0];

	// PDF/CDF 계산
	float* CDF_input = cal_CDF(Y_input);
	float* CDF_ref = cal_CDF(Y_ref);

	G trans_func_input[L] = { 0 }; // T(r)
	G trans_func_ref[L] = { 0 };   // G(z)

	// 각 이미지의 Y 채널 평활화 함수 계산 (매칭의 중간 단계)
	hist_eq(Y_input, channels_input[0], trans_func_input, CDF_input);
	hist_eq(Y_ref, channels_ref[0], trans_func_ref, CDF_ref);

	Mat matched_YUV = equalized_input.clone();
	Mat channels_matched[3];
	split(matched_YUV, channels_matched);

	G trans_func_z[L] = { 0 }; // 최종 매핑 함수 r -> z

	// 히스토그램 매칭 수행
	hist_matching(Y_input, channels_matched[0], trans_func_z, trans_func_input, trans_func_ref);

	// 채널 합치기 및 다시 BGR로 변환
	merge(channels_matched, 3, matched_YUV);
	Mat matched_BGR;
	cvtColor(matched_YUV, matched_BGR, COLOR_YUV2BGR);

	free(CDF_input);
	free(CDF_ref);

	///////////////////////// files //////////////////////////
	FILE* f_trans_func, * f_hist_orig, * f_hist_matched;
	fopen_s(&f_trans_func, "matching_function_color.txt", "w+");
	fopen_s(&f_hist_orig, "matching_original_hist_color.txt", "w+");
	fopen_s(&f_hist_matched, "matched_hist_color.txt", "w+");

	for (int i = 0; i < L; i++) {
		fprintf(f_trans_func, "%d\t%d\n", i, trans_func_z[i]);
	}

	int histSize = L;
	float ran[] = { 0, 256 };
	const float* range = { ran };

	// 원본 BGR 히스토그램 계산
	vector<Mat> bgr_orig;
	split(input, bgr_orig);
	Mat hB, hG, hR;
	calcHist(&bgr_orig[0], 1, 0, Mat(), hB, 1, &histSize, &range);
	calcHist(&bgr_orig[1], 1, 0, Mat(), hG, 1, &histSize, &range);
	calcHist(&bgr_orig[2], 1, 0, Mat(), hR, 1, &histSize, &range);

	// 매칭 결과 BGR 히스토그램 계산
	vector<Mat> bgr_matched;
	split(matched_BGR, bgr_matched);
	Mat hBm, hGm, hRm;
	calcHist(&bgr_matched[0], 1, 0, Mat(), hBm, 1, &histSize, &range);
	calcHist(&bgr_matched[1], 1, 0, Mat(), hGm, 1, &histSize, &range);
	calcHist(&bgr_matched[2], 1, 0, Mat(), hRm, 1, &histSize, &range);

	for (int i = 0; i < histSize; i++) {
		fprintf(f_hist_orig, "%d\t%f\t%f\t%f\n", i, hR.at<float>(i), hG.at<float>(i), hB.at<float>(i));
		fprintf(f_hist_matched, "%d\t%f\t%f\t%f\n", i, hRm.at<float>(i), hGm.at<float>(i), hBm.at<float>(i));
	}
	fclose(f_trans_func);
	fclose(f_hist_orig);
	fclose(f_hist_matched);

	////////////////////// Show ///////////////////////
	// 창을 'NORMAL' 모드로 일단 생성
	namedWindow("Input", WINDOW_NORMAL);
	namedWindow("Reference", WINDOW_NORMAL);
	namedWindow("Matched Result", WINDOW_NORMAL);

	// 이미지가 너무 큰 관계로 윈도우 사이즈 조정
	resizeWindow("Input", 500, 500);
	resizeWindow("Reference", 500, 500);
	resizeWindow("Matched Result", 500, 500);

	imshow("Input", input);
	imshow("Reference", reference);
	imshow("Matched Result", matched_BGR);

	waitKey(0);
	return 0;
}

// equalization 함수
void hist_eq(Mat& input, Mat& equalized, G* trans_func, float* CDF) {
	for (int i = 0; i < L; i++)
		trans_func[i] = (G)((L - 1) * CDF[i]);

	for (int i = 0; i < input.rows; i++)
		for (int j = 0; j < input.cols; j++)
			equalized.at<G>(i, j) = trans_func[input.at<G>(i, j)];
}

// 히스토그램 매칭 알고리즘 구현
void hist_matching(Mat& input, Mat& matched, G* trans_func_z, G* T, G* G_map) {
	// z = G^-1(T(r)) 구현
	for (int r = 0; r < L; r++) {
		int s = T[r]; // 입력 이미지의 평활화된 값
		int best_z = 0;
		int min_diff = L;

		// 참조 영상의 평활화 함수 G(z) 중 s와 가장 가까운 값을 가진 z를 찾음
		for (int z = 0; z < L; z++) {
			int diff = abs((int)G_map[z] - s);
			if (diff < min_diff) {
				min_diff = diff;
				best_z = z;
			}
		}
		trans_func_z[r] = (G)best_z;
	}

	// 실제 픽셀 매핑
	for (int i = 0; i < input.rows; i++) {
		for (int j = 0; j < input.cols; j++) {
			matched.at<G>(i, j) = trans_func_z[input.at<G>(i, j)];
		}
	}
}