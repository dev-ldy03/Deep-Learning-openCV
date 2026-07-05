================================================================
  Assignment 08 - README
  컴퓨터공학과 2317022 이다영
================================================================

----------------------------------------------------------------
  1. 코드 목적
----------------------------------------------------------------

[Hough.cpp]
- Canny 에지 검출 후 Hough 변환으로 이미지에서 직선 및 선분을 검출한다.
- 표준 Hough 변환(HoughLines)과 확률적 Hough 변환(HoughLinesP) 두 가지 방식을 지원한다.
- HoughLines 결과에서 실제 에지 범위에 맞는 선분을 추출하는 getSegment() 함수를 포함한다.

[stitching.cpp]
- SIFT 알고리즘으로 두 이미지의 키포인트와 디스크립터를 추출하고 대응점을 매칭한다.
- 매칭된 대응점으로 affine 행렬을 계산하여 두 이미지를 stitching한다.
- Case1: 전체 대응점으로 affine 행렬 계산 (Mx=b)
- Case2: RANSAC으로 outlier를 배제하고 affine 행렬 계산 (Mx=b + RANSAC)

[sudo code]
- technical report pdf 파일 13페이지에 작성해 두었다. 

----------------------------------------------------------------
  2. 실행 방법 (macOS / Linux 기준)
----------------------------------------------------------------

[공통 사전 준비]
- OpenCV 설치 필요: brew install opencv
- 실행 파일과 같은 폴더에 입력 이미지 위치시키기 (과제 제출 폴더에 포함되어 있음)
  - Hough.cpp   → building.jpg 필요 
  - stitching.cpp → input1.jpg, input2.jpg 필요

[Hough.cpp 컴파일 및 실행]
  g++ -std=c++17 Hough.cpp -o Hough `pkg-config --cflags --libs opencv4`
  ./Hough

[stitching.cpp 컴파일 및 실행]
  g++ -std=c++17 stitching.cpp -o stitching `pkg-config --cflags --libs opencv4`
  ./stitching

[window 환경]
소스파일 상단에 주석 처리된 부분의 주석을 풀어서 실행해보기

----------------------------------------------------------------
  3. 파라미터 조정 방법
----------------------------------------------------------------

[Hough.cpp]

▶ HoughLines / HoughLinesP 전환
  - #if 1  → HoughLines 실행
  - #if 0  → HoughLinesP 실행 (#else 블록 활성화)

▶ HoughLines 파라미터 (main() 내부)
  HoughLines(dst, lines, rho, theta, threshold);
  - rho       : 거리 해상도 (기본값: 1 픽셀)
  - theta     : 각도 해상도 (기본값: CV_PI/180)
  - threshold : 직선 판단 최소 득표 수 (기본값: 180, 낮출수록 더 많은 직선 검출)

▶ HoughLinesP 파라미터 (#else 블록 내부)
  HoughLinesP(dst, lines, rho, theta, threshold, minLineLength, maxLineGap);
  - threshold     : 최소 득표 수 (기본값: 50)
  - minLineLength : 선분으로 인정할 최소 길이 (기본값: 50)
  - maxLineGap    : 선분 내 허용 최대 공백 (기본값: 10)

▶ getSegment 활성화 (선분 추출 테스트)
  - main() 내 아래 두 부분의 주석을 해제
    1) Mat color_dst_test = color_dst.clone();
    2) if (true) { ... } 블록 전체
  - getSegment(dst, lines[i], minLineLength, maxLineGap, color_dst_test);
    - minLineLength : 최소 선분 길이 (기본값: 10)
    - maxLineGap    : 최대 허용 공백 (기본값: 50)

[stitching.cpp]

▶ Case1 / Case2 전환
  - main() 내 stitching() 호출 부분에서 "case1", "case2" 인자로 전환
    stitching(input1, input2, srcPoints, dstPoints, "case1", k, S);
    stitching(input1, input2, srcPoints, dstPoints, "case2", k, S);

▶ RANSAC 파라미터 (stitching() 내 case2 분기)
  - k     : 랜덤 샘플 개수 (3 또는 4 비교 권장)
  - S     : 반복 횟수 (기본값: 10000)
  - delta : inlier 판단 거리 임계값 (기본값: 3 픽셀)

▶ k=3 / k=4 반복문으로 실행됨 (case2 분기)


▶ SIFT 파라미터 (main() 내부)
  SIFT::create(nFeatures, nOctaveLayers, contrastThreshold, edgeThreshold, sigma)
  - 기본값: 0, 4, 0.04, 10, 1.6

▶ 매칭 옵션 (main() 내부)
  - crossCheck      : true / false (기본값: true)
  - ratio_threshold : true / false (기본값: true, false로 변경 시 outlier 증가)

----------------------------------------------------------------
  4. 기본 파라미터 정리
----------------------------------------------------------------

  [Hough.cpp]
  - HoughLines  : rho=1, theta=CV_PI/180, threshold=180
  - HoughLinesP : rho=1, theta=CV_PI/180, threshold=50, minLineLength=50, maxLineGap=10
  - getSegment  : minLineLength=10, maxLineGap=50

  [stitching.cpp]
  - SIFT        : nFeatures=0, nOctaveLayers=4, contrastThreshold=0.04,
                  edgeThreshold=10, sigma=1.6
  - RANSAC      : k=3 or 4, S=20000, delta=3
  - blend alpha : 0.5
  - RATIO_THR   : 0.4

================================================================
