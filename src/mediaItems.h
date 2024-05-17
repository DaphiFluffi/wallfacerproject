#pragma once


#include "ofMain.h"
#include <tuple>
#include "ioManager.h"
#include <optional>



enum class MediaType
{
    IMAGE,
    VIDEO,
    FEED,
    EMPTY,
    COLLECTOR,
};  


class mediaItem
{

private:
public:
    int drawingPriotity = 0;
    MediaType type;

    virtual ~mediaItem() = default;
    virtual void draw(float display_size = 1.0) = 0;
    virtual void draw(float x, float y, float w, float h) = 0;
    virtual void drawMetadata(float x, float y, float w, float h) {};

    bool is_empty() const {
        return  type == MediaType::EMPTY;
    }

    virtual void update() = 0;
    virtual void togglePause(){};
    
    virtual void load(const std::string &path){};


    explicit mediaItem(MediaType type) : type(type){};


    int start_x, start_y;
    int current_x, current_y, start_w, start_h;

    float display_size;

    void reset() {
        current_x = start_x;
        current_y = start_y;
    }
    void assign_start_cords(int x, int y, int width, float display_size = 1.0)
    {
        start_x = current_x = x;
        start_y = current_y = y;
        start_w = start_h = width;
        this->display_size = display_size;
    };

    std::tuple<float, float> get_center() const
    {

        return {current_x + start_w / 2, current_y + start_h / 2};
    };

    ofRectangle get_bounding_box(const float size) const
    {

        auto [center_x, center_y] = get_center();

        float x = center_x - start_w * size / 2;

        float y = center_y - start_h * size / 2;

        return ofRectangle(x, y, start_w * size, start_h * size);
    }

    ofRectangle get_bounding_box() const
    {

        return get_bounding_box(display_size);
    }

    bool contains(const float x,const  float y) const
    {
        const auto box = get_bounding_box();
        return box.inside(x, y);
    }

    virtual const Metadata* getMetaData() {
        return nullptr;
    }

    virtual std::string getId() {
        return "";
    }

    virtual void reset_label(){};



};




class emptyItem : public mediaItem
{
private:
    ofColor color = ofColor(255);

public:
    emptyItem() : mediaItem(MediaType::EMPTY){};

    void update() override{};

    void draw(float size = 1.0) override
    {

        const auto box = get_bounding_box();
        ofSetColor(color);
        ofDrawRectangle(box);

    }
    void draw(const float x,const  float y,const  float w,const  float h) override
    {
        ofSetColor(color);
        ofDrawRectangle(x,y,w,h);
    }
};

