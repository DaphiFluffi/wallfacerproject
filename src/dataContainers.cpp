// dataContainers.cpp

#include "dataContainers.h"
#include "fontManager.h"

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


string wrapText(string text, float width, ofTrueTypeFont &font) {
    string typeWrapped = "";
    vector<string> words = ofSplitString(text, " ");
    
    string tempString = "";

    for(int i = 0; i < words.size(); i++) {
        string wrd = words[i];
        if (font.stringWidth(tempString + wrd) < width) {
            tempString += wrd + " ";
        } else {
            typeWrapped += tempString + "\n";
            tempString = wrd + " ";
        }
    }
    
    // Add the last line
    typeWrapped += tempString;
    
    return typeWrapped;
}


void  Metadata::draw(float x, float y, float w, float h){

    fontManager& font_manager = fontManager::getInstance();
    ofNoFill();
    ofSetColor(0); 
    ofDrawRectangle(x, y, w, h); 
    ofFill();

    ofTrueTypeFont& regularFont = font_manager.getRegularFont();
    ofTrueTypeFont& titleFont = font_manager.getTitleFont();

    float textY = y + titleFont.getLineHeight() + 5;

    string title = "Metadata";
    float titleWidth = titleFont.stringWidth(title);
    titleFont.drawString(title, x + (w - titleWidth) / 2, textY);

    textY += regularFont.getLineHeight() + 10;

    if (description.has_value()) {
        string wrappedDesc = wrapText("Description: "+ description.value(), w - 20, regularFont);
        regularFont.drawString(wrappedDesc, x + 10, textY);

        textY += regularFont.getLineHeight() * (std::count(wrappedDesc.begin(), wrappedDesc.end(), '\n') + 1);
    }

    if (brightness.has_value()) {
        string brightnessStr = "Brightness: " + ofToString(brightness.value());
        regularFont.drawString(brightnessStr, x + 10, textY);
        textY += regularFont.getLineHeight();
    }


    if (redHist.has_value() && blueHist.has_value() && greenHist.has_value()){

        textY += 40;

        title = "Color Histograms";
        titleWidth = regularFont.stringWidth(title);
        regularFont.drawString(title, x + (w - titleWidth) / 2, textY);
        textY += 2; 
        
        float histHeight = 100; 
        float binWidth = w / 256; 

        float max_val = std::max({
            *std::max_element(redHist.value().begin(), redHist.value().end()),
            *std::max_element(greenHist.value().begin(), greenHist.value().end()),
            *std::max_element(blueHist.value().begin(), blueHist.value().end())
        });

        auto drawHistogram = [&](const std::optional<std::vector<float>>& hist, ofColor color, float& posY) {
            if (hist.has_value()) {
                ofSetColor(color);
                float prevVal = posY - hist->at(0) * histHeight/ max_val;
                for (size_t i = 1; i < hist->size(); i++) {
                    float currentVal = posY - hist->at(i) * histHeight / max_val;
                    ofDrawLine(x + binWidth * (i - 1), prevVal, x + binWidth * i, currentVal);
                    prevVal = currentVal;
                }
                
            }
        };

        ofNoFill();
        ofDrawRectangle(x, textY, w, histHeight); 
        ofFill();

        textY += histHeight;

        drawHistogram(redHist, ofColor(255, 0, 0), textY);   
        drawHistogram(greenHist, ofColor(0, 255, 0), textY);
        drawHistogram(blueHist, ofColor(0, 0, 255), textY);

        textY += 20;
    };
};


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

    vid.setLoopState(OF_LOOP_NORMAL);
    vid.play();
    vid.update();
    vid.setFrame(1);
    vid.update();

    ofxCvColorImage ofcv_img;
    ofcv_img.allocate(vid.getWidth(), vid.getHeight());
    ofcv_img.setFromPixels(vid.getPixels());
    return ofcv_img;
}
