# OSP Assignments

> **컴퓨터공학과 이다영**  
> 영상처리 및 딥러닝 기반 컴퓨터 비전 과목(O.S.P) 과제 포트폴리오

---

## 프로젝트 소개

본 저장소는 **OpenCV(C++) 기반 전통적 영상처리**부터 **NumPy/PyTorch 기반 딥러닝**까지, 컴퓨터 비전의 핵심 주제를 11개 과제로 단계적으로 구현한 결과물입니다.

모든 과제에는 **Technical Report(PDF)** 와 **실행 가이드(README)** 가 포함되어 있어, 설계 의도와 실험 결과를 함께 확인할 수 있습니다.

---

## 핵심 역량 요약

| 영역 | 구현 내용 |
|------|-----------|
| **기하학적 변환** | 역방향 매핑 기반 이미지 회전, Affine 변환, 파노라마 스티칭, Alpha Blending |
| **히스토그램 처리** | PDF/CDF 분석, Stretching / Equalization / Matching (Gray·RGB·YUV) |
| **공간 필터링** | Mean, Gaussian(분리 가능), Sobel, Laplacian, Unsharp Masking — 경계 처리 3종 직접 구현 |
| **노이즈 & 복원** | Gaussian / Salt & Pepper 노이즈 추가 및 Gaussian·Median·Bilateral 필터 복원 |
| **세그멘테이션** | Adaptive Thresholding, K-Means, Mean Shift |
| **특징점 & 매칭** | LoG, Canny, Harris Corner, SIFT, SURF, Homography, RANSAC |
| **기하학적 검출** | Hough 변환(표준/확률적), 선분 추출 |
| **딥러닝 (분류)** | 2-Layer FC Network (NumPy), VGG-16 / ResNet-50 (PyTorch) |
| **딥러닝 (분할)** | UNet, ResNet-50 Encoder UNet — Pascal VOC Semantic Segmentation |

**사용 기술:** C++ · OpenCV 4.x · Python · NumPy · PyTorch · Jupyter Notebook · Google Colab

---

## 저장소 구조

```
OSP_Assignments/
├── Task01/   기하학적 변환 (회전, 스티칭)
├── Task02/   히스토그램 처리
├── Task03/   공간 필터링
├── Task04/   노이즈 추가 및 복원
├── Task05/   세그멘테이션
├── Task06/   에지·코너 검출
├── Task07/   SIFT / SURF 특징점 매칭
├── Task08/   Hough 변환 & SIFT 기반 스티칭
├── Task09/   2-Layer Neural Network (CIFAR-10)
├── Task10/   CNN — VGG-16 & ResNet-50 (CIFAR-10)
└── Task11/   Semantic Segmentation — UNet 
```

각 `TaskXX/` 폴더에는 소스 코드, 입력 이미지, `README.txt`, `Technical Report (PDF)` 가 포함되어 있습니다.

---

## 상세 내용

### Task 01 — 기하학적 변환

| 파일 | 내용 |
|------|------|
| `rotate_skeleton_v2.cpp` | **역방향 매핑(Inverse Warping)** 기반 이미지 회전. Nearest / Bilinear 보간 지원, Clamping으로 Hole 현상 방지 |
| `stitching.cpp` | 대응점 기반 **Least Squares Affine 변환** 추정 → Inverse Warping → **Alpha Blending** 파노라마 생성 |

**핵심 구현:** 템플릿 기반 다형성(`Vec3b` / `uchar`), Row-major 인덱스 규약 준수

---

### Task 02 — 히스토그램 처리

| 파일 | 내용 |
|------|------|
| `PDF_CDF.cpp` | PDF·CDF 계산 및 시각화 |
| `hist_stretching.cpp` | 구간별 선형 변환 기반 **히스토그램 스트레칭** |
| `hist_eq.cpp` / `hist_eq_RGB.cpp` / `hist_eq_YUV.cpp` | **히스토그램 평활화** (Gray / RGB 채널별 / YUV Y채널) |
| `hist_matching_gray.cpp` / `hist_matching_color.cpp` | CDF 기반 **히스토그램 매칭** |

