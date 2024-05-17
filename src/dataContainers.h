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
    private:
        bool loaded = false;
    public:

    std::string filePath;
    std::string metadataPath;
    MediaType data;  // Direct storage of MediaType object

    Metadata metadata;

    DataPoint() = default;


    // Constructor to initialize with MediaType and metadata
    DataPoint(MediaType media, Metadata metadata, std::string time)
        : metadata(metadata), data(std::move(media)) {
        metadataPath = "camera_image_"+ time;
        filePath = "camera_image_"+ time;
        loaded = true;
    }

    MediaType loadMedia() {
        MediaType media;
        media.load(filePath);
        return media;
    }

    MediaType* getData() {
        if (!loaded) {
            data = loadMedia();
            loaded = true;
        }
        return &data;
    }

    ofxCvColorImage getOFCVImage();
};


template<>
ofxCvColorImage DataPoint<ofImage>::getOFCVImage();

template<>
ofxCvColorImage DataPoint<ofVideoPlayer>::getOFCVImage();

