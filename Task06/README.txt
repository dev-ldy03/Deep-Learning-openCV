================================================================================
  Assignment 06 - Readme
  컴퓨터공학과 이다영 2317022
================================================================================

--------------------------------------------------------------------------------
1. 코드 목적
--------------------------------------------------------------------------------
  LoG.cpp
    - 가우시안 필터로 이미지를 smoothing한 뒤 라플라시안 필터를 적용하여
      에지를 검출한다. (Laplacian of Gaussian)
    - grayscale 및 컬러 이미지 모두 처리 가능하며, 미러링으로 경계를 처리한다.

  Canny.cpp
    - OpenCV 제공 Canny() 함수를 사용해 에지를 검출한다.
    - Gaussian smoothing -> gradient 계산 -> non-maximum suppression
      -> double thresholding 순서로 동작한다.

  Harris_corner.cpp
    - OpenCV 제공 cornerHarris() 함수를 사용해 코너를 검출한다.
    - Non-maximum suppression 및 subpixel refinement 옵션을 제공한다.

--------------------------------------------------------------------------------
2. 실행 방법
--------------------------------------------------------------------------------
  (1) 입력 이미지를 빌드된 실행파일(.exe)과 같은 폴더에 위치시킨다.
      사용 이미지: lena.jpg, checkerboard.png, checkerboard2.jpg

  (2) Visual Studio에서 솔루션을 빌드한다. (Ctrl+Shift+B 또는 Rebuild Solution)

  (3) 실행한다. (Ctrl+F5)
      결과 윈도우가 출력되며, 아무 키나 누르면 종료된다.

  주의) stdafx.h 관련
    LoG.cpp와 Harris_corner.cpp 상단에 아래 줄이 주석 처리되어 있다.

      // #include "stdafx.h"

    stdafx.h는 Visual Studio의 프리컴파일 헤더로, 프로젝트 설정에 따라
    필요할 수도 있고 필요 없을 수도 있다.
    - 빌드 오류가 발생하지 않는다면 주석 상태로 두면 된다.
    - "unexpected end of file while looking for precompiled header" 오류가
      발생하면 주석을 해제하거나, 프로젝트 속성에서
      [C/C++ -> 미리 컴파일된 헤더 -> 미리 컴파일된 헤더 사용 안 함]으로
      설정을 변경한다.
    - macOS에서 컴파일할 경우 해당 줄은 주석 상태로 두어야 한다.

--------------------------------------------------------------------------------
3. 파라미터 조정 방법
--------------------------------------------------------------------------------

[LoG.cpp] - main() 상단에서 변경
  int    window_radius = 2    가우시안 커널 반지름 -> 커널 크기 (2N+1)x(2N+1)
  double sigma_t       = 2.0  x 방향 표준편차 (값이 클수록 더 많이 흐려짐)
  double sigma_s       = 2.0  y 방향 표준편차 (값이 클수록 더 많이 흐려짐)

[Canny.cpp] - main() 내 변수 선언부에서 변경
  double threshold1   = 50.0   낮은 hysteresis threshold (T_L)
  double threshold2   = 150.0  높은 hysteresis threshold (T_H)
  int    apertureSize = 3      Sobel 필터 커널 크기 (3, 5, 7 중 선택)
  bool   L2gradient   = false  true: L2 norm 사용 / false: L1 norm 사용

[Harris_corner.cpp] - main() 내 변수 선언부에서 변경
  int    blockSize    = 2      Harris 응답 계산 윈도우 크기
  int    ksize        = 3      Sobel 필터 커널 크기
  double k            = 0.04   Harris free parameter (alpha, 보통 0.04~0.06)
  bool   NonMaxSupp   = true   Non-maximum suppression 실행 여부
  bool   Subpixel     = true   Subpixel refinement 실행 여부

  이미지 변경 방법:
    main() 상단의 imread() 3줄 중 원하는 이미지 줄의 주석을 해제하고
    나머지 두 줄을 주석 처리한 뒤 다시 빌드한다.

    // Mat input = imread("checkerboard.png",  IMREAD_COLOR);
    // Mat input = imread("checkerboard2.jpg", IMREAD_COLOR);
       Mat input = imread("lena.jpg",          IMREAD_COLOR);  <- 현재 활성

--------------------------------------------------------------------------------
4. 기본 파라미터 설정값 (Default Parameters)
--------------------------------------------------------------------------------
  LoG.cpp
    window_radius = 2,  sigma_t = 2.0,  sigma_s = 2.0

  Canny.cpp
    threshold1 = 50.0,  threshold2 = 150.0,
    apertureSize = 3,   L2gradient = false

  Harris_corner.cpp
    blockSize = 2,  ksize = 3,  k = 0.04,
    NonMaxSupp = true,  Subpixel = true
    입력 이미지: lena.jpg

  파라미터 변경 후에는 반드시 Rebuild Solution 후 실행해야 반영된다.

================================================================================
