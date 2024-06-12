//
// Created by felix on 10.05.24.
//

#pragma once

#include "ofMain.h"
#include "ofxCvHaarFinder.h"
#include "ofxOpenCv.h"
#include <cmath>
#include <vector>
#include "opencv2/core.hpp"
#include "opencv2/features2d.hpp"
#include "ioManager.h"
#include <memory>
#include <opencv2/imgproc.hpp>  // Include necessary header for image processing


inline std::string get_log_path(const string &filepath, const std::string className) {


    std::string base;

    // case of camerasnap
    if (filepath == "") {
        base = ofFilePath::join(ofFilePath::join(ofFilePath::join(ofFilePath::getCurrentExeDir(), "data"), "matches"), "camera_snap");
    } else {
        auto name = ofFilePath::getFileName(ofFilePath::removeExt(filepath));

        std::filesystem::path diretory = ofFilePath::getEnclosingDirectory(filepath);


        base = ofFilePath::join(ofFilePath::join(diretory.parent_path().parent_path().string(), "matches"), name);
    }

    auto final_path = ofFilePath::getAbsolutePath(base) + "_" + className + ".jpg";

    return ofFilePath::getAbsolutePath(final_path);
};

struct ExampleObject {
    std::string name;
    cv::Mat descriptors;
    std::vector<cv::KeyPoint> keypoints;
    cv::Mat image;

    // Constructor to initialize all member variables
    ExampleObject(const std::string& name,
        const cv::Mat& descriptors,
        const std::vector<cv::KeyPoint>& keypoints,
        const cv::Mat& image)
        : name(name), descriptors(descriptors), keypoints(keypoints), image(image) {}
};


class SurfComputer {
private:
    ioManager<ofImage> examples_io_manager = ioManager<ofImage>("examples", {"jpg", "png"});

    std::vector<std::unique_ptr<ExampleObject>> example_objects;

    cv::Ptr<cv::DescriptorMatcher> matcher;
    cv::Ptr<cv::SIFT> detector;




    float matchTreshhold = 0.005;

public:
    SurfComputer() {
        examples_io_manager.setup();

        detector = cv::SIFT::create();
        matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::BRUTEFORCE);

        auto data = examples_io_manager.getData();

        for (int i = 0; i < data.size(); i++) {
            auto point = data[i];
            auto img = point.getOFCVImage();


            IplImage *iplImg = img.getCvImage();

            cv::Mat colorMat = cv::cvarrToMat(iplImg);
            cv::Mat resizedColorMat;
            cv::resize(colorMat, resizedColorMat, cv::Size(256, 256));

            cv::Mat descriptors;
            std::vector<cv::KeyPoint> keypoints;

            detector->detectAndCompute(resizedColorMat, cv::noArray(), keypoints, descriptors);

            example_objects.push_back(
                std::make_unique<ExampleObject>(point.metadata.description.value(),
                    descriptors,
                    keypoints,
                    resizedColorMat)
            );
        }


        std::cout << "loaded surf manager with that man objects: " << example_objects.size() << std::endl;
    }

    vector<std::string> getObjects(const cv::Mat &colorMat, const std::string &metadata_path) {
        cv::Mat queryImg;
        cv::resize(colorMat, queryImg, cv::Size(256, 256));

        cv::Mat queryDescriptors;
        std::vector<cv::KeyPoint> queryKeypoints;

        detector->detectAndCompute(queryImg, cv::noArray(), queryKeypoints, queryDescriptors);

       //  std::cout << "shape  " << queryDescriptors.size() << std::endl;

        vector<std::string> matchedObjects;

        for (auto& ptr : example_objects){

            auto& refObject = *ptr;

            std::vector<std::vector<cv::DMatch> > knnMatches;
            std::vector<cv::DMatch> bestMatches;

            matcher->knnMatch(queryDescriptors, refObject.descriptors, knnMatches, 2);

            // save best matches for saving the images
            for (auto matchPair: knnMatches) {
                bestMatches.push_back(matchPair[0]);
            }


            std::vector<cv::DMatch> goodMatches;
            for (auto &matchPair: knnMatches) {
                if (matchPair.size() < 2)
                    continue;

                const cv::DMatch &bestMatch = matchPair[0];
                const cv::DMatch &betterMatch = matchPair[1];

                //std::cout << "distances " << bestMatch.distance << " "  << betterMatch.distance << std::endl;

                if (bestMatch.distance < 0.8 * betterMatch.distance) {
                    goodMatches.push_back(bestMatch);
                }
            }



            // Check if enough good matches are found to consider the object found
            float ratio = (queryKeypoints.size()) ? static_cast<float>(goodMatches.size()) / static_cast<float>(queryKeypoints.size()) : 0;

            // this shouold normalize the score given the number of descriptors in the class
            float class_norm = log(static_cast<float>(refObject.descriptors.rows));

            float score = ratio / class_norm;

            std ::cout << "score " << score << " ratio " << ratio  << " classnorm " << class_norm   << std::endl;

            if (score > matchTreshhold) {
                matchedObjects.push_back(refObject.name);
            }


            // Output information about the data sizes and matrix dimensions
            // std::cout << "Keypoints in query image: " << queryKeypoints.size() << std::endl;
            // std::cout << "Keypoints in query descriptios: " << queryDescriptors.size() << std::endl;
            // std::cout << "Keypoints in reference image: " << refObject.keypoints.size() << std::endl;
            // std::cout << "Keypoints in reference descriptios: " << refObject.descriptors.size() << std::endl;
            // std::cout << "Number of matches: " << bestMatches.size() << std::endl;

            if (!queryKeypoints.empty() && !refObject.keypoints.empty() && !bestMatches.empty()) {
                cv::Mat img_matches;
                cv::drawMatches(queryImg, queryKeypoints, refObject.image, refObject.keypoints, bestMatches, img_matches);

                auto log_path = get_log_path(metadata_path, refObject.name);
                cv::imwrite(log_path, img_matches);
                //std::cout << "Matched image saved to: " << log_path << std::endl;

                std::cout << "Object: " << refObject.name << " - Good Matches: " << goodMatches.size() << std::endl;
            }
        }

        if (matchedObjects.size()) {
            cout << "Found these objects: ";
            for (const auto &a: matchedObjects) {
                std::cout << a << ", ";
            }
            cout << endl;
        }
        return matchedObjects;
    }
};


