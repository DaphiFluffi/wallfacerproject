#pragma once

#include "dataContainers.h"
#include "CV_Stuff.h"
#include <vector>
#include <tuple>


class metadataGenerator {
        metadataGenerator() {
        }
public:

    TextureHistogramComputer textureHistogramComputer;
    EdgeHistogramComputer edgeHistogramComputer;
    FaceFinder& faceFinder = FaceFinder::getInstance();
    SurfComputer surf_computer;
    metadataGenerator(const metadataGenerator&) = delete;
    metadataGenerator& operator=(const metadataGenerator&) = delete;


    static metadataGenerator& getInstance() {
        static metadataGenerator instance;
        return instance;
    }


    
    template <typename MediaType>
    void updateMetadata(DataPoint<MediaType>& dataPoint);

    Metadata compute_metatada(ofxCvColorImage& cvImage, std::string original_path);


};

float calculateBrightness(const cv::Mat& image);


std::tuple<std::vector<float>, std::vector<float>, std::vector<float>> computeColorHistogram(const cv::Mat& image);


template <typename MediaType>
void metadataGenerator::updateMetadata(DataPoint<MediaType>& dataPoint) {

    ofxCvColorImage cvImage = dataPoint.getOFCVImage();


    dataPoint.metadata = compute_metatada(cvImage, dataPoint.metadataPath);
    // save changes
    dataPoint.metadata.save(dataPoint.metadataPath);
}
