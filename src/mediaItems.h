#pragma once

#include "ofMain.h"
#include <memory>
#include <tuple>
#include "cameraManager.h"
#include "ioManager.h"



enum class MediaType
{
    IMAGE,
    VIDEO,
    FEED
};  


class mediaItem
{

private:
public:
    int drawingPriotity = 0;
    MediaType type;

    virtual void draw(float display_size = 1.0) = 0;
    virtual void draw(float x, float y, float w, float h) = 0;
    virtual void drawMetadata(float x, float y, float w, float h) {};

    virtual void update() = 0;
    virtual void togglePause(){};
    
    virtual void load(const std::string &path){};


    mediaItem(MediaType type) : type(type){};

    virtual ~mediaItem() = default;

    int start_x, start_y;
    int current_x, current_y, start_w, start_h;

    float display_size;

    void assign_start_cords(int x, int y, int width, float display_size = 1.0)
    {
        start_x = current_x = x;
        start_y = current_y = y;
        start_w = start_h = width;
        this->display_size = display_size;
    };

    std::tuple<float, float> get_center()
    {

        return {current_x + start_w / 2, current_y + start_h / 2};
    };

    ofRectangle get_bounding_box(float size)
    {

        auto [center_x, center_y] = get_center();

        float x = center_x - start_w * size / 2;

        float y = center_y - start_h * size / 2;

        return ofRectangle(x, y, start_w * size, start_h * size);
    }

    ofRectangle get_bounding_box()
    {

        return get_bounding_box(display_size);
    }

    bool contains(float x, float y)
    {
        auto box = get_bounding_box();
        return box.inside(x, y);
    }


};



class mediaVideo : public mediaItem
{
    ofVideoPlayer item;
    DataPoint<ofVideoPlayer>& datapoint;
    bool active = true;


public:
    mediaVideo(DataPoint<ofVideoPlayer>& data) : mediaItem(MediaType::VIDEO), datapoint(data){

        item = data.loadMedia();
        item.setLoopState(OF_LOOP_NORMAL);
        item.play();



    };

    void togglePause(){
        active = !active;
    }

    void stop()
    {
        active = false;
    }

    void play()
    {
        active = true;
    }

    void update() override
    {
        if (active)
            item.update();
    };

    void draw(float size = 1.0) override
    {
        auto box = get_bounding_box();
        item.draw(box.getLeft(), box.getTop(), start_w * display_size, start_h * display_size);
    }

    void draw(float x, float y, float w, float h)
    {
        item.draw(x, y, w, h);
    }
    
    void drawMetadata(float x, float y, float w, float h) override {

        datapoint.metadata.draw(x, y, w, h);
    };
};

class mediaImage : public mediaItem
{
    ofImage item;
    DataPoint<ofImage>& datapoint;

public:
    mediaImage(DataPoint<ofImage>& data) : mediaItem(MediaType::IMAGE), datapoint(data){

        item = data.loadMedia();
    };

    void draw(float size = 1.0) override
    {

        auto box = get_bounding_box();
        item.draw(box.getLeft(), box.getTop(), start_w * display_size, start_h * display_size);
    }
    void draw(float x, float y, float w, float h)
    {
        item.draw(x, y, w, h);
    }

    void update() override{};


    void drawMetadata(float x, float y, float w, float h) override {

        datapoint.metadata.draw(x, y, w, h);
    };
};

template <typename ofType>
class mediaFeed : public mediaItem
{

private:
    ofType &item;

public:
    mediaFeed(ofType &grab) : mediaItem(MediaType::FEED), item(grab){};

    void update(){};

    void draw(float size = 1.0) override
    {

        auto box = get_bounding_box();
        ofSetHexColor(0xffffff);
        item.draw(box.getLeft(), box.getTop(), start_w * display_size, start_h * display_size);
    }
    void draw(float x, float y, float w, float h)
    {
        ofSetHexColor(0xffffff);
        item.draw(x, y, w, h);
    }
};