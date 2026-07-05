Assignment 11 
2317022 이다영 (컴퓨터공학과)

=== Purpose ===
Pascal VOC 2012 데이터셋을 이용한 이미지 semantic segmentation 수행.
기본 UNet과 ResNet-50을 encoder로 사용하는 UNet을 구현하고 학습한다.

=== Files ===
- UNet.py                  : UNet 모델 구현
- resnet_encoder_unet.py   : ResNet-50 encoder 기반 UNet 모델 구현
- modules.py               : train / validation 모듈 (train_model, val_model 등)
- datasets.py              : Pascal VOC 2012 데이터 로더
- main_unet.py             : UNet 학습 실행 메인 코드
- main_resnet.py           : ResNet-encoder-UNet 학습 실행 메인 코드

* main 파일을 두 개로 분리한 이유:
  과제에서 UNet과 ResNet-encoder-UNet 두 모델을 각각 학습하고 평가하도록 요구하므로,
  모델별로 main 파일을 분리하여 각각 독립적으로 실행할 수 있도록 구성하였다.

=== How to Run ===
1. Pascal VOC 2012 데이터셋 다운로드
   http://host.robots.ox.ac.uk/pascal/VOC/voc2012/VOCtrainval_11-May-2012.tar

2. 아래 디렉토리 구조로 데이터셋 배치
   ../VOCdevkit/VOC2012/
   ├── JPEGImages/
   ├── SegmentationClass/
   └── ImageSets/Segmentation/trainval.txt

3. pretrained checkpoint 파일을 main 파일과 같은 디렉토리에 배치
   - resnet_encoder_unet.pth  (ResNet-encoder-UNet용)
   - UNet_trained_model.pth   (UNet용)

4. 실행
   - UNet:               python main_unet.py
   - ResNet-encoder-UNet: python main_resnet.py

=== Parameters ===
- batch_size    : 16
- learning_rate : 0.001
- resize_size   : 256 x 256
- epochs        : 40 (pretrained checkpoint 로드 시 자동으로 1로 설정됨)
- optimizer     : Adam
- loss          : CrossEntropyLoss
- scheduler     : StepLR (step_size=4, gamma=0.1)
- model         : UNet (main_unet.py) / UNetWithResnet50Encoder (main_resnet.py)

=== Output ===
- history/result{date}/result.png         : loss 및 accuracy 그래프
- history/result{date}/predicted/epoch{N} : segmentation 결과 이미지
- output/model{date}/{epoch}.pth          : 학습된 모델 checkpoint

=== Environment ===
- Python 3.x
- PyTorch
- Google Colab (GPU: T4)
- Pascal VOC 2012