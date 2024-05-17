#include "metadataGenerator.h"




float calculateBrightness(const cv::Mat& crayMat) {

    cv::Scalar meanBrightness = cv::mean(crayMat);
    return static_cast<float>(meanBrightness[0]);  
}

Metadata compute_metatada(ofxCvColorImage& cvImage) {

    Metadata metadata;

    cvImage.resize(256, 256);
    IplImage* iplImg = cvImage.getCvImage();

    // Convert IplImage* to cv::Mat
    cv::Mat colorMat = cv::cvarrToMat(iplImg);  // Use cvarrToMat to convert
    cv::Mat grayMat;
    cv::cvtColor(colorMat, grayMat, cv::COLOR_RGB2GRAY);

    metadata.brightness = calculateBrightness(grayMat);
    std::cout << "computed brightness " << metadata.brightness.value() << std::endl;


    auto [redHist, greenHist, blueHist] = computeColorHistogram(colorMat);

    metadata.redHist = redHist;
    metadata.blueHist = blueHist;
    metadata.greenHist = greenHist;

    metadata.n_faces = static_cast<int>(FaceFinder::getInstance().detect_faces(cvImage.getPixels()).size());
    std::cout << "faces: " << metadata.n_faces.value() << endl;

    return metadata;
};



std::tuple<std::vector<float>, std::vector<float>, std::vector<float>> computeColorHistogram(const cv::Mat& image) {
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