class EdgeHistogramComputer {
private:
    cv::Mat ver, hor, diag1, diag2, non_directional;
    float threshold = 0.3;

public:
    EdgeHistogramComputer() {
        ver = (cv::Mat_<double>(2, 2) << 1, 1, -1, -1);
        hor = (cv::Mat_<double>(2, 2) << -1, 1, -1, 1);
        diag1 = (cv::Mat_<double>(2, 2) << std::sqrt(2), 0, 0, std::sqrt(2));
        diag2 = (cv::Mat_<double>(2, 2) << 0, std::sqrt(2), std::sqrt(2), 0);
        non_directional = (cv::Mat_<double>(2, 2) << 2, -2, -2, 2);
    }

    vector<float> computeEdgeHistogram(const cv::Mat &grayMat) {
        cv::Mat resizedGrayMat;
        cv::resize(grayMat, resizedGrayMat, cv::Size(256, 256));
        cv::Mat verEdges, horEdges, diag1Edges, diag2Edges, nonDirEdges;
        cv::filter2D(resizedGrayMat, verEdges, -1, ver);

        cv::filter2D(resizedGrayMat, horEdges, -1, hor);
        cv::filter2D(resizedGrayMat, diag1Edges, -1, diag1);
        cv::filter2D(resizedGrayMat, diag2Edges, -1, diag2);
        cv::filter2D(resizedGrayMat, nonDirEdges, -1, non_directional);


        vector<float> histograms(5 * 64, 0.0);
        int binSize = 256 / 8;


        for (int i = 0; i < 256; i++) {
            for (int j = 0; j < 256; j++) {
                int idx = (i / binSize) * 8 + (j / binSize);

                histograms[0 * 64 + idx] += static_cast<float>(abs(verEdges.at<double>(i, j)) > threshold);
                histograms[1 * 64 + idx] += static_cast<float>(abs(horEdges.at<double>(i, j)) > threshold);
                histograms[2 * 64 + idx] += static_cast<float>(abs(diag1Edges.at<double>(i, j)) > threshold);
                histograms[3 * 64 + idx] += static_cast<float>(abs(diag2Edges.at<double>(i, j)) > threshold);
                histograms[4 * 64 + idx] += static_cast<float>(abs(nonDirEdges.at<double>(i, j)) > threshold);
            }
        }

        return histograms;
    }
};


class TextureHistogramComputer {
private:
    std::vector<cv::Mat> kernels;
    std::vector<float> oriantations = {0.0, 0.3, 0.6, 0.9, 1.2, 1.5, 1.8, 2.1};
    std::vector<float> frequencys = {0.3, 0.9, 1.2, 1.8};


    float threshold = 0.3;

public:
    TextureHistogramComputer() {
        for (auto freq: frequencys) {
            for (auto oriantation: oriantations) {
                kernels.push_back(cv::getGaborKernel(cv::Size(31, 31), 5, oriantation, freq, 0));
            }
        }
    }

    vector<float> computeTextureHistogram(const cv::Mat &grayMat) {
        cv::Mat resizedGrayMat;
        cv::resize(grayMat, resizedGrayMat, cv::Size(256, 256));

        vector<float> histogram(kernels.size(), 0.0);

        for (const auto &filter: kernels) {
            cv::Mat filtered;
            cv::filter2D(resizedGrayMat, filtered, -1, filter);
            cv::threshold(filtered, filtered, threshold, 1, cv::THRESH_BINARY);

            histogram.push_back(static_cast<float>(cv::sum(filtered)[0]));
        }
        return histogram;
    }
};


class FaceFinder {
private:
    ofxCvHaarFinder finder;

    FaceFinder() {
    }

public:
    FaceFinder(const FaceFinder &) = delete;

    FaceFinder &operator=(const FaceFinder &) = delete;

    static FaceFinder &getInstance() {
        static FaceFinder instance;
        return instance;
    }

    void setup() {
        finder.setup("haarcascade_frontalface_default.xml");
    }

    vector<ofRectangle> detect_faces(ofPixels &pixels) {
        finder.findHaarObjects(pixels);

        vector<ofRectangle> faces;

        for (unsigned int i = 0; i < finder.blobs.size(); i++) {
            faces.push_back(finder.blobs[i].boundingRect);
        }

        return faces;
    };
};
