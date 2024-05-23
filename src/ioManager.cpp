//
// Created by felix on 17.05.24.
//
#include "ioManager.h"
#include "metadataGenerator.h"


template<>
void ioManager<ofImage>::updateMetadata(){

    for (auto& data : mediaPoints) {
        metadataGenerator::getInstance().updateMetadata(data);
    }
};

template<>
void ioManager<ofVideoPlayer>::updateMetadata(){

    for (auto& data : mediaPoints) {
        metadataGenerator::getInstance().updateMetadata(data);
    }
};

