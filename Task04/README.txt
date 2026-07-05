================================================================
               Assignment 04 
               2317022 이다영
================================================================

----------------------------------------------------------------
1. 코드 목적
----------------------------------------------------------------

본 과제는 OpenCV를 이용하여 두 가지 종류의 노이즈를 추가하고,
각 노이즈에 적합한 필터로 제거하는 영상 복원 프로그램을 구현합니다.

[Gaussian.cpp]
  - Gaussian 노이즈를 영상에 추가
  - Gaussian 필터를 이용한 노이즈 제거
  - 그레이스케일 및 컬러 영상 모두 지원

[salt_and_pepper.cpp]
  - Salt & Pepper (임펄스) 노이즈를 영상에 추가
  - 미디언 필터를 이용한 노이즈 제거
  - 그레이스케일 및 컬러 영상 모두 지원

[Bilateral.cpp]
  - Gaussian 노이즈를 영상에 추가
  - Bilateral 필터를 이용한 노이즈 제거 (엣지 보존)
  - 그레이스케일 및 컬러 영상 모두 지원

----------------------------------------------------------------
2. 실행 방법
----------------------------------------------------------------

1) 입력 영상 파일 "lena.jpg"를 각 소스 파일과 동일한 디렉토리에 위치시킵니다.
2) 각 cpp 파일을 OpenCV 라이브러리와 함께 컴파일합니다.

   예시 (Linux/Mac):
     g++ Gaussian.cpp -o Gaussian `pkg-config --cflags --libs opencv4`
     g++ salt_and_pepper.cpp -o salt_and_pepper `pkg-config --cflags --libs opencv4`
     g++ Bilateral.cpp -o Bilateral `pkg-config --cflags --libs opencv4`

3) 컴파일 후 실행합니다.
     ./Gaussian
     ./salt_and_pepper
     ./Bilateral

4) 실행 시 아래 창이 순서대로 표시됩니다.
   - 원본 영상 (그레이스케일 / 컬러)
   - 노이즈가 추가된 영상
   - 필터링 후 복원된 영상
   - 아무 키나 누르면 종료됩니다.

----------------------------------------------------------------
3. 파라미터 설명 및 조정 방법
----------------------------------------------------------------

▶ Gaussian.cpp

  [노이즈 추가] Add_Gaussian_noise(input, mean, sigma)
    - mean  : 노이즈의 평균 (기본값: 0)
    - sigma : 노이즈의 표준편차 (기본값: 0.1) → 값이 클수록 노이즈 강도 증가

  [필터링] Gaussianfilter_Gray / Gaussianfilter_RGB(input, n, sigmaT, sigmaS, opt)
    - n      : 커널 반경, 커널 크기 = (2n+1)×(2n+1) (기본값: 5)
    - sigmaT : x 방향 표준편차 (기본값: 3)
    - sigmaS : y 방향 표준편차 (기본값: 3)
    - opt    : 경계 처리 방법 (기본값: "zero-padding")
               "zero-padding"  → 경계 밖을 0으로 처리
               "mirroring"     → 경계에서 반사 처리
               "adjustkernel"  → 유효 픽셀만으로 커널 재정규화

  main() 내 조정 위치:
    Mat Denoised_Gray = Gaussianfilter_Gray(noise_Gray, 5, 3, 3, "zero-padding");
    Mat Denoised_RGB  = Gaussianfilter_RGB (noise_RGB,  5, 3, 3, "zero-padding");

---

▶ salt_and_pepper.cpp

  [노이즈 추가] Add_salt_pepper_Noise(input, ps, pp)
    - ps : salt(흰색) 노이즈 밀도, 0~1 사이 (기본값: 0.1)
    - pp : pepper(검정) 노이즈 밀도, 0~1 사이 (기본값: 0.1)

  [필터링] Salt_pepper_noise_removal_Gray / _RGB(input, n, opt)
    - n   : 커널 반경, 커널 크기 = (2n+1)×(2n+1) (기본값: 2)
    - opt : 경계 처리 방법 (기본값: "zero-padding")
            "zero-padding"  → 경계 밖을 0으로 처리
            "mirroring"     → 경계에서 반사 처리
            "adjustkernel"  → 유효 픽셀만으로 중앙값 계산

  main() 내 조정 위치:
    int window_radius = 2;
    Mat Denoised_Gray = Salt_pepper_noise_removal_Gray(noise_Gray, window_radius, "zero-padding");
    Mat Denoised_RGB  = Salt_pepper_noise_removal_RGB (noise_RGB,  window_radius, "zero-padding");

---

▶ Bilateral.cpp

  [노이즈 추가] Add_Gaussian_noise(input, mean, sigma)
    - mean  : 노이즈의 평균 (기본값: 0)
    - sigma : 노이즈의 표준편차 (기본값: 0.1)

  [필터링] bilateralfilter_Gray / bilateralfilter_RGB(input, n, sigmaT, sigmaS, sigmaR, opt)
    - n      : 커널 반경, 커널 크기 = (2n+1)×(2n+1) (기본값: 5)
    - sigmaT : x 방향 공간 표준편차 (기본값: 3)
    - sigmaS : y 방향 공간 표준편차 (기본값: 3)
    - sigmaR : 화소값(intensity) 차이에 대한 표준편차 (기본값: 0.2)
               → 값이 작을수록 엣지 보존 효과 강해짐
    - opt    : 경계 처리 방법 (기본값: "zero-padding")
               "zero-padding"  → 경계 밖을 0으로 처리
               "mirroring"     → 경계에서 반사 처리
               "adjustkernel"  → 유효 픽셀만으로 가중치 재정규화

  main() 내 조정 위치:
    Mat Denoised_Gray_bilateral = bilateralfilter_Gray(noise_Gray, 5, 3, 3, 0.2, "zero-padding");
    Mat Denoised_RGB_bilateral  = bilateralfilter_RGB (noise_RGB,  5, 3, 3, 0.2, "zero-padding");

----------------------------------------------------------------
4. 기본 파라미터 요약
----------------------------------------------------------------

  파일                  | n | sigmaT | sigmaS | sigmaR | 경계처리
  ----------------------|---|--------|--------|--------|-------------
  Gaussian.cpp          | 5 |   3    |   3    |   -    | zero-padding
  salt_and_pepper.cpp   | 2 |   -    |   -    |   -    | zero-padding
  Bilateral.cpp         | 5 |   3    |   3    |  0.2   | zero-padding

----------------------------------------------------------------
5. 참고 사항
----------------------------------------------------------------

- 입력 영상 타입: CV_64FC3 (Gaussian, Bilateral), CV_8UC3 (salt_and_pepper)
- 입력 파일명을 변경하려면 main() 내 imread("lena.jpg", ...) 부분을 수정하세요.
- 경계 처리 방법을 변경하려면 opt 인자를 수정하세요.

================================================================
