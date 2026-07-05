#include <iostream>
#include <opencv2/opencv.hpp>

#define IM_TYPE CV_8UC3
#define L 256           // Number of intensity levels (0~255)

using namespace cv;

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

int main() {

    Mat input = imread("lena.jpg", IMREAD_COLOR);

    // Check if image loaded successfully before any processing
    if (!input.data) {
        std::cout << "Could not open" << std::endl;
        return -1;
    }

    Mat input_gray, output;
    cvtColor(input, input_gray, COLOR_RGB2GRAY); // Convert to grayscale

    namedWindow("Original", WINDOW_AUTOSIZE);
    imshow("Original", input);
    namedWindow("Original Gray", WINDOW_AUTOSIZE);
    imshow("Original Gray", input_gray);

    // Sample matrix: each row = one pixel's feature vector
    Mat samples(input.rows * input.cols, 3, CV_32F);
    int clusterCount = 10;  // Number of clusters K
    Mat labels;             // Output: cluster label for each pixel
    int attempts = 5;       // Number of times kmeans is run with different initializations
    Mat centers;            // Output: cluster center coordinates

    int rows = input.rows;
    int cols = input.cols;

    // -------------------------------------------------------------------------
    // Case 1: Grayscale intensity only (1D feature: intensity)
    // Input feature: I (normalized to [0,1])
    // -------------------------------------------------------------------------
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            // Normalize intensity to [0, 1]
            samples.at<float>(y + x * rows, 0) = input_gray.at<G>(y, x) / (float)(L - 1);
            samples.at<float>(y + x * rows, 1) = 0;
            samples.at<float>(y + x * rows, 2) = 0;
        }
    }
    kmeans(samples, clusterCount, labels,
           TermCriteria(TermCriteria::COUNT | TermCriteria::EPS, 10000, 0.0001),
           attempts, KMEANS_PP_CENTERS, centers);

    // Reconstruct output image: map each pixel to its cluster center intensity
    Mat gray_intensity(input.size(), input_gray.type());
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            // Denormalize: multiply by 255 to restore intensity range
            gray_intensity.at<G>(y, x) = (G)(centers.at<float>(labels.at<int>(y + x * rows, 0), 0) * (L - 1));
        }
    }
    namedWindow("Grayscale Intensity", WINDOW_AUTOSIZE);
    imshow("Grayscale Intensity", gray_intensity);

    // -------------------------------------------------------------------------
    // Case 2: Grayscale intensity + position (3D feature: I, x, y)
    // All features normalized to [0,1] by dividing by image dimensions
    // This ensures intensity and position have equal influence on clustering
    // -------------------------------------------------------------------------
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            int idx = y + x * rows;
            samples.at<float>(idx, 0) = input_gray.at<G>(y, x) / (float)(L - 1); // Normalized intensity [0,1]
            samples.at<float>(idx, 1) = float(x) / (cols - 1);  // Normalized x position [0,1]
            samples.at<float>(idx, 2) = float(y) / (rows - 1);  // Normalized y position [0,1]
        }
    }
    labels = Mat();
    centers = Mat();
    kmeans(samples, clusterCount, labels,
           TermCriteria(TermCriteria::COUNT | TermCriteria::EPS, 10000, 0.0001),
           attempts, KMEANS_PP_CENTERS, centers);

    // Reconstruct output: denormalize cluster center intensity
    Mat gray_intensity_position(input_gray.size(), input_gray.type());
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            gray_intensity_position.at<G>(y, x) = (G)((L - 1) * centers.at<float>(labels.at<int>(y + x * rows, 0), 0));
        }
    }
    namedWindow("Grayscale Intensity & Position", WINDOW_AUTOSIZE);
    imshow("Grayscale Intensity & Position", gray_intensity_position);

    // -------------------------------------------------------------------------
    // Case 3: RGB color only (3D feature: R, G, B)
    // Note: intensity not normalized here since position is not used simultaneously
    // -------------------------------------------------------------------------
    for (int y = 0; y < input.rows; y++)
        for (int x = 0; x < input.cols; x++)
            for (int z = 0; z < 3; z++)
                samples.at<float>(y + x * input.rows, z) = input.at<Vec3b>(y, x)[z]; // Raw RGB values

    labels = Mat();
    centers = Mat();
    kmeans(samples, clusterCount, labels,
           TermCriteria(TermCriteria::COUNT | TermCriteria::EPS, 10000, 0.0001),
           attempts, KMEANS_PP_CENTERS, centers);

    // Reconstruct output: assign cluster center color to each pixel
    Mat new_image(input.size(), input.type());
    for (int y = 0; y < input.rows; y++) {
        for (int x = 0; x < input.cols; x++) {
            int cluster_idx = labels.at<int>(y + x * input.rows, 0);
            new_image.at<C>(y, x)[0] = (G)(centers.at<float>(cluster_idx, 0)); // B channel
            new_image.at<C>(y, x)[1] = (G)(centers.at<float>(cluster_idx, 1)); // G channel
            new_image.at<C>(y, x)[2] = (G)(centers.at<float>(cluster_idx, 2)); // R channel
        }
    }
    namedWindow("clustered image", WINDOW_AUTOSIZE);
    imshow("clustered image", new_image);

    // -------------------------------------------------------------------------
    // Case 4: RGB color + position (5D feature: R, G, B, x, y)
    // All features normalized to [0,1]:
    //   color: divided by 255, position: divided by image dimensions
    // -------------------------------------------------------------------------
    samples = Mat(rows * cols, 5, CV_32F); // Resize sample matrix to 5 dimensions
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            int idx = y + x * rows;
            samples.at<float>(idx, 0) = input.at<C>(y, x)[0] / (float)(L - 1); // Normalized B [0,1]
            samples.at<float>(idx, 1) = input.at<C>(y, x)[1] / (float)(L - 1); // Normalized G [0,1]
            samples.at<float>(idx, 2) = input.at<C>(y, x)[2] / (float)(L - 1); // Normalized R [0,1]
            samples.at<float>(idx, 3) = float(x) / (cols - 1);  // Normalized x position [0,1]
            samples.at<float>(idx, 4) = float(y) / (rows - 1);  // Normalized y position [0,1]
        }
    }
    labels = Mat();
    centers = Mat();
    kmeans(samples, clusterCount, labels,
           TermCriteria(TermCriteria::COUNT | TermCriteria::EPS, 10000, 0.0001),
           attempts, KMEANS_PP_CENTERS, centers);

    // Reconstruct output: denormalize cluster center color values
    Mat RGB_intensity_position_5D(input.size(), input.type());
    for (int y = 0; y < input.rows; y++) {
        for (int x = 0; x < input.cols; x++) {
            int cluster_idx = labels.at<int>(y + x * input.rows, 0);
            RGB_intensity_position_5D.at<C>(y, x)[0] = (G)((L - 1) * centers.at<float>(cluster_idx, 0)); // B
            RGB_intensity_position_5D.at<C>(y, x)[1] = (G)((L - 1) * centers.at<float>(cluster_idx, 1)); // G
            RGB_intensity_position_5D.at<C>(y, x)[2] = (G)((L - 1) * centers.at<float>(cluster_idx, 2)); // R
        }
    }
    namedWindow("RGB Intensity & Position 5D", WINDOW_AUTOSIZE);
    imshow("RGB Intensity & Position 5D", RGB_intensity_position_5D);

    waitKey(0);
    return 0;
}