// dataContainers.cpp

#include "dataContainers.h"

std::optional<int> Metadata::getOptionalIntValue(const std::string &tag, int which)
{
    if (settings.tagExists(tag, which))
    {
        return settings.getValue(tag, 0, which);
    }
    return std::nullopt;
}

std::optional<double> Metadata::getOptionalDoubleValue(const std::string &tag, int which)
{
    if (settings.tagExists(tag, which))
    {
        return settings.getValue(tag, 0.0, which);
    }
    return std::nullopt;
}


std::optional<vector<float>> Metadata::getOptionalDoubleVecValue(const std::string &tag, int size)
{
    
    vector<float> vec;
    
    for (int i = 0; i < size; i++){
        if (!settings.tagExists(tag, i))
        {
            return std::nullopt;
        }

        vec.push_back(settings.getValue(tag, 0.0, i));

    }

    return vec;
}

std::optional<std::string> Metadata::getOptionalStringValue(const std::string &tag, int which)
{
    if (settings.tagExists(tag, which))
    {
        return settings.getValue(tag, "", which);
    }
    return std::nullopt;
}

Metadata::Metadata(const std::string &filepath)
{

    settings.load(filepath);

    if (!settings.tagExists("metadata"))
    {
        cerr << "Metadata tag not found in metadata file "<< filepath << endl;
        return;
    }

    settings.pushTag("metadata");

    description = getOptionalStringValue("description");

    if (!description)
    {
        cerr << "Description not found in metadata file" << endl;
    }
    else
    {

        std::cout << "Loaded metadata " << description.value() << std::endl;
    }

    brightness = getOptionalDoubleValue("brightness");


    // load color histograms
    if (!settings.tagExists("color_histograms"))
    {
        cerr << "color histograms not found " << endl;
    }

    settings.pushTag("color_histograms");
    redHist = getOptionalDoubleVecValue("redHist", 256);
    greenHist = getOptionalDoubleVecValue("greenHist", 256);
    blueHist = getOptionalDoubleVecValue("blueHist", 256);
    
    if (!redHist || !greenHist || !blueHist)
    {
        cerr << "Color histograms not found in metadata file" << endl;
    }
    else
    {
        std::cout << "Loaded color histograms "  << std::endl;
    }
    settings.popTag();
    

    settings.popTag();
};

void Metadata::save(const std::string &filepath)
{

    ofxXmlSettings write_settings;

    std::cout << "save called" << std::endl;

    write_settings.addTag("metadata");
    write_settings.pushTag("metadata");

    if (description.has_value())
    {
        write_settings.setValue("description", description.value());
    }
    if (brightness.has_value())
    {
        write_settings.setValue("brightness", brightness.value());
    }

    write_settings.addTag("color_histograms");
    write_settings.pushTag("color_histograms");

    if (redHist.has_value())
    {
        for (int i = 0; i < redHist.value().size(); i++)
        {
            write_settings.setValue("redHist", redHist.value()[i], i);
        }
    }

    if (greenHist.has_value())
    {
        for (int i = 0; i < greenHist.value().size(); i++)
        {
            write_settings.setValue("greenHist", greenHist.value()[i], i);
        }
    }

    if (blueHist.has_value())
    {
        for (int i = 0; i < blueHist.value().size(); i++)
        {
            write_settings.setValue("blueHist", blueHist.value()[i], i);
        }
    }
    write_settings.popTag();

    // pop metadata tag at the end
    write_settings.popTag();

    write_settings.save(filepath);
}

template <>
ofxCvColorImage DataPoint<ofImage>::getOFCVImage()
{
    ofImage img = loadMedia();

    ofxCvColorImage ofcv_img;
    ofcv_img.allocate(img.getWidth(), img.getHeight());
    ofcv_img.setFromPixels(img.getPixels());
    return ofcv_img;
}

template <>
ofxCvColorImage DataPoint<ofVideoPlayer>::getOFCVImage()
{
    ofVideoPlayer vid = loadMedia();

    vid.setFrame(0);
    vid.update();

    ofxCvColorImage ofcv_img;
    ofcv_img.allocate(vid.getWidth(), vid.getHeight());
    ofcv_img.setFromPixels(vid.getPixels());
    return ofcv_img;
}
