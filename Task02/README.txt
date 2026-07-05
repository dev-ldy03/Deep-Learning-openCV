=========================================================
  Assignment 02
  컴퓨터공학과 | 2317022 이다영
=========================================================


[목적]
---------------------------------------------------------
본 프로젝트는 OpenCV와 C++을 사용하여 히스토그램 기반의 영상 처리 기술을 구현한다. 영상의 밝기 분포(PDF/CDF)를 분석하고, 다음의 세 가지 방법을 통해 영상의 명암비(Contrast)를 개선하는 것을 목표로 한다.

  1. 히스토그램 스트레칭(Stretching) — 저대조 영상에서 특정 밝기범위를 확장하여 명암비를 개선한다.

  2. 히스토그램 평활화(Equalization) — 사용자 정의 매개변수 없이 자동으로 밝기 분포를 재배치하여 균일한 분포를 만든다.

  3. 히스토그램 매칭(Matching) — 입력 영상의 히스토그램이 참조 영상의 분포와 유사해지도록 변환하여 타겟 분포를 제어한다.

각 기법은 그레이스케일과 컬러 영상 모두에 대해 구현되었다. 컬러 영상의 경우, 색상 왜곡을 방지하기 위해 YUV 색 공간을 사용하며 밝기 정보인 Y 채널에 대해서만 평활화 및 매칭을 적용한다.


[파일 구성]
---------------------------------------------------------
  hist_func.h            공통 헤더: PDF, CDF 계산 및 그래프 출력(plot)
  PDF_CDF.cpp            PDF/CDF 계산 및 시각화
  hist_stretching.cpp    히스토그램 스트레칭 구현
  hist_eq.cpp            그레이스케일 히스토그램 equalization
  hist_eq_RGB.cpp        RGB 채널별 독립적 equalization
  hist_eq_YUV.cpp        YUV Y채널 기준 equalization
  hist_matching_gray.cpp 그레이스케일 히스토그램 매칭
  hist_matching_color.cpp 컬러 히스토그램 매칭 


[필요 입력 영상]
---------------------------------------------------------
다음 사진 파일들을 실행 파일과 동일한 디렉토리(또는 Visual Studio프로젝트 디렉토리)에 배치해야 한다.

  input.jpg              기본 입력 영상
                         → hist_matching_*.cpp를 제외한 모든 프로그램의 입력으로 사용되며, 
			   매칭 프로그램에서는 참조 영상으로 사용됨.

  input_image_flower.jpg 매칭 프로그램용 입력 영상
                         → hist_matching_gray.cpp 및
                           hist_matching_color.cpp에서 매칭될
                           원본 영상으로 사용됨.


[실행 방법]
---------------------------------------------------------
Step 1. Visual Studio를 실행하고 새 C++ 프로젝트를 생성한다.
        (또는 기존 .sln 파일을 연다.)

Step 2. OpenCV 환경 설정 (프로젝트당 1회 수행):
        - 프로젝트 속성 -> C/C++ -> 추가 포함 디렉터리:
          OpenCV include 폴더 경로 추가 (예: C:\opencv\build\include)
        - 프로젝트 속성 -> 링커 -> 추가 라이브러리 디렉터리:
          OpenCV lib 폴더 경로 추가 (예: C:\opencv\build\x64\vc17\lib)
        - 프로젝트 속성 -> 링커 -> 입력 -> 추가 종속성:
            Debug   : opencv_world4XXd.lib
            Release : opencv_world4XX.lib
          (4XX 부분은 본인의 OpenCV 버전 숫자로 대체한다.)

Step 3. 입력 영상 파일들을 .vcxproj 파일이 있는 폴더에 배치한다.

Step 4. 실행하고자 하는 .cpp 파일을 활성화한다.
        (한 번에 하나의 main()만 컴파일 가능하므로, 사용하지 않는
        다른 .cpp 파일들은 우클릭 -> 속성 -> 일반 -> 빌드에서 제외를
        '예'로 설정한다.)

Step 5. 빌드 및 실행 (F5 또는 Ctrl+F5).
        화면에 결과 영상 창이 나타나며, 프로젝트 디렉토리에 결과물인
        .txt 파일들이 저장된다.

Step 6. 영상 창에서 아무 키나 누르면 프로그램이 종료된다.


[매개변수 조정 방법]
---------------------------------------------------------
대부분의 프로그램은 매개변수 없이 자동으로 실행되나,
hist_stretching.cpp만 수동 매개변수 입력이 필요하다.

