#pragma once

#include <opencv2/opencv.hpp>
#include <stdio.h>

// 기본 설정
#define L 256		// # of intensity levels -> 8 bit image는 0~225 단계
#define IM_TYPE	CV_8UC3 // 이미지 타입 설정

using namespace cv;
using namespace std;    // 추가

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

// 함수1 : Grayscale PDF 계산
// generate PDF for single channel image
float* cal_PDF(Mat& input) {

    int count[L] = { 0 }; // 각 밝기 값(0~255)의 출현 횟수를 저장할 빈 통
    float* PDF = (float*)calloc(L, sizeof(float)); // PDF 값을 저장할 메모리 할당 (초기값 0.0)

    // Count 
    // 픽셀 조사해서 빈도수 세기
    for (int i = 0; i < input.rows; i++)
        for (int j = 0; j < input.cols; j++)
            count[input.at<G>(i, j)]++;

    // Compute PDF
    // 전체 픽셀수 (rows*cols)로 나누어서 확률값 도출 -> normalization
    for (int i = 0; i < L; i++)
        PDF[i] = (float)count[i] / (float)(input.rows * input.cols);

    return PDF;
}

// 함수2 RGB PDF 계산
// generate PDF for color image
// 이때 출력은 float** (256행 3열의 2차원 배열)
float** cal_PDF_RGB(Mat& input) {

    int count[L][3] = { 0 };
    float** PDF = (float**)malloc(sizeof(float*) * L);

    for (int i = 0; i < L; i++)
        PDF[i] = (float*)calloc(3, sizeof(float));

    ////////////////////////////////////////////////
    //											  //
    // How to access multi channel matrix element //
    //											  //
    // if matrix A is CV_8UC3 type,				  //
    // A(i, j, k) -> A.at<Vec3b>(i, j)[k]		  //
    //											  //
    ////////////////////////////////////////////////

    // Count
    // OpenCV의 컬러는 BGR 순서로 [0], [1], [2] 인덱스를 가짐
    // [0] = B intensity, [1] = G intensity, [2] = R intensity
    for (int i = 0; i < input.rows; i++) {
        for (int j = 0; j < input.cols; j++) {
            for (int k = 0; k < 3; k++) {   
                count[input.at<Vec3b>(i, j)[k]][k]++; // count[intensity][channel]
            }
        }
    }

    // Compute PDF
    for (int i = 0; i < L; i++) {
        for (int k = 0; k < 3; k++) {
            PDF[i][k] = (float)count[i][k] / (float)(input.rows * input.cols);
        }
    }


    return PDF;
}

// 함수3: grayscale CDF 계산
// 누적 분포 함수: PDF를 누적으로 합산한 값 (0.0에서 시작해 마지막 인덱스는 반드시 1.0)
// generate CDF for single channel image
float* cal_CDF(Mat& input) {

    int count[L] = { 0 };
    float* CDF = (float*)calloc(L, sizeof(float));

    // Count
    for (int i = 0; i < input.rows; i++)
        for (int j = 0; j < input.cols; j++)
            count[input.at<G>(i, j)]++;

    // Compute CDF
    for (int i = 0; i < L; i++) {
        CDF[i] = (float)count[i] / (float)(input.rows * input.cols); 

        if (i != 0) {   
            CDF[i] += CDF[i - 1]; // 이전 단계의 확률값을 더해 누적
        }
    }

    return CDF;
}

// 함수 4: 컬러 이미지(RGB) CDF 계산
// generate CDF for color image
float** cal_CDF_RGB(Mat& input) {
    int count[L][3] = { 0 };
    float** CDF = (float**)malloc(sizeof(float*) * L);

    for (int i = 0; i < L; i++)
        CDF[i] = (float*)calloc(3, sizeof(float));

    // Count
    for (int i = 0; i < input.rows; i++) {
        for (int j = 0; j < input.cols; j++) {
            for (int k = 0; k < 3; k++) {
                count[input.at<Vec3b>(i, j)[k]][k]++;
            }
        }
    }

    // Compute CDF
    for (int k = 0; k < 3; k++) {
        for (int i = 0; i < L; i++) {   
            CDF[i][k] = (float)count[i][k] / (float)(input.rows * input.cols);

            if (i != 0) {  
                CDF[i][k] += CDF[i - 1][k];
            }
        }
    }

    return CDF;
}

