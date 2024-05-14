#include "metadataGenerator.h"




float metadataGenerator::calculateBrightness(const cv::Mat& crayMat) {

    cv::Scalar meanBrightness = cv::mean(crayMat);
    return static_cast<float>(meanBrightness[0]);  
}



std::tuple<std::vector<float>, std::vector<float>, std::vector<float>> metadataGenerator::computeColorHistogram(const cv::Mat& image) {
    // Histogram parameters
    int histSize = 256; // Number of bins
    float range[] = { 0, 256 };
    const float* histRange = { range };

    cv::Mat redHist, greenHist, blueHist;
    std::vector<cv::Mat> bgr_planes;
    cv::split(image, bgr_planes);

    // Calculate histograms for each channel
    cv::calcHist(&bgr_planes[0], 1, 0, cv::Mat(), blueHist, 1, &histSize, &histRange, true, false);
    cv::calcHist(&bgr_planes[1], 1, 0, cv::Mat(), greenHist, 1, &histSize, &histRange, true, false);
    cv::calcHist(&bgr_planes[2], 1, 0, cv::Mat(), redHist, 1, &histSize, &histRange, true, false);

    cv::normalize(blueHist, blueHist, 1.0, 0.0, cv::NORM_L1);
    cv::normalize(greenHist, greenHist, 1.0, 0.0, cv::NORM_L1);
    cv::normalize(redHist, redHist, 1.0, 0.0, cv::NORM_L1);

    // Convert histograms to vectors
    std::vector<float> blueHistVec(blueHist.begin<float>(), blueHist.end<float>());
    std::vector<float> greenHistVec(greenHist.begin<float>(), greenHist.end<float>());
    std::vector<float> redHistVec(redHist.begin<float>(), redHist.end<float>());

    return std::make_tuple(redHistVec, greenHistVec, blueHistVec);
}