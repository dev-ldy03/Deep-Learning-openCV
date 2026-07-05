#include "hist_func.h"

void linear_stretching(Mat& input, Mat& stretched, G* trans_func, G x1, G x2, G y1, G y2);

int main() {

	Mat input = imread("input.jpg", IMREAD_COLOR);

	// 이미지 로드 확인용 예외 처리
	if (input.empty()) return -1;

	Mat input_gray;
	cvtColor(input, input_gray, COLOR_BGR2GRAY);	// convert RGB to Grayscale

	Mat stretched = input_gray.clone();

	// PDF or transfer function txt files
	FILE* f_PDF;
	FILE* f_stretched_PDF;
	FILE* f_trans_func_stretch;

	fopen_s(&f_PDF, "PDF.txt", "w+");
	fopen_s(&f_stretched_PDF, "stretched_PDF.txt", "w+");
	fopen_s(&f_trans_func_stretch, "trans_func_stretch.txt", "w+");

	G trans_func_stretch[L] = { 0 };

	float* PDF = cal_PDF(input_gray);

	linear_stretching(input_gray, stretched, trans_func_stretch, 50, 110, 10, 110);	// histogram stretching (x1 ~ x2 -> y1 ~ y2)
	// linear_stretching(input_gray, stretched, trans_func_stretch, 120, 150, 120, 200);	// histogram stretching (x1 ~ x2 -> y1 ~ y2)
	float* stretched_PDF = cal_PDF(stretched);										// stretched PDF

	for (int i = 0; i < L; i++) {
		// write PDF
		fprintf(f_PDF, "%d\t%f\n", i, PDF[i]);
		fprintf(f_stretched_PDF, "%d\t%f\n", i, stretched_PDF[i]);

		// write transfer functions
		fprintf(f_trans_func_stretch, "%d\t%d\n", i, trans_func_stretch[i]);
	}

	plot(PDF, "PDF", "PDF");
	plot(stretched_PDF, "PDF", "PDF(stretching)");

	// memory release
	free(PDF);
	free(stretched_PDF);
	fclose(f_PDF);
	fclose(f_stretched_PDF);
	fclose(f_trans_func_stretch);

	////////////////////// histogram ///////////////////////
	FILE* f_hist;
	FILE* f_stretched_hist;

	fopen_s(&f_hist, "hist.txt", "w+");
	fopen_s(&f_stretched_hist, "stretched_hist.txt", "w+");

	int histSize = L;
	float ran[] = { 0, 256 };
	const float* range = { ran };

	Mat hist, stretched_hist;
	calcHist(&input_gray, 1, 0, Mat(), hist, 1, &histSize, &range);
	calcHist(&stretched, 1, 0, Mat(), stretched_hist, 1, &histSize, &range);

	for (int i = 0; i < histSize; i++) {
		fprintf(f_hist, "%d\t%f\n", i, hist.at<float>(i));
		fprintf(f_stretched_hist, "%d\t%f\n", i, stretched_hist.at<float>(i));
	}

	fclose(f_hist);
	fclose(f_stretched_hist);

	////////////////////// Show each image ///////////////////////

	namedWindow("Grayscale", WINDOW_AUTOSIZE);
	imshow("Grayscale", input_gray);

	namedWindow("Stretched", WINDOW_AUTOSIZE);
	imshow("Stretched", stretched);

	//////////////////////////////////////////////////////////////

	waitKey(0);

	return 0;
}

// histogram stretching (linear method)
void linear_stretching(Mat& input, Mat& stretched, G* trans_func, G x1, G x2, G y1, G y2) {

	// 구간별 기울기 계산 (정수 나눗셈 방지를 위해 float 형변환 추가)
	float s1 = (float)y1 / x1;
	float constant = (y2 - y1) / (float)(x2 - x1);
	float s3 = (float)(L - 1 - y2) / (L - 1 - x2); // 기울기 수식 오류 수정 (분자/분모 위치 교정)

	// compute transfer function
	for (int i = 0; i < L; i++) {	// 전체 픽셀단위
		if (i >= 0 && i <= x1)		// 0~x1이면
			trans_func[i] = (G)(s1 * i);
		else if (i > x1 && i <= x2)
			trans_func[i] = (G)(constant * (i - x1) + y1);
		else
			// 마지막 구간 매핑 수식 적용
			trans_func[i] = (G)(s3 * (i - x2) + y2);
	}

	// perform the transfer function
	// 계산된 변환 함수(trans_func)를 실제 이미지 픽셀에 적용
	for (int i = 0; i < input.rows; i++)
		for (int j = 0; j < input.cols; j++)
			stretched.at<G>(i, j) = trans_func[input.at<G>(i, j)];
}