**핵심 구현:** `hist_func.h` 공통 모듈화, YUV 색 공간 활용으로 컬러 왜곡 방지, PDF/CDF 텍스트 출력으로 변환 함수 검증

---

### Task 03 — 공간 필터링

| 파일 | 내용 |
|------|------|
| `MeanFilter*.cpp` | 균일 평균 필터 (Gray / RGB) |
| `Gaussian*.cpp` / `Gaussian*_Sep.cpp` | 가우시안 필터 및 **분리 가능(Separable) 구현** |
| `Sobel*.cpp` / `Laplacian*.cpp` | 에지 검출 |
| `Unsharp*.cpp` | **Unsharp Masking** 기반 선명화 |

**핵심 구현:** Zero-padding / Mirroring / Adjust-kernel 3종 **경계 처리** 직접 구현, 이방성 가우시안(`sigmaT`, `sigmaS`) 지원

---

### Task 04 — 노이즈 추가 및 영상 복원

| 파일 | 노이즈 | 복원 필터 |
|------|--------|-----------|
| `Gaussian.cpp` | Gaussian | Gaussian Filter |
| `salt_and_pepper.cpp` | Salt & Pepper (Impulse) | Median Filter |
| `Bilateral.cpp` | Gaussian | **Bilateral Filter** (엣지 보존) |

**핵심 구현:** 노이즈 유형별 최적 필터 선택 및 파라미터(`sigmaR` 등) 실험을 통한 복원 품질 비교

---

### Task 05 — 세그멘테이션

| 파일 | 내용 |
|------|------|
| `adaptivethreshold.cpp` | Moving Average 기반 **Adaptive Thresholding** — 불균일 조명 문서 이진화 |
| `kmeans.cpp` | **K-Means** 클러스터링 (밝기 / RGB / 위치 5D 등 4가지 방식) |
| `meanshift.cpp` | **Mean Shift** 세그멘테이션 (`pyrMeanShiftFiltering`) |

---

### Task 06 — 에지·코너 검출

| 파일 | 내용 |
|------|------|
| `LoG.cpp` | Gaussian Smoothing + Laplacian → **LoG 에지 검출** (직접 구현) |
| `Canny.cpp` | OpenCV Canny — Non-maximum Suppression, Double Thresholding |
| `Harris_corner.cpp` | **Harris Corner** 검출, NMS 및 Subpixel Refinement |

---

### Task 07 — SIFT / SURF 특징점 매칭

| 파일 | 내용 |
|------|------|
| `SIFT.cpp` | SIFT 키포인트 추출 → NN 매칭. **Cross-checking / Ratio-based Thresholding** 3가지 케이스 비교 실험 |
| `SURF.cpp` | SURF + FLANN 매처 → **RANSAC Homography** → 객체 위치 검출 및 시각화 |

---

### Task 08 — Hough 변환 & SIFT 스티칭

| 파일 | 내용 |
|------|------|
| `Hough.cpp` | Canny → **HoughLines / HoughLinesP** 직선 검출, `getSegment()` 선분 추출 |
| `stitching.cpp` | SIFT 매칭 → Affine 변환. **Case1 (전체 대응점 LS)** vs **Case2 (RANSAC Outlier 제거)** 비교 |

---

### Task 09 — 2-Layer Neural Network (CIFAR-10)

| 파일 | 내용 |
|------|------|
| `classifiers/neural_net.py` | **TwoLayerNet** — Forward/Backward Pass, Softmax CE Loss + L2 Regularization |
| `two_layer_net.ipynb` | 학습·검증·Grid Search 실험 노트북 |

**실험 결과**

| 지표 | 값 |
|------|----|
| Validation Accuracy | **47.0%** |
| Test Accuracy | **47.5%** (목표 36% 초과) |

**Grid Search:** hidden_size × learning_rate × regularization = 8가지 조합 탐색  
**추가 작업:** `scipy.misc.imread` → `imageio.imread` 호환성 수정

