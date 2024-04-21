#include "metadataGenerator.h"




float metadataGenerator::calculateBrightness(const cv::Mat& crayMat) {

    cv::Scalar meanBrightness = cv::mean(crayMat);
    return static_cast<float>(meanBrightness[0]);  
}


