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
    



};
float calculateBrightness(const cv::Mat& image);


std::tuple<std::vector<float>, std::vector<float>, std::vector<float>> computeColorHistogram(const cv::Mat& image);


Metadata compute_metatada(ofxCvColorImage& cvImage);
template <typename MediaType>
void metadataGenerator::updateMetadata(DataPoint<MediaType>& dataPoint) {

    ofxCvColorImage cvImage = dataPoint.getOFCVImage();


    dataPoint.metadata = compute_metatada(cvImage);
    // save changes
    dataPoint.metadata.save(dataPoint.metadataPath);
}
