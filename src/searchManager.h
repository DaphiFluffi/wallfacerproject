//
// Created by felix on 29.04.24.
//

#pragma once
#include "ofMain.h"
#include "ioManager.h"
#include "mediaItems.h"
#include "searchFunctions.h"
#include <vector>
#include <functional>
#include <cmath>
#include <tuple>
#include <optional>
#include <algorithm>
#include <stdexcept>

template <typename MediaType>
struct SearchResult {
    const DataPoint<MediaType>* data;
    float score;

    bool operator<(const SearchResult& other) const {
        return score < other.score;
    }
};

class SearchManager {

private:
    ioManager<ofImage>* image_io_manager = nullptr;
    ioManager<ofVideoPlayer>* video_io_manager = nullptr;
public:

    void setup(ioManager<ofImage>* img, ioManager<ofVideoPlayer>* vid) {
        if (!img && !vid) {
            throw std::invalid_argument("At least one manager must be non-null");
        }
        image_io_manager = img;
        video_io_manager = vid;
    }


    std::vector<SearchResult<ofImage>> search_imgs(mediaItem* item,searchModes mode, distanceMetrics metric, int max_samples = -1) {


        const auto opt_goal = item->getMetaData();

        if(!opt_goal) {
            std::cerr << "Cant Search no metadata available" << std::endl;
            return {};
        }
        const Metadata& goal = *opt_goal;


        if (!image_io_manager) {
            std::cerr << "Image manager is not set up." << std::endl;
            return {};
        }

        const auto data = image_io_manager->getData();

        std::vector<SearchResult<ofImage>> results;

        for (auto& datapoint : data) {
            float score = score_func(mode, metric, goal, datapoint.metadata);
            results.push_back(SearchResult<ofImage>{&datapoint, score});
        }

        std::sort(results.begin(), results.end());

        if (max_samples > 0 && max_samples < static_cast<int>( results.size())) {
            results.resize(max_samples);
        }

        return results;
    };

    std::vector<SearchResult<ofVideoPlayer>> search_videos(mediaItem* item, searchModes mode, distanceMetrics metric, int max_samples = -1) {


        const auto opt_goal = item->getMetaData();

        if(!opt_goal) {
            std::cerr << "Cant Search no metadata available" << std::endl;
            return {};
        }
        const Metadata& goal = *opt_goal;

        if (video_io_manager) {
            std::cerr << "Video manager is not set up." << std::endl;
            return {};
        }

        const auto& data = video_io_manager->getData();
        std::vector<SearchResult<ofVideoPlayer>> results;

        for (auto& datapoint : data) {
            float score = score_func(mode, metric, goal, datapoint.metadata);
            results.push_back(SearchResult<ofVideoPlayer>{&datapoint, score});
        }

        std::sort(results.begin(), results.end());

        if (max_samples > 0 && max_samples < results.size()) {
            results.resize(max_samples);
        }

        return results;
    }


};