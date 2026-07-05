#include "hist_func.h"

// 함수 선언
void hist_eq(Mat& input, Mat& equalized, G* trans_func, float* CDF);
// 매칭 함수: 입력 영상의 평활화 함수(T)와 참조 영상의 평활화 함수(G_map)를 모두 사용한다.
void hist_matching(Mat& input, Mat& matched, G* trans_func_z, G* T, G* G_map);

int main() {
	// 이미지 읽기 
	Mat input = imread("input_image_flower.jpg", IMREAD_COLOR);
	Mat reference = imread("input.jpg", IMREAD_COLOR);

	if (input.empty() || reference.empty()) return -1;

	//  Grayscale 변환 (OpenCV 기본 BGR 기준)
	Mat input_gray, reference_gray;
	cvtColor(input, input_gray, COLOR_BGR2GRAY);
	cvtColor(reference, reference_gray, COLOR_BGR2GRAY);

	// 각 영상의 CDF 및 Equalization함수 계산
	float* CDF_input = cal_CDF(input_gray);
	float* CDF_ref = cal_CDF(reference_gray);

	G trans_func_input[L] = { 0 }; // T(r)
	G trans_func_ref[L] = { 0 };   // G(z)

	Mat equalized_input = input_gray.clone();
	Mat equalized_ref = reference_gray.clone();

	// Equalization 함수를 통해 T(r)과 G(z) 테이블을 생성함
	hist_eq(input_gray, equalized_input, trans_func_input, CDF_input);
	hist_eq(reference_gray, equalized_ref, trans_func_ref, CDF_ref);

	// 히스토그램 매칭 수행
	Mat matched = input_gray.clone(); // 결과 저장용
	G trans_func_z[L] = { 0 };        // 최종 매핑 함수 r -> z

	// Equalization된 이미지가 아닌 원본 gray 이미지를 넣어 r -> z로 바로 매핑함
	hist_matching(input_gray, matched, trans_func_z, trans_func_input, trans_func_ref);

	// 결과 데이터
	FILE* f_trans, * f_hist, * f_matched_hist;
	fopen_s(&f_trans, "matching_function_gray.txt", "w+");
	fopen_s(&f_hist, "matching_original_hist_gray.txt", "w+");
	fopen_s(&f_matched_hist, "matched_hist_gray.txt", "w+");

	for (int i = 0; i < L; i++) {
		fprintf(f_trans, "%d\t%d\n", i, trans_func_z[i]);
	}

	// 히스토그램 계산 및 저장
	int histSize = L;
	float ran[] = { 0, 256 };
	const float* range = { ran };
	Mat hist, m_hist;
	calcHist(&input_gray, 1, 0, Mat(), hist, 1, &histSize, &range);
	calcHist(&matched, 1, 0, Mat(), m_hist, 1, &histSize, &range);

	for (int i = 0; i < histSize; i++) {
		fprintf(f_hist, "%d\t%f\n", i, hist.at<float>(i));
		fprintf(f_matched_hist, "%d\t%f\n", i, m_hist.at<float>(i));
	}

	// 메모리 해제
	free(CDF_input);
	free(CDF_ref);
	fclose(f_trans);
	fclose(f_hist);
	fclose(f_matched_hist);

	// 화면 출력 - window 크기 조정 로직 
	namedWindow("Input", WINDOW_NORMAL);
	resizeWindow("Input", 500, 500);
	imshow("Input", input_gray);

	namedWindow("Reference", WINDOW_NORMAL);
	resizeWindow("Reference", 500, 500);
	imshow("Reference", reference_gray);

	namedWindow("Matched (Gray)", WINDOW_NORMAL);
	resizeWindow("Matched (Gray)", 500, 500);
	imshow("Matched (Gray)", matched);

	waitKey(0);
	return 0;
}

// 히스토그램 Equalization 로직 (변환 함수 T 생성)
void hist_eq(Mat& input, Mat& equalized, G* trans_func, float* CDF) {
	for (int i = 0; i < L; i++)
		trans_func[i] = (G)((L - 1) * CDF[i]);

	for (int i = 0; i < input.rows; i++)
		for (int j = 0; j < input.cols; j++)
			equalized.at<G>(i, j) = trans_func[input.at<G>(i, j)];
}

// 히스토그램 매칭 로직: z = G^-1(T(r))
void hist_matching(Mat& input, Mat& matched, G* trans_func_z, G* T, G* G_map) {
	// 모든 밝기 r(0~255)에 대해 최적의 z를 찾음
	for (int r = 0; r < L; r++) {
		int s = (int)T[r]; // 입력의 평활화된 밝기 값
		int best_z = 0;
		int min_diff = L;

		// 참조 영상의 변환 값들 중 s와 가장 가까운 값을 가진 인덱스 z를 검색
		for (int z = 0; z < L; z++) {
			int diff = abs((int)G_map[z] - s);
			if (diff < min_diff) {
				min_diff = diff;
				best_z = z;
			}
			if (diff == 0) break; // 완벽히 일치하면 중단
		}
		trans_func_z[r] = (G)best_z; // r을 z로 매핑하는 테이블 완성
	}

	// 실제 이미지 픽셀 매핑
	for (int i = 0; i < input.rows; i++) {
		for (int j = 0; j < input.cols; j++) {
			matched.at<G>(i, j) = trans_func_z[input.at<G>(i, j)];
		}
	}
}