---

### Task 10 — CNN: VGG-16 & ResNet-50 (CIFAR-10)

| 파일 | 내용 |
|------|------|
| `vgg16_full.py` | VGG-16 전체 구현 |
| `resnet50_full.py` | ResNet-50 (ResidualBlock) 전체 구현 |
| `main.py` | 데이터 증강, 학습, 평가 파이프라인 |

**실험 결과**

| 모델 | Test Accuracy |
|------|---------------|
| VGG-16 | **86.70%** |
| ResNet-50 | **81.37%** |

---

### Task 11 — Semantic Segmentation (Pascal VOC 2012)

| 파일 | 내용 |
|------|------|
| `UNet.py` | 기본 **UNet** 구현 |
| `resnet_encoder_unet.py` | **ResNet-50 Encoder UNet** 구현 |
| `modules.py` | Train / Validation 루프 |
| `datasets.py` | Pascal VOC 2012 DataLoader |
| `main_unet.py` / `main_resnet.py` | 모델별 독립 실행 진입점 |

**학습 설정:** Adam, CrossEntropyLoss, StepLR, 256×256 Resize, Batch Size 16

---

## 실행 환경

### C++ 과제 (Task 01–08)

```
- C++17
- OpenCV 4.x
- Visual Studio 2022 (Windows) 또는 g++ + pkg-config (macOS/Linux)
```

**macOS/Linux 컴파일 예시:**
```bash
g++ -std=c++17 SIFT.cpp -o SIFT `pkg-config --cflags --libs opencv4`
./SIFT
```

> 각 Task 폴더의 `README.txt`에 파일별 입력 이미지, 파라미터, 빌드 방법이 상세히 기술되어 있습니다.

### Python 과제 (Task 09–11)

```
- Python 3.x
- NumPy, SciPy, Matplotlib (Task 09)
- PyTorch, Torchvision (Task 10–11)
- Jupyter Notebook / Google Colab (Task 09, 11)
```

**Task 09 실행:**
```bash
cd Task09
jupyter notebook two_layer_net.ipynb
```

**Task 10 실행:**
```bash
cd Task10
python main.py
```

**Task 11 실행:**
```bash
cd Task11
python main_unet.py        # UNet
python main_resnet.py      # ResNet-Encoder UNet
```

---

## Technical Report

각 과제의 알고리즘 분석, 파라미터 실험, 결과 비교는 해당 Task 폴더 내 PDF 파일에 정리되어 있습니다.

| Task | Report 파일 |
|------|-------------|
| 01 | `Task01/Assignment01_technical_report.pdf` |
| 02 | `Task02/Technical_Report_Assignment02.pdf` |
| 03 | `Task03/Technical Report-Assignment03(이다영).pdf` |
| 04 | `Task04/Technical_Report_Assignment04_이다영.pdf` |
| 05 | `Task05/05.Technical_Report.pdf` |
| 06 | `Task06/ldy_Technical_Report_Assignment06.pdf` |
| 07 | `Task07/Technical_Report-Assignment07.pdf` |
| 08 | `Task08/technical_Report_08.pdf` |
| 09 | `Task09/TechnicalReport_Assignment_09_이다영.pdf` |
| 10 | `Task10/OSP_Assignment_10_Technical_Report.pdf` |
| 11 | `Task11/TechnicalReport-Assginment11.pdf` |

---

## 학습 흐름

```
[전통적 영상처리]                    [딥러닝]
Task 01 ─ 기하학적 변환
Task 02 ─ 히스토그램
Task 03 ─ 공간 필터링
Task 04 ─ 노이즈·복원
Task 05 ─ 세그멘테이션
Task 06 ─ 에지·코너
Task 07 ─ SIFT/SURF
Task 08 ─ Hough·스티칭
         │
         ▼
Task 09 ─ FC Network (NumPy, 역전파 직접 구현)
Task 10 ─ CNN 분류 (VGG-16, ResNet-50)
Task 11 ─ Semantic Segmentation (UNet)
```

---

