//dataContainers.h
#pragma once

#include "ofMain.h"
#include <string>
#include <vector>
#include <optional>
#include "ofxXmlSettings.h"
#include "ofxCvColorImage.h"

struct Metadata {
    std::optional<std::string> description = std::nullopt;
    std::optional<float> brightness = std::nullopt;
    std::optional<vector<float>> redHist = std::nullopt;
    std::optional<vector<float>> greenHist = std::nullopt;
    std::optional<vector<float>> blueHist = std::nullopt;
    std::optional<int> n_faces = std::nullopt;

    ofxXmlSettings settings;


    std::optional<int> getOptionalIntValue(const std::string& tag, int which = 0);
    std::optional<double> getOptionalDoubleValue(const std::string& tag, int which = 0);
    std::optional<std::string> getOptionalStringValue(const std::string& tag, int which = 0);
    std::optional<vector<float>> getOptionalDoubleVecValue(const std::string &tag, int size);

    Metadata() = default;
    Metadata(const std::string& filepath);

    void save(const std::string& filepath);

    void draw(float x, float y, float w, float h);

    
};


template<typename MediaType>
struct DataPoint {
    std::string filePath;
    std::string metadataPath;

    Metadata metadata;

    MediaType loadMedia() {
        MediaType media;
        media.load(filePath); 
        return media;
    }


    ofxCvColorImage getOFCVImage();
};




template<>
ofxCvColorImage DataPoint<ofImage>::getOFCVImage();

template<>
ofxCvColorImage DataPoint<ofVideoPlayer>::getOFCVImage();

