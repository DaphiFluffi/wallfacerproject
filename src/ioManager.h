//ioManager.h
#pragma once

#include "ofMain.h"
#include <string>
#include <vector>
#include <optional>
#include "ofxXmlSettings.h"
#include "metadataGenerator.h"
#include "dataContainers.h"


inline std::string get_metadata_path(const string& filepath){

    return ofFilePath::removeExt(filepath) + ".xml";  
};


inline bool file_exists(std::string filepath){
    ofFile file(filepath);
    return file.exists();

};

template<typename MediaType>
class ioManager {
private:
    ofDirectory dir;
    std::vector<DataPoint<MediaType>> mediaPoints;

    std::string directory;
    std::vector<std::string> extensions;

    metadataGenerator metadataGen;

public:
    ioManager(const std::string& dir, const std::vector<std::string>& extens): directory(dir), extensions(extens) {
    }
    
    void setup(){
        dir.open(directory);
        for (const auto& ext : extensions) {
            dir.allowExt(ext);
        }
        dir.sort(); 

        std::vector<std::string> with_metadata = {};

        for (size_t i = 0; i < dir.size(); ++i) {
            std::string path = dir.getPath(i);

            std::string metadata_path = get_metadata_path(path);

            if( file_exists(metadata_path) ){
                with_metadata.push_back(path);
            } else {
                std::cout << "For file " << path << " metadata file not found at " << metadata_path << std::endl;
            }
        }

        if (with_metadata.size()) {
            mediaPoints.resize(with_metadata.size());


            for (size_t i = 0; i < mediaPoints.size(); ++i) {
                mediaPoints[i].filePath = with_metadata[i];
                mediaPoints[i].metadataPath = get_metadata_path(with_metadata[i]);
                mediaPoints[i].metadata = Metadata(get_metadata_path(with_metadata[i]));
            };
        }

        std::cout << "\n---------------------------------------------\n";
        std::cout << "Media loaded: " << mediaPoints.size() << std::endl;
        std::cout << "Directory: " << dir.path() << std::endl;
    }

    DataPoint<MediaType>& getData(size_t index) {
        if (index >= mediaPoints.size()) {
            throw std::out_of_range("Index out of range");
        }
        return mediaPoints[index];
    }

    size_t size() const {
        return mediaPoints.size();
    }

    void updateMetadata() {

        for (auto& data : mediaPoints) {
            metadataGen.updateMetadata(data);
        }
    }
};
