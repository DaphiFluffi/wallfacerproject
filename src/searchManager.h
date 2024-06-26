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
    size_t index;
    float score;

};
class SearchManager {
private:

    SearchManager() = default;

public:
    ioManager<ofImage>* image_io_manager = nullptr;
    ioManager<ofVideoPlayer>* video_io_manager = nullptr;
    SearchManager(const SearchManager&) = delete;
    SearchManager& operator=(const SearchManager&) = delete;

    static SearchManager& getInstance() {
        static SearchManager instance;
        return instance;
    }

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

        auto data = image_io_manager->getData();

        std::cout << "init path" <<  data[0].filePath << std::endl;

        std::vector<SearchResult<ofImage>> results;

        auto og_file_path = item->getId();

        for (size_t i = 0; i < data.size(); i++) {
            if(og_file_path ==data[i].filePath) {
                continue;
            }

            float score = score_func(mode, metric, goal, data[i].metadata);
            results.push_back(SearchResult<ofImage>{i, score});
        }

        std::sort(results.begin(), results.end(), [metric](const SearchResult<ofImage>& a, const SearchResult<ofImage>& b) {
                    return (metric == distanceMetrics::COS_SIMILARITY || metric == distanceMetrics::SET_OVERLAP) ? (a.score > b.score) : (a.score < b.score);
                });


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

        auto& data = video_io_manager->getData();
        std::vector<SearchResult<ofVideoPlayer>> results;

        auto og_file_path = item->getId();

        for (size_t i = 0; i < data.size(); i++) {
            if(og_file_path ==data[i].filePath) {
                continue;
            }
            float score = score_func(mode, metric, goal, data[i].metadata);
            results.push_back(SearchResult<ofVideoPlayer>{i, score});
        }

        std::sort(results.begin(), results.end(), [metric](const SearchResult<ofVideoPlayer>& a, const SearchResult<ofVideoPlayer>& b) {
            return (metric == distanceMetrics::COS_SIMILARITY || metric == distanceMetrics::SET_OVERLAP) ? (a.score > b.score) : (a.score < b.score);
        });

        if (max_samples > 0 && max_samples < results.size()) {
            results.resize(max_samples);
        }

        return results;
    }


};