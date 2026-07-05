====================================================
 Assignment 09 
 2317022 이다영
====================================================

■ 코드 목적
----------------------------------------------------
CIFAR-10 데이터셋을 이용하여 2층 완전연결 신경망(Two-Layer Neural Network)을
구현하고 학습시키기.

네트워크 구조: input → FC layer(W1, b1) → ReLU → FC layer(W2, b2) → Softmax
손실 함수: Softmax Cross-Entropy Loss + L2 Regularization


■ 파일 구성
----------------------------------------------------
Assignment09_이다영/
├── two_layer_net.ipynb       # 메인 실행 노트북
├── data_utils.py             # CIFAR-10 데이터 로딩 유틸 (수정됨, 아래 참고)
├── gradient_check.py         # 수치 미분을 이용한 그래디언트 검증 유틸
├── vis_utils.py              # 가중치 시각화 유틸
├── Readme.txt                # 본 파일
├── TechnicalReport_Assignment_09_이다영.pdf     # Technical Report
├── datasets/
│   └── cifar-10-batches-py/  # CIFAR-10 데이터셋 (자동 다운로드)
└── classifiers/
    ├── __init__.py
    └── neural_net.py         # TwoLayerNet 클래스 구현 파일


■ 실행 방법 (Google Colab 기준)
----------------------------------------------------
1. 구글 드라이브에 위 파일 구조대로 업로드

2. two_layer_net.ipynb를 Google Colaboratory로 열기
   (파일 우클릭 → 연결 앱 → Google Colaboratory)

3. 첫 번째 셀(Cell 1) 실행
   - 구글 드라이브 마운트
   - CIFAR-10 데이터셋 자동 다운로드 및 압축 해제
   ※ 이 셀을 가장 먼저 실행해야 합니다.

4. 이후 셀을 순서대로 모두 실행


■ 구현 내용 (classifiers/neural_net.py)
----------------------------------------------------
1. loss() - Forward pass & Backward pass
   - Forward: s1 = X·W1 + b1 → ReLU → scores = p1·W2 + b2
   - Loss: Softmax Cross-Entropy Loss + L2 Regularization
   - Backward: 역전파를 통한 W1, b1, W2, b2 그래디언트 계산

2. train() - SGD 기반 학습
   - 미니배치 랜덤 샘플링 (np.random.choice)
   - 파라미터 업데이트: W_new = W - lr * dL/dW
   - 매 epoch마다 learning rate decay 적용

3. predict() - 예측
   - Forward pass 수행 후 최대 score의 클래스 인덱스 반환


■ 실험 파라미터
----------------------------------------------------
[ 데이터 ]
- 학습 데이터: 19,000개
- 검증 데이터: 1,000개
- 테스트 데이터: 1,000개

[ 기본 설정 ]
- input_size  : 32 × 32 × 3 = 3072
- num_classes : 10
- batch_size  : 200
- num_iters   : 1000
- learning_rate_decay : 0.95

[ 하이퍼파라미터 탐색 범위 (Grid Search) ]
- hidden_size    : [100, 150]
- learning_rate  : [1e-3, 3e-3]
- regularization : [0.1, 0.5]
- 총 8가지 조합 탐색

[ 최적 하이퍼파라미터 ]
- hidden_size   : 100
- learning_rate : 1e-3
- reg           : 0.1


■ 실험 결과
----------------------------------------------------
- Validation accuracy : 0.470
- Train accuracy      : 0.538
- Test accuracy       : 0.475  (목표 36% 초과 달성)


■ 수정 사항
----------------------------------------------------
* data_utils.py의 scipy.misc.imread를 최신 환경 호환을 위해
  imageio.imread로 수정하였습니다. (scipy 버전 호환성 문제)
  - 원본 코드는 scipy==1.2.1 기준으로 작성되었으나,
    최신 scipy에서 imread가 제거되어 imageio로 대체하였습니다.
  - 원본 코드는 삭제하지 않고 주석처리 하였습니다. 따라서 필요한 경우 주석을 풀고 사용하면 됩니다. 

====================================================
