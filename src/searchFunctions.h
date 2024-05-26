//
// Created by felix on 29.04.24.
//

#pragma once
#include "ofMain.h"
#include "ioManager.h"
#include "mediaItems.h"
#include <vector>
#include <functional>
#include <cmath>
#include <tuple>
#include <optional>
#include <algorithm>
#include <stdexcept>



enum class searchModes {
    BRIGHTNESS,
    COLOR,
    FACES,
    EDGES,
    Texture,
    OBJECTS,
};

enum class distanceMetrics {
    DEFAULT,
    MAE,
    MSE,
    COS_SIMILARITY,
    SET_OVERLAP,
};

bool invalidMetricCombination(searchModes mode, distanceMetrics metric);

float mae(const float& a, const float& b);
float mae(const std::vector<float>& a, const std::vector<float>& b) ;
float mse(const float& a, const float& b);

float mse(const std::vector<float>& a, const std::vector<float>& b) ;

float cos_similarity(const float& a, const float& b) ;

float cos_similarity(const std::vector<float>& a, const std::vector<float>& b) ;

float mae(const std::tuple<const std::vector<float>&, const std::vector<float>&, const std::vector<float>&>& a,
          const std::tuple<const std::vector<float>&, const std::vector<float>&, const std::vector<float>&>& b) ;

float mse(const std::tuple<const std::vector<float>&, const std::vector<float>&, const std::vector<float>&>& a,
          const std::tuple<const std::vector<float>&, const std::vector<float>&, const std::vector<float>&>& b) ;

float cos_similarity(const std::tuple<const std::vector<float>&, const std::vector<float>&, const std::vector<float>&>& a,
                     const std::tuple<const std::vector<float>&, const std::vector<float>&, const std::vector<float>&>& b);

float set_interset(const std::vector<std::string>& a, const std::vector<std::string>& b) ;


distanceMetrics get_default_metric(searchModes mode);

template <typename T>
inline float apply_metric(distanceMetrics metric, const std::optional<T>& a, const std::optional<T>& b) {
    if (!a || !b) {
        return (metric == distanceMetrics::COS_SIMILARITY) ? -std::numeric_limits<float>::infinity() : std::numeric_limits<float>::infinity();
    }

    switch (metric) {
        case distanceMetrics::MAE:
            return mae(a.value(), b.value());
        case distanceMetrics::MSE:
            return mse(a.value(), b.value());
        case distanceMetrics::COS_SIMILARITY:
            return cos_similarity(a.value(), b.value());
        default:
            throw std::invalid_argument("Distance metric not implemented");
    }

}


std::optional<std::tuple<const std::vector<float>&, const std::vector<float>&, const std::vector<float>&>>
make_optional_tuple(const std::optional<std::vector<float>>& r, const std::optional<std::vector<float>>& g, const std::optional<std::vector<float>>& b) ;

float score_func(searchModes mode, distanceMetrics metric, const Metadata& am, const Metadata& bm) ;




