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
#include "opencv2/highgui.hpp"
#include "opencv2/features2d.hpp"
#include "ioManager.h"
#include <memory>




class SurfComputer {


    private:
    ioManager<ofImage> examples_io_manager = ioManager<ofImage>("examples", { "jpg", "png" });
    std::vector<std::unique_ptr<cv::Mat>> example_descriptors;
    std::vector<std::string> names;
    cv::Ptr<cv::DescriptorMatcher> matcher;

    cv::Ptr<cv::ORB> detector;

    int matchTreshhold = 200;
public:
    SurfComputer() {
        examples_io_manager.setup();

        detector = cv::ORB::create();
        matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::BRUTEFORCE_HAMMING);

        auto data = examples_io_manager.getData();

        for (int i = 0; i < data.size(); i++) {


            auto point = data[i];
            auto img = point.getOFCVImage();


            img.resize(256, 256);
            IplImage* iplImg = img.getCvImage();

            cv::Mat colorMat = cv::cvarrToMat(iplImg);

            cv::Mat descriptors;
            std::vector<cv::KeyPoint> keypoints;

            detector->detectAndCompute(colorMat, cv::noArray(), keypoints, descriptors);

            example_descriptors.push_back(std::make_unique<cv::Mat>(descriptors));

            names.push_back(point.metadata.description.value());
        }

        if (names.size() != example_descriptors.size())
            throw logic_error("something bad happened");

        std::cout << "loaded surf manager with that man objects: " << example_descriptors.size() << std::endl;
    }

    vector<std::string> getObjects(const cv::Mat& colorMat) {
        cv::Mat resizedColorMat;
        cv::resize(colorMat, resizedColorMat, cv::Size(256, 256));

        cv::Mat descriptors;
        std::vector<cv::KeyPoint> keypoints;

        detector->detectAndCompute(resizedColorMat, cv::noArray(), keypoints, descriptors);

        if( descriptors.cols != 32) {
            std::cerr << "descriptors have 0 cols" << std::endl;
            std::vector<std::string> vec(0);
            return vec;
        }


        vector<std::string> objects;

        for (size_t i = 0; i < example_descriptors.size(); i++) {


            std::vector<std::vector<cv::DMatch>> knnMatches;

            matcher->knnMatch(*example_descriptors[i], descriptors, knnMatches, 2); // Find the 2 nearest matches

            std::vector<cv::DMatch> goodMatches;
            for (auto& matchPair : knnMatches) {
                if (matchPair.size() < 2)
                    continue;

                const cv::DMatch &bestMatch = matchPair[0];
                const cv::DMatch &betterMatch = matchPair[1];

                // Apply Lowe's ratio test
                if (bestMatch.distance < 0.75 * betterMatch.distance) {
                    goodMatches.push_back(bestMatch);
                }
            }

            // Check if enough good matches are found to consider the object found
            if (goodMatches.size() > matchTreshhold) {
                objects.push_back(names[i]);
            }

            std::cout << "Object: " << names[i] << " - Good Matches: " << goodMatches.size() << std::endl;
        }
        
        if (objects.size()) {
            cout << "Found these objects: ";
            for (const auto& a : objects) {
                std::cout << a << ", ";
            }
            cout << endl;
        }
        return objects;
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
    vector<float> computeEdgeHistogram(const cv::Mat& grayMat) {
        cv::Mat resizedGrayMat;
        cv::resize(grayMat, resizedGrayMat, cv::Size(256, 256));
        cv::Mat verEdges, horEdges, diag1Edges, diag2Edges, nonDirEdges;
        cv::filter2D(resizedGrayMat, verEdges, -1, ver);

        cv::filter2D(resizedGrayMat, horEdges, -1, hor);
        cv::filter2D(resizedGrayMat, diag1Edges, -1, diag1);
        cv::filter2D(resizedGrayMat, diag2Edges, -1, diag2);
        cv::filter2D(resizedGrayMat, nonDirEdges, -1, non_directional);



        vector<float> histograms(5* 64, 0.0);
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
    std::vector<float> oriantations = {0.0,0.3,0.6,0.9,1.2,1.5,1.8,2.1};
    std::vector<float> frequencys = {0.3,0.9,1.2,1.8};



    float threshold = 0.3;

public:

    TextureHistogramComputer() {

        for (auto freq : frequencys) {
            for (auto oriantation : oriantations) {
                kernels.push_back(cv::getGaborKernel(cv::Size(31,31),5, oriantation, freq, 0));
            }
        }

    }
    vector<float> computeTextureHistogram(const cv::Mat& grayMat) {


        cv::Mat resizedGrayMat;
        cv::resize(grayMat, resizedGrayMat, cv::Size(256, 256));

        vector<float> histogram(kernels.size(), 0.0);

        for (const auto& filter : kernels) {
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
    FaceFinder(const FaceFinder&) = delete;
    FaceFinder& operator=(const FaceFinder&) = delete;

    static FaceFinder& getInstance() {
        static FaceFinder instance;
        return instance;
    }

    void setup() {
            finder.setup("haarcascade_frontalface_default.xml");
    }

    vector<ofRectangle> detect_faces(ofPixels& pixels){
        finder.findHaarObjects(pixels);

        vector<ofRectangle> faces;

        for(unsigned int i = 0; i < finder.blobs.size(); i++) {
            faces.push_back(finder.blobs[i].boundingRect);
        }

        return faces;
    };


};

