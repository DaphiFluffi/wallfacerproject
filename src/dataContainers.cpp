//dataContainers.cpp

#include "dataContainers.h"




std::optional<int> Metadata::getOptionalIntValue(const std::string& tag, int which) {
if (settings.tagExists(tag, which)) {
    return settings.getValue(tag, 0, which);
}
return std::nullopt;
}

std::optional<double> Metadata::getOptionalDoubleValue(const std::string& tag, int which) {
    if (settings.tagExists(tag, which)) {
        return settings.getValue(tag, 0.0, which);
    }
    return std::nullopt;
}

std::optional<std::string> Metadata::getOptionalStringValue(const std::string& tag, int which) {
    if (settings.tagExists(tag, which)) {
        return settings.getValue(tag, "", which);
    }
    return std::nullopt;
}

Metadata::Metadata(const std::string& filepath){
    settings.loadFile(filepath);
    
    std::string val = settings.getValue("metadata:description", "");

    std::cout << "Loaded val " << val << std::endl;
    
    description = getOptionalStringValue("metadata:description");

    if (!description) {
        cerr << "Description not found in metadata file" << endl;
    }else {

        std::cout << "Loaded metadata " << description.value() << std::endl;
    }


};

void Metadata::save(const std::string& filepath) {

        ofxXmlSettings write_settings;

        std::cout << "save called" << std::endl;


        if (description.has_value()) {
            write_settings.setValue("metadata:description", description.value());
        }
        if (brightness.has_value()) {
            write_settings.setValue("metadata:brightness", brightness.value());
        }
        if (color.has_value()) {
            write_settings.setValue("metadata:color", color.value());
        }

        write_settings.saveFile(filepath);
    }
    

template<>
ofxCvColorImage DataPoint<ofImage>::getOFCVImage(){
    ofImage img = loadMedia();

    ofxCvColorImage ofcv_img;
    ofcv_img.allocate(img.getWidth(), img.getHeight());
    ofcv_img.setFromPixels(img.getPixels());
    return ofcv_img;
}



template<>
ofxCvColorImage DataPoint<ofVideoPlayer>::getOFCVImage(){
    ofVideoPlayer vid = loadMedia();

    vid.setFrame(0);
    vid.update();

    ofxCvColorImage ofcv_img;
    ofcv_img.allocate(vid.getWidth(), vid.getHeight());
    ofcv_img.setFromPixels(vid.getPixels());
    return ofcv_img;
}