////////////////////// Plot the graph ///////////////////////
// 그래프 시각화 (plot)
// 함수값을 선 그래프 형태로 출력함
void drawGraph(Mat& graph, int x, int y, int w, int h, int margin, float maxSize, const string& opt, string title);

void plot(float* function, string opt, string title) {
    int x = 512;   // 그래프 가로 길이
    int y = 250;   // 그래프 세로 높이
    int margin = 50;        // margin
    int w = x + margin * 2; // width
    int h = y + margin * 2; // height

    Mat graph = Mat::zeros(h, w, CV_8UC3); // 검은색 화면 생성
    float maxSize;

    if (opt == "PDF") {
        maxSize = 0.025f; // PDF 는 확률값이므로 Y축 범위를 작게 잡음 (기본 2.5%)

        for (int i = 0; i < L; i++) {
            if (function[i] > maxSize) {
                maxSize = function[i];   // 최댓값이 버위를 넘으면 조정
                h += maxSize;
            }
        }
    }
    else {
        maxSize = 1.2f; // CDF는 최대값이 1.0이므로 1.2를 기준으로 잡음
    }

    // 데이터 값을 좌표로 변환하여 선 그리기
    for (int i = 0; i < L; i++) {   
        int pointX = margin + cvRound((i / 255.0) * x); // 0~255 밝기를 0~512 좌표로 매핑
        int pointY = margin + y - cvRound((function[i] / maxSize) * y);  // 값을 높이 좌표로 매핑

        line(graph, Point(pointX, y + margin), Point(pointX, pointY), Scalar(255, 255, 255));
    }

    drawGraph(graph, x, y, w, h, margin, maxSize, opt, title);

    namedWindow(title, WINDOW_AUTOSIZE);
    imshow(title, graph);
}

/*
    y축 그리기
    x축: PDF, CDF 동일(0~225)
    y축: 
        PDF: 0.005 간격 0~0.025
        CDF: 0.2 간격 0.0~1.2
    */

// 그래프 축 및 텍스트 표시 함수
void drawGraph(Mat& graph, int x, int y, int w, int h, int margin, float maxSize, const string& opt, string title) {     
    // 그래프 제목 표시
    putText(graph, title, Point(w / 2 - 30, margin / 2), FONT_ITALIC, 0.5, Scalar(255, 255, 255), 1);   

    line(graph, Point(margin, margin + y), Point(margin + x, margin + y), Scalar(255, 255, 255), 1); // x축
    for (int i = 0; i <= 255; i += 15) {   
        int pointX = margin + cvRound((i / 255.0) * x);

        putText(graph, to_string(i), Point(pointX - 10, margin + y + 10), FONT_ITALIC, 0.4, Scalar(255, 255, 255), 1);
    }

    // y축
    line(graph, Point(margin, margin), Point(margin, margin + y), Scalar(255, 255, 255), 1); 
    if (opt == "PDF") { 
        vector<float> label = {};
        float iPos = maxSize / 6;
        for (float i = 0.000; i <= maxSize; i += iPos) {
            label.push_back(i);
        }
        

        for (int i = 0; i < label.size(); i++) {   
            int pointY = margin + y - cvRound((label[i] / maxSize) * y);    
            putText(graph, to_string(label[i]).substr(0, 5), Point(margin - 40, pointY + 5), FONT_ITALIC, 0.4, Scalar(255, 255, 255), 1);
        }
    }
    else {
        float label[] = { 0.0f, 0.2f, 0.4f, 0.6f, 0.8f, 1.0f, 1.2f };   

        for (int i = 0; i < 7; i++) {
            int pointY = margin + y - cvRound((label[i] / maxSize) * y);

            putText(graph, to_string(label[i]), Point(margin - 40, pointY + 5), FONT_ITALIC, 0.4, Scalar(255, 255, 255), 1);
        }
    }
}