---- hist_stretching.cpp --------------------------------

  함수 호출 (main 함수 내부):
    linear_stretching(input_gray, stretched,
                      trans_func_stretch, x1, x2, y1, y2);

  매개변수 설명:
    x1  : 스트레칭할 입력 밝기 범위의 하한값
    x2  : 스트레칭할 입력 밝기 범위의 상한값
    y1  : 출력 밝기 범위의 하한값 (x1이 매핑될 값)
    y2  : 출력 밝기 범위의 상한값 (x2가 매핑될 값)

  변환 함수(Transfer Function)는 다음과 같이 구간별로 정의된다.

    [0,  x1] : y = (y1/x1) * x
    [x1, x2] : y = ((y2-y1)/(x2-x1)) * (x-x1) + y1
    [x2, 255]: y = ((255-y2)/(255-x2)) * (x-x2) + y2

  기본 매개변수 설정 팁:
    먼저 PDF_CDF.cpp를 실행하여 출력된 PDF 그래프를 확인한다.
    픽셀이 집중적으로 분포된 밝기 범위를 찾아 [x1, x2]로 설정하고,
    이를 확장시키고 싶은 더 넓은 범위를 [y1, y2]로 설정한다.

  본 코드에 설정된 기본값:
    x1=50, x2=110, y1=10, y2=110
    → input.jpg의 PDF 그래프상에서 대부분의 픽셀이 50~110 사이에
       집중되어 있으므로, 이 구간을 [10, 110]으로 확장하여 명암비를
       높이도록 설정하였다.

  기타 예시 (코드 내 주석 처리됨):
    x1=120, x2=150, y1=120, y2=200
    → 중고조 영역에 픽셀이 집중된 영상(예: lena.jpg)에 적합하다.

---- 기타 프로그램 (매개변수 조정 불필요) ----

  hist_eq.cpp, hist_eq_RGB.cpp, hist_eq_YUV.cpp:
    완전 자동 실행된다. 변환 함수 s = (L-1) * CDF(r)이
    입력 영상으로부터 직접 계산된다.

  hist_matching_gray.cpp, hist_matching_color.cpp:
    참조 영상을 변경하려면 main()의 imread() 호출 부분을 수정한다.
      Mat reference = imread("원하는_참조_영상.jpg", IMREAD_COLOR);
    매칭 알고리즘은 두 영상의 CDF를 기반으로 매핑을 자동 계산한다.


[생성되는 출력 파일]
---------------------------------------------------------
  PDF_CDF.cpp         -> PDF.txt, CDF.txt
  hist_stretching.cpp -> PDF.txt, stretched_PDF.txt,
                         trans_func_stretch.txt,
                         hist.txt, stretched_hist.txt
  hist_eq.cpp         -> PDF.txt, equalized_PDF_gray.txt,
                         trans_func_eq.txt,
                         hist.txt, equalized_hist.txt
  hist_eq_RGB.cpp     -> PDF_RGB.txt,
                         equalized_PDF_RGB.txt,
                         trans_func_eq_RGB.txt
  hist_eq_YUV.cpp     -> PDF_YUV.txt,
                         equalized_PDF_YUV.txt,
                         trans_func_eq_YUV.txt,
                         hist_YUV.txt,
                         equalized_hist_YUV.txt
  hist_matching_gray  -> matching_function_gray.txt,
                         matching_original_hist_gray.txt,
                         matched_hist_gray.txt
  hist_matching_color -> matching_function_color.txt,
                         matching_original_hist_color.txt,
                         matched_hist_color.txt


[참고 사항]
---------------------------------------------------------
- OpenCV 4.x 이상 버전이 필요하다. v3 대비 다음과 같은 API
  변경 사항이 프로젝트 전체에 적용되었다.
    CV_LOAD_IMAGE_COLOR  ->  IMREAD_COLOR
    CV_RGB2GRAY          ->  COLOR_RGB2GRAY
    CV_RGB2YUV           ->  COLOR_BGR2YUV

- cal_PDF(), cal_CDF() 등에서 할당된 메모리는 호출한 쪽에서
  반드시 free()를 통해 해제해야 한다. 2차원 배열(RGB 버전)의
  경우, 각 행을 먼저 해제한 후 배열 포인터를 해제한다.
    for (int i = 0; i < L; i++) free(arr[i]);
    free(arr);

- technical report 에 언급하지 못한 디테일한 코드에 대한 부연설명은 주석으로 처리하였다

=========================================================