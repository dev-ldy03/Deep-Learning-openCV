================================================================================
  README - Assignment 03: Image Filtering with OpenCV (C++)
	2317022 이다영 컴퓨터공학과
================================================================================
참고) 코드의 분량이 커서 technical report 에는 결과 위주의 설명을 담았고 핵심 코드에 대한 해석 및 분석은 
주석으로 처리하였다.
--------------------------------------------------------------------------------
  PURPOSE
--------------------------------------------------------------------------------

This project implements various spatial filtering techniques using OpenCV in C++.
The goal is to understand the mathematical principles of smoothing (Mean/Gaussian),
edge detection (Sobel/Laplacian), and sharpening (Unsharp Masking) by 
implementing them from scratch, including boundary processing logic.

  File List & Purpose:
  -----------------------------------------------------------------------
  MeanFilterGray.cpp    - Uniform mean filter on grayscale image
  MeanFilterRGB.cpp     - Uniform mean filter on color (RGB) image
  GaussianGray.cpp      - Gaussian filter on grayscale image
  GaussianRGB.cpp       - Gaussian filter on color (RGB) image
  GaussianGray_Sep.cpp  - Separable Gaussian filter on grayscale image 
                          (Applied as 1D kernels sequentially for speed)
  GaussianRGB_sep.cpp   - Separable Gaussian filter on color (RGB) image
  SobelGray.cpp         - Sobel edge detection on grayscale image
  SobelRGB.cpp          - Sobel edge detection on color (RGB) image
  LaplacianGray.cpp     - Laplacian edge detection on grayscale image
  LaplacianRGB.cpp      - Laplacian edge detection on color (RGB) image
  UnsharpGray.cpp    	- Image sharpening using Gaussian low-pass filter
  UnsharpRGB.cpp        - Image sharpening with color image


--------------------------------------------------------------------------------
  HOW TO RUN THIS CODE
--------------------------------------------------------------------------------

Step 1. Open Visual Studio 2022 and create a new C++ Console App project.
Step 2. Configure OpenCV (v4.x) environment:
        - Add 'include' directory to Additional Include Directories.
        - Add 'lib' directory to Additional Library Directories.
        - Add 'opencv_world4XX.lib' to Additional Dependencies.
Step 3. Place 'lena.jpg' in the same directory as the .vcxproj file.
Step 4. Add the desired .cpp file to the project. 
        Note: Since each file contains a main() function, ensure only ONE .cpp 
        file is included in the build (Right-click others -> Properties -> 
        General -> Excluded from Build: Yes).
Step 5. Build and run (Ctrl+F5).


--------------------------------------------------------------------------------
  HOW TO ADJUST PARAMETERS
--------------------------------------------------------------------------------

You can modify the parameters in the main() function of each file:

1. Kernel Size (n):
   - Defines the filter window as (2n+1) x (2n+1).
   - Example: n=1 results in a 3x3 kernel; n=3 results in a 7x7 kernel.

2. Standard Deviation (sigmaT, sigmaS):
   - Used in Gaussian filters to control the distribution spread.
   - sigmaT for x-coordinate and sigmaS for y-coordinate (Anisotropic support).

3. Boundary Processing (opt):
   - "zero-padding" : Pads the boundary with zeros (black pixels).
   - "mirroring"    : Mirrors the boundary pixels (Prevents edge artifacts).
   - "adjustkernel" : Re-normalizes the kernel based on available pixels.

4. Sharpening Constant (k):
   - Used in Unsharp Masking to control the sharpening intensity.


--------------------------------------------------------------------------------
  HOW TO DEFINE DEFAULT PARAMETERS
--------------------------------------------------------------------------------

The default parameters in this project are defined based on the following logic:

1. Choice of Kernel Size (n=1 or n=3):
   - For Edge Detection (Sobel/Laplacian), n=1 (3x3) is default to capture 
     high-frequency local variations without excessive blurring.
   - For Smoothing (Mean/Gaussian), n=3 (7x7) is default to provide a 
     sufficiently visible blurring effect on a 512x512 image.

2. Choice of Sigma (sigma=1.0):
   - A standard deviation of 1.0 is effective for 3x3 or 7x7 kernels, as 
     approximately 99.7% of the Gaussian weight falls within 3*sigma.

3. Choice of Boundary Option ("mirroring"):
   - Set as default for Sobel/Laplacian tasks because mirroring prevents 
     the creation of artificial edges at the image borders, which would 
     otherwise be detected as false edges.

4. Choice of k (k=0.5):
   - k=0.5 provides a balanced unsharp masking result that enhances edges 
     without causing over-saturation or extreme noise amplification.


--------------------------------------------------------------------------------
  DEFAULT PARAMETERS TABLE
--------------------------------------------------------------------------------

  +---------------------+-------+--------+--------+------+-----------------+
  | File                |   n   | sigmaT | sigmaS |  k   |       opt       |
  +---------------------+-------+--------+--------+------+-----------------+
  | MeanFilterGray.cpp  |   3   |   -    |   -    |  -   | "mirroring"     |
  | MeanFilterRGB.cpp   |   3   |   -    |   -    |  -   | "zero-paddle"   |
  | GaussianGray.cpp    |   1   |   1.0  |   1.0  |  -   | "mirroring"     |
  | GaussianRGB.cpp     |   1   |   1.0  |   1.0  |  -   | "zero-paddle"   |
  | GaussianGray_Sep.cpp|   1   |   1.0  |   1.0  |  -   | "mirroring"     |
  | GaussianRGB_sep.cpp |   1   |   1.0  |   1.0  |  -   | "zero-paddle"   |
  | SobelGray.cpp       |   1   |   -    |   -    |  -   | "mirroring"* |
  | LaplacianGray.cpp   |   1   |   -    |   -    |  -   | "mirroring"* |
  | UnsharpGray.cpp     |   3   |   3.0  |   3.0  | 0.5  | "mirroring"     |
  | UnsharpRGB.cpp      |   3   |   3.0  |   3.0  | 0.8  | "mirroring"     |
  +---------------------+-------+--------+--------+------+-----------------+