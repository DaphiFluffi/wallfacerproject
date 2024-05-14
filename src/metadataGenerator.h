#pragma once

#include "dataContainers.h"
#include "CV_Stuff.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include <tuple>

class metadataGenerator {
public:
    
    metadataGenerator() = default;

    
    template <typename MediaType>
    void updateMetadata(DataPoint<MediaType>& dataPoint);

private:
    
    float calculateBrightness(const cv::Mat& image);
    
    std::tuple<std::vector<float>, std::vector<float>, std::vector<float>> computeColorHistogram(const cv::Mat& image);

};


template <typename MediaType>
void metadataGenerator::updateMetadata(DataPoint<MediaType>& dataPoint) {

    ofxCvColorImage cvImage = dataPoint.getOFCVImage();


    IplImage* iplImg = cvImage.getCvImage();

    // Convert IplImage* to cv::Mat
    cv::Mat colorMat = cv::cvarrToMat(iplImg);  // Use cvarrToMat to convert
    cv::Mat grayMat;
    cv::cvtColor(colorMat, grayMat, cv::COLOR_RGB2GRAY);

    dataPoint.metadata.brightness = calculateBrightness(grayMat);
    std::cout << "computed brightness " << dataPoint.metadata.brightness.value() << std::endl;
    

    auto [redHist, greenHist, blueHist] = computeColorHistogram(colorMat);

    dataPoint.metadata.redHist = redHist;
    dataPoint.metadata.blueHist = blueHist;
    dataPoint.metadata.greenHist = greenHist;

    dataPoint.metadata.n_faces = static_cast<int>(FaceFinder::getInstance().detect_faces(cvImage.getPixels()).size());
    std::cout << "faces: " << dataPoint.metadata.n_faces.value() << endl;

    // save changes
    dataPoint.metadata.save(dataPoint.metadataPath);
}
