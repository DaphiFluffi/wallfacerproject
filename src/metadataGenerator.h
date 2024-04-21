#pragma once

#include "dataContainers.h"
#include <opencv2/opencv.hpp>  
#include <opencv2/imgproc/imgproc.hpp>

class metadataGenerator {
public:
    
    metadataGenerator() = default;

    
    template <typename MediaType>
    void updateMetadata(DataPoint<MediaType>& dataPoint);

private:
    
    float calculateBrightness(const cv::Mat& image);
    
};


template <typename MediaType>
void metadataGenerator::updateMetadata(DataPoint<MediaType>& dataPoint) {

    ofxCvColorImage cvImage = dataPoint.getOFCVImage();

    IplImage* iplImg = cvImage.getCvImage();  // Gets the IplImage*

    // Convert IplImage* to cv::Mat
    cv::Mat colorMat = cv::cvarrToMat(iplImg);  // Use cvarrToMat to convert
    cv::Mat grayMat;
    cv::cvtColor(colorMat, grayMat, cv::COLOR_RGB2GRAY);

    dataPoint.metadata.brightness = calculateBrightness(grayMat);
    std::cout << "computed brightness " << dataPoint.metadata.brightness.value() << std::endl;
    
    // save changes
    dataPoint.metadata.save(dataPoint.metadataPath);
}
