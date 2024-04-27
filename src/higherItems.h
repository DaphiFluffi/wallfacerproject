#pragma once

#include "mediaItems.h"

#include <memory>
#include <tuple>
#include "cameraManager.h"
#include "ioManager.h"
#include "mediaGrid.h"



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

    void togglePause() override{
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
        ofSetColor(ofColor::white);

        item.draw(box.getLeft(), box.getTop(), start_w * display_size, start_h * display_size);
    }

    void draw(const float x,const  float y,const  float w,const  float h) override {
        ofSetColor(ofColor::white);

        item.draw(x, y, w, h);
    }
    
    void drawMetadata(const float x,const  float y,const  float w,const  float h) override {

        datapoint.metadata.draw(x, y, w, h);
    };
};

class mediaImage : public mediaItem
{
    ofImage item;
    DataPoint<ofImage>& datapoint;

public:
    explicit mediaImage(DataPoint<ofImage>& data) : mediaItem(MediaType::IMAGE), datapoint(data){

        item = data.loadMedia();
    };

    void draw(float size = 1.0) override
    {

        auto box = get_bounding_box();
        ofSetColor(ofColor::white);

        item.draw(box.getLeft(), box.getTop(), start_w * display_size, start_h * display_size);
    }
    void draw(const float x,const  float y,const  float w,const  float h) override
    {
        ofSetColor(ofColor::white);

        item.draw(x, y, w, h);
    }

    void update() override{};


    void drawMetadata(const float x,const  float y,const  float w,const  float h) override {

        datapoint.metadata.draw(x, y, w, h);
    };
};


class collectorItem : public mediaItem
{

private:
    ofColor color = ofColor(100);


public:

    int n_items = 2;


    float rel_border = 0.075f; // the relative border to the edge
    float item_size = 0.8; // how big are the smaller items in here
    int n_items_drawn = 5; // how many items are drawn in  the collector
    float buffer_percentage = 0.05f;
    mediaGrid grid = mediaGrid(3, 3, 30, 300);



    collectorItem() : mediaItem(MediaType::COLLECTOR) {
        grid.adjustToCenteredSquare(buffer_percentage);
    };

    void update() override {


        auto items = grid.get_n_first_full(n_items_drawn);

        for (auto& item : items) {
            item->update();
        };
    };

    void draw(float size = 1.0) override
    {
        const auto box = get_bounding_box();
        ofSetColor(color);
        ofDrawRectangle(box);

        auto items = grid.get_n_first_full(n_items_drawn);

        const int n_items = std::min(n_items_drawn, static_cast<int>(items.size()));

        if (!n_items) return;



        // draw the contained items in an overlapping manner from top left to bottom right
        const float complete_items_width = box.width * (1-rel_border * 2) ;
        const float complete_items_height = box.height * (1-rel_border * 2) ;

        float x = box.x + box.width * rel_border;
        float y = box.y + box.height * rel_border;

        for (int i = 0; i < n_items; ++i) {

            items[i]->draw(x,y,complete_items_width * item_size, complete_items_height * item_size);
            x += complete_items_width * (1- item_size) / n_items;
            y += complete_items_height * (1- item_size) / n_items;

        };


    }
    void draw(const float x_in,const  float y_in,const  float width,const  float height) override
    {
        //ofSetColor(color);
        //ofDrawRectangle(x_in,y_in,width,height);

        // auto items = grid.get_n_first_full(n_items_drawn);
        // const int n_items = min(n_items_drawn, static_cast<int>(items.size()));

        // if (!n_items) return;



        // // draw the contained items in an overlapping manner from top left to bottom right
        // const float complete_items_width = width * (1-rel_border * 2) ;
        // const float complete_items_height = height * (1-rel_border * 2) ;

        // float x = x_in + width * rel_border;
        // float y = y_in + height * rel_border;

        // for (int i = 0; i < n_items; ++i) {

        //     items[i]->draw(x,y,complete_items_width * item_size, complete_items_height * item_size);
        //     x += complete_items_width * (1- item_size) / n_items;
        //     y += complete_items_height * (1- item_size) / n_items;

        // };

        grid.draw();

    }



    void add_item(std::unique_ptr<mediaItem> item)
    {

        grid.addItem(std::move(item));
    }


    std::unique_ptr<mediaItem> remove_item(mediaItem* item)
    {
        return std::move(grid.pop_item(item));
    }

    mediaGrid* getGrid() {
        return &grid;
    }

};






template <typename ofType>
class mediaFeed : public mediaItem
{

private:
    ofType &item;

public:
    mediaFeed(ofType &grab) : mediaItem(MediaType::FEED), item(grab){};

    void update() override{};

    void draw(float size = 1.0) override
    {
        const auto box = get_bounding_box();
        ofSetHexColor(0xffffff);
        item.draw(box.getLeft(), box.getTop(), start_w * display_size, start_h * display_size);
    }
    void draw(const float x,const  float y,const  float w,const  float h) override
    {
        ofSetHexColor(0xffffff);
        item.draw(x, y, w, h);
    }
};