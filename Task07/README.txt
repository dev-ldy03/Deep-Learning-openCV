========================================
 README - Assignment 07 
 컴퓨터공학 이다영(2317022)
========================================

----------------------------------------
1. 코드 목적 
----------------------------------------

[SIFT.cpp]
두 이미지 간의 SIFT 기반 키포인트 매칭을 수행하는 코드이다.
OpenCV의 SIFT 라이브러리를 사용해 두 이미지의 키포인트와 디스크립터를
추출하고, Nearest Neighbor(NN) 방식으로 대응점을 찾는다.
Cross-checking 및 Ratio-based thresholding 옵션을 조합하여
세 가지 케이스(Case 1/2/3)의 매칭 결과를 비교 실험한다.
For 문과 같은 반복문으로 케이스 3가지를 모두 실행되도록 구현하지 않고 기존 스켈레톤 코드를 유지하는 방향으로
코드 실험을 할때 bool 값을 바꿔가면서 실험하여 technical report에 반영하였다. 
더불어 코드 주석으로도 어떻게 케이스 3가지를 실험했는지 언급해 두었다. 

[SURF.cpp]
SURF 기반 키포인트 매칭과 Homography 추정을 수행하는 코드이다.
FLANN 매처로 두 이미지의 대응점을 찾고, RANSAC 기반의 Homography 행렬을
계산하여 배경 이미지에서 객체 이미지의 위치를 검출하고 시각화한다.


----------------------------------------
2. 실행 방법
----------------------------------------

※ OpenCV 4.x 이상이 설치되어 있어야 합니다.

[빌드]
  g++ SIFT.cpp -o SIFT `pkg-config --cflags --libs opencv4`
  g++ SURF.cpp -o SURF `pkg-config --cflags --libs opencv4`

[실행]
  ./SIFT
  ./SURF

※ 실행 파일과 동일한 디렉토리에 input1.jpg, input2.jpg가 있어야 한다.
※ 과제 제출 zip 파일에 input1.jpg, input2.jpg 모두 포함했다.


----------------------------------------
3. 파라미터 조정 방법
----------------------------------------

[SIFT.cpp - 매칭 옵션 조정]
main() 함수 내 아래 두 변수의 true/false 값을 변경하여
세 가지 실험 케이스를 재현할 수 있다.

  bool crossCheck = true;       // cross-checking 적용 여부
  bool ratio_threshold = true;  // ratio-based thresholding 적용 여부

  - Case 1 : crossCheck = false, ratio_threshold = false
  - Case 2 : crossCheck = true,  ratio_threshold = false
  - Case 3 : crossCheck = true,  ratio_threshold = true

[SURF.cpp - Hessian Threshold 조정]
main() 함수 내 아래 변수를 변경하여 검출되는 키포인트 수를 조절할 수 있다.
값이 클수록 키포인트가 적게 검출된다.

  int minHessian = 400;


----------------------------------------
4. 기본 파라미터 설정 
----------------------------------------

[SIFT.cpp]
  - nFeatures          : 0       (검출 키포인트 수 제한 없음)
  - nOctaveLayers      : 4       (옥타브당 레이어 수)
  - contrastThreshold  : 0.04    (낮은 대비 키포인트 제거 기준)
  - edgeThreshold      : 10      (에지 응답 제거 기준)
  - sigma              : 1.6     (가우시안 필터 표준편차)
  - RATIO_THR          : 0.4     (ratio-based thresholding 기준값)
  - crossCheck         : true    (현재 Case 3 기준)
  - ratio_threshold    : true    (현재 Case 3 기준)

[SURF.cpp]
  - minHessian         : 400     (Hessian 키포인트 검출 threshold)
  - good match 기준    : distance ≤ 3 × min_dist
  - Homography 추정    : RANSAC 방식 사용

========================================
