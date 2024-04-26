#pragma once

#include "mediaItems.h"

#include <memory>
#include <tuple>
#include "cameraManager.h"
#include "ioManager.h"




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
    vector<std::unique_ptr<mediaItem>> items;
    float rel_border = 0.075f; // the relative border to the edge
    float item_size = 0.8; // how big are the smaller items in here
    int n_items_drawn = 5; // how many items are drawn in  the collector


    collectorItem() : mediaItem(MediaType::COLLECTOR){};

    void update() override {

        const int n_items = min(n_items_drawn, static_cast<int>(items.size()));

        for (int i = 0; i < n_items; ++i) {
            items[i]->update();
        };
    };

    void draw(float size = 1.0) override
    {
        const auto box = get_bounding_box();
        ofSetColor(color);
        ofDrawRectangle(box);

        const int n_items = min(n_items_drawn, static_cast<int>(items.size()));

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
        ofSetColor(color);
        ofDrawRectangle(x_in,y_in,width,height);

        const int n_items = min(n_items_drawn, static_cast<int>(items.size()));

        if (!n_items) return;



        // draw the contained items in an overlapping manner from top left to bottom right
        const float complete_items_width = width * (1-rel_border * 2) ;
        const float complete_items_height = height * (1-rel_border * 2) ;

        float x = x_in + width * rel_border;
        float y = y_in + height * rel_border;

        for (int i = 0; i < n_items; ++i) {

            items[i]->draw(x,y,complete_items_width * item_size, complete_items_height * item_size);
            x += complete_items_width * (1- item_size) / n_items;
            y += complete_items_height * (1- item_size) / n_items;

        };



    }

    void add_item(std::unique_ptr<mediaItem> item)
    {
        if(!item){
            cerr << "Tried to add nullptr to collector" << endl;
            return;
        }
        items.push_back(std::move(item));
    }   

    std::unique_ptr<mediaItem> remove_item(const int index)
    {
        if (index < 0 || index >= items.size()) return nullptr;

        auto item = std::move(items[index]);
        items.erase(items.begin() + index);
        return item;
    }

    std::unique_ptr<mediaItem> remove_item(const mediaItem* item)
    {
        for (int i = 0; i < items.size(); ++i)
        {
            if (items[i].get() == item)
            {
                return remove_item(i);
            }
        }
        return nullptr;
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