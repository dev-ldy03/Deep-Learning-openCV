// opencv_test.cpp : Defines the entry point for the console application.

// 필요한 경우 주석을 풀고 코드를 돌려야 함.
// #include "pch.h" 
//#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include "opencv2/highgui.hpp"
#include <iostream>
#include <string>

using namespace cv;
using namespace std;

void getSegment(const Mat image, const Vec2f& lines, int minLineLength, int maxLineGap, Mat& colorDst);

int main() {

	Mat src = imread("building.jpg", IMREAD_COLOR);
	Mat dst, color_dst;

	// check for validation
	if (!src.data) {
		printf("Could not open\n");
		return -1;
	}

	Canny(src, dst, 50, 200, 3);
	cvtColor(dst, color_dst, COLOR_GRAY2BGR);
	// Mat color_dst_test = color_dst.clone();	// 수동 segmentation 테스트

	//Standard Hough transform (using 'HoughLines')
#if 1
	vector<Vec2f> lines;
	//Fill this line

	HoughLines(dst, lines, 1, CV_PI / 180, 180); // 추후에 threshold값 바꿔가면서 실험

	for (size_t i = 0; i < lines.size(); i++)
	{
		float rho = lines[i][0];
		float theta = lines[i][1];
		double a = cos(theta), b = sin(theta);
		double x0 = a * rho, y0 = b * rho;
		Point pt1(cvRound(x0 + 1000 * (-b)), cvRound(y0 + 1000 * (a)));
		Point pt2(cvRound(x0 - 1000 * (-b)), cvRound(y0 - 1000 * (a)));
		line(color_dst, pt1, pt2, Scalar(0, 0, 255), 3, 8);
	}

	// 수동 segmentation 테스트
	// if (true) {
	// 	for (size_t i = 0; i < lines.size(); i++)
	// 	{
	// 		getSegment(dst, lines[i], 20, 50, color_dst_test);
	// 	}
	// 	namedWindow("Detected Lines 2", 1);
	// 	imshow("Detected Lines 2", color_dst_test);

	// }
	//Probabilistic Hough transform (using 'HoughLinesP')
#else
	vector<Vec4i> lines;
	//Fill this line
	HoughLinesP(dst, lines, 1, CV_PI / 180, 50, 50, 10);

	for (size_t i = 0; i < lines.size(); i++)
	{
		line(color_dst, Point(lines[i][0], lines[i][1]),
			Point(lines[i][2], lines[i][3]), Scalar(0, 0, 255), 3, 8);
	}
#endif
	namedWindow("Source", 1);
	imshow("Source", src);
	namedWindow("Detected Lines", 1);
	imshow("Detected Lines", color_dst);
	waitKey(0);

	return 0;
}

void getSegment(const Mat image, const Vec2f& lines, int minLineLength, int maxLineGap, Mat& colorDst) {

	Mat output = image.clone();	// result image

	float rho = lines[0];
	float theta = lines[1];
	double a = cos(theta), b = sin(theta);
	double x0 = a * rho, y0 = b * rho;
	Point pt1(cvRound(x0 + 1000 * (-b)), cvRound(y0 + 1000 * (a)));
	Point pt2(cvRound(x0 - 1000 * (-b)), cvRound(y0 - 1000 * (a)));

	LineIterator it(image, pt1, pt2, 8);	// LineIterator: pixel iteration within a line
	int gap = 0;	// gap count
	int count = 0;	// line segment count
	Point start;

	for (int i = 0; i < it.count; i++, ++it) {
		Point p = it.pos();	// current pixel point
		if (p.x < 0 || p.x >= image.cols || p.y < 0 || p.y >= image.rows) {
			continue;	// if out of image range, skip
		}

		bool isEdge = image.at<uchar>(p) == 255;	

		if (isEdge) {	
			if (count == 0) {	// if line segment's first pixel
				start = p;
			}
			gap++;
			count++;
		}
		// not an edge pixel
		else if (count > 0) {	// line segment exists
			gap++;	// gap count ����
			if (gap == maxLineGap) {	// gap이 maxLineGap과 같으면 line segment 아님.
				if (count >= minLineLength) {	// line segment가 존재하고 길이가 minLineLength보다 큰 경우
					Point end = Point( 
						it.pos().x - cvRound(gap * (-b)),
						it.pos().y - cvRound(gap * (a))
					);
					line(colorDst, start, end, Scalar(0, 0, 255), 3, 8);
				}
				count = 0;	// line segment count initialize
				gap = 0;	// gap count initialize

			}
		}
	}// end of iteration
	if (count >= minLineLength) {
		Point end = it.pos();
		line(colorDst, start, end, Scalar(0, 0, 255), 3, 8);
	}
}
