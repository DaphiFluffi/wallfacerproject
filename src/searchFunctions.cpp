//
// Created by felix on 29.04.24.
//

#include "searchFunctions.h"

float mae(const float& a, const float& b) {
    return std::abs(a - b);
}

float mae(const std::vector<float>& a, const std::vector<float>& b) {
    float sum = 0.0;
    for (size_t i = 0; i < a.size(); ++i) {
        sum += std::abs(a[i] - b[i]);
    }
    return sum / a.size();
}

float mse(const float& a, const float& b) {
    return std::pow(a - b, 2);
}

float mse(const std::vector<float>& a, const std::vector<float>& b) {
    float sum = 0.0;
    for (size_t i = 0; i < a.size(); ++i) {
        sum += std::pow(a[i] - b[i], 2);
    }
    return sum / a.size();
}

float cos_similarity(const float& a, const float& b) {

    std::cerr << "cos_sim of 2 floats called. this should not happen!" << std::endl;
    return 1;
}

float cos_similarity(const std::vector<float>& a, const std::vector<float>& b) {
    float dot_product = 0.0, norm_a = 0.0, norm_b = 0.0;
    for (size_t i = 0; i < a.size(); ++i) {
        dot_product += a[i] * b[i];
        norm_a += a[i] * a[i];
        norm_b += b[i] * b[i];
    }
    return dot_product / (std::sqrt(norm_a) * std::sqrt(norm_b));
}

float mae(const std::tuple<const std::vector<float>&, const std::vector<float>&, const std::vector<float>&>& a,
          const std::tuple<const std::vector<float>&, const std::vector<float>&, const std::vector<float>&>& b) {
    return (mae(std::get<0>(a), std::get<0>(b)) + mae(std::get<1>(a), std::get<1>(b)) + mae(std::get<2>(a), std::get<2>(b))) / 3;
}

float mse(const std::tuple<const std::vector<float>&, const std::vector<float>&, const std::vector<float>&>& a,
          const std::tuple<const std::vector<float>&, const std::vector<float>&, const std::vector<float>&>& b) {
    return (mse(std::get<0>(a), std::get<0>(b)) + mse(std::get<1>(a), std::get<1>(b)) + mse(std::get<2>(a), std::get<2>(b))) / 3;
}

float cos_similarity(const std::tuple<const std::vector<float>&, const std::vector<float>&, const std::vector<float>&>& a,
                     const std::tuple<const std::vector<float>&, const std::vector<float>&, const std::vector<float>&>& b) {
    return (cos_similarity(std::get<0>(a), std::get<0>(b)) + cos_similarity(std::get<1>(a), std::get<1>(b)) + cos_similarity(std::get<2>(a), std::get<2>(b))) / 3;
}


float set_interset(const std::vector<std::string> &a, const std::vector<std::string> &b) {

    float v = 0;

    for (const auto& v1: a) {
        for (const auto& v2: b) {
            if (v1 == v2) {
                v += 1;
            }
        }
    }
    return v;
}


bool invalidMetricCombination(searchModes mode, distanceMetrics metric) {

    if (metric == distanceMetrics::SET_OVERLAP && mode != searchModes::OBJECTS)
        return true;

    if (metric != distanceMetrics::SET_OVERLAP && mode == searchModes::OBJECTS)
        return true;


    if (metric == distanceMetrics::COS_SIMILARITY) {
        switch (mode) {
            case searchModes::BRIGHTNESS:
            case searchModes::FACES:
                return true;
            default:
                return false;
        }

    }
    return false;

};


distanceMetrics get_default_metric(searchModes mode) {
    switch (mode) {
        case searchModes::BRIGHTNESS:
        case searchModes::FACES:
            return distanceMetrics::MAE;
        case searchModes::COLOR:
        case searchModes::EDGES:
        case searchModes::Texture:
            return distanceMetrics::COS_SIMILARITY;
        case searchModes::OBJECTS:
            return distanceMetrics::SET_OVERLAP;
        default:
            throw std::logic_error("mode not implemented");
    }

    return distanceMetrics::DEFAULT;
}


std::optional<std::tuple<const std::vector<float>&, const std::vector<float>&, const std::vector<float>&>>
make_optional_tuple(const std::optional<std::vector<float>>& r, const std::optional<std::vector<float>>& g, const std::optional<std::vector<float>>& b) {
    if (!r || !g || !b) {
        return std::nullopt;
    }
    return std::tuple<const std::vector<float>&, const std::vector<float>&, const std::vector<float>&>{r.value(), g.value(), b.value()};
}

float score_func(searchModes mode, distanceMetrics metric, const Metadata& am, const Metadata& bm) {
    if (metric == distanceMetrics::DEFAULT) {
        metric = get_default_metric(mode);
    }

    switch (mode) {
        case searchModes::BRIGHTNESS: {
            if (am.brightness && bm.brightness) {
                return apply_metric(metric, am.brightness, bm.brightness);
            }
        }
        break;
        case searchModes::COLOR: {
            auto a_tup = make_optional_tuple(am.redHist, am.greenHist, am.blueHist);
            auto b_tup = make_optional_tuple(bm.redHist, bm.greenHist, bm.blueHist);
            if (a_tup && b_tup) {
                return apply_metric(metric, a_tup, b_tup);
            }
        }
        case searchModes::FACES: {
            if (am.n_faces && bm.n_faces) {
                return apply_metric(metric, am.n_faces, bm.n_faces);
            }
        }
        case searchModes::EDGES: {
            if (am.edgeHist && bm.edgeHist) {
                return apply_metric(metric, am.edgeHist, bm.edgeHist);
            }
        }
        case searchModes::Texture: {
            if (am.textureHist && bm.textureHist) {
                return apply_metric(metric, am.textureHist, bm.textureHist);
            }
        }
        case searchModes::OBJECTS: {
            if (am.objects && bm.objects) {
                return set_interset(am.objects.value(), bm.objects.value());
            }
        }

        break;
        default:
            throw std::logic_error("Mode not implemented");
    }
    return std::numeric_limits<float>::infinity();
}


