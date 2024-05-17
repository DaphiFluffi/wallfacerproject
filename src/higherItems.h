#pragma once

#include "mediaItems.h"

#include <memory>
#include <tuple>
#include "cameraManager.h"
#include "ioManager.h"
#include "mediaGrid.h"
#include "ofxGui.h"
#include "Buttons.h"




class CustomGuiGroup : public ofxGuiGroup {
public:

    bool frozen = false;
    CustomGuiGroup() : ofxGuiGroup() {
        header = false;  // Disable the header by default in this custom class
    }

    // Constructor with parameters
    CustomGuiGroup(const ofParameterGroup &parameters, const std::string &filename = "settings.xml", float x = 10, float y = 10)
        : ofxGuiGroup(parameters, filename, x, y) {
        header = false;
    }

    // Override the render method to exclude header drawing
    void render() override {
        for (auto& element : collection) {
            element->draw();
        }
    }

    // Override mouse events to exclude header interactions
    bool mousePressed(ofMouseEventArgs &args) override {
        if(frozen) return false;
        if (!b.inside(args.x, args.y)) return false; // Early exit if click is outside the group bounds
        for (auto& element : collection) {
            if (element->mousePressed(args)) {
                return true;
            }
        }
        return false;
    }

    bool mouseReleased(ofMouseEventArgs &args) override {
        if(frozen) return false;

        for (auto& element : collection) {
            if (element->mouseReleased(args)) {
                return true;
            }
        }
        return false;
    }

    bool mouseDragged(ofMouseEventArgs &args) override {
        if(frozen) return false;

        for (auto& element : collection) {
            if (element->mouseDragged(args)) {
                return true;
            }
        }
        return false;
    }

    bool mouseMoved(ofMouseEventArgs &args) override {
        if(frozen) return false;

        for (auto& element : collection) {
            if (element->mouseMoved(args)) {
                return true;
            }
        }
        return false;
    }

    bool mouseScrolled(ofMouseEventArgs &args) override {
        for (auto& element : collection) {
            if (element->mouseScrolled(args)) {
                return true;
            }
        }
        return false;
    }

    // Optionally provide methods to manually manage the header
    void enableHeader() {
        header = true;
    }

    void disableHeader() {
        header = false;
    }

protected:
    bool header; // This controls whether the header is considered in interactions
};




class labeledItem : public mediaItem {

public:
    CustomGuiGroup gui;
    ofParameter<std::string> textInput;
    std::string title = "";
    bool gui_active = false;

    labeledItem(MediaType type, std::string label = "", std::string init = "", bool frozen = false): mediaItem(type), title(label){

        if (title != "") {

            gui.setup();
            gui.add(textInput.set(title, init));
            gui.setPosition(0, 0 - gui.getHeight() - 20);

            gui.frozen = frozen;

            gui_active = true;
        }
    };

    void update_gui_pos() {

        if(!gui_active) return;

        const auto box = get_bounding_box();

        gui.setPosition(box.x + std::max(box.width/2 - gui.getWidth()/2.0f, 0.0f), box.y - gui.getHeight()/ 2.0f);

    };



    void draw() {
        if (gui_active) {
            gui.draw();
        }
    };

    void reset_label() {
        gui.clear();
        title = "";
        gui_active = false;
    }

};



class mediaVideo : public labeledItem
{
    ofVideoPlayer item;
    DataPoint<ofVideoPlayer>* dataptr;
    bool active = true;


public:
    mediaVideo(DataPoint<ofVideoPlayer>* ptr, std::string label = "", std::string init = "", bool frozen = false) : labeledItem(MediaType::VIDEO, label, init, frozen), dataptr(ptr){

        item = *dataptr->getData();
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

        update_gui_pos();

        if (active)
            item.update();
    };

    void draw(float size = 1.0) override
    {
        auto box = get_bounding_box();
        ofSetColor(ofColor::white);

        item.draw(box.getLeft(), box.getTop(), start_w * display_size, start_h * display_size);

        labeledItem::draw();
    }

    void draw(const float x,const  float y,const  float w,const  float h) override {
        ofSetColor(ofColor::white);

        item.draw(x, y, w, h);
    }
    
    void drawMetadata(const float x,const  float y,const  float w,const  float h) override {

        dataptr->metadata.draw(x, y, w, h);
    };

    virtual const Metadata* getMetaData() {
        return &(dataptr->metadata);
    }
    std::string getId() {
        return dataptr->filePath;
    }
};

class mediaImage : public labeledItem
{
    ofImage item;
    DataPoint<ofImage>* dataptr;

public:



    explicit mediaImage(DataPoint<ofImage>* ptr, std::string label = "", std::string init = "", bool frozen = false) : labeledItem(MediaType::IMAGE, label, init, frozen), dataptr(ptr){

        item = *dataptr->getData();

        if (!item.isAllocated()) {
            std::cerr << "could not load image " << dataptr->filePath << std::endl;
        }
    };



    void draw(float size = 1.0) override
    {

        auto box = get_bounding_box();
        ofSetColor(ofColor::white);
        if (item.isAllocated())

            item.draw(box.getLeft(), box.getTop(), start_w * display_size, start_h * display_size);

        labeledItem::draw();

    }
    void draw(const float x,const  float y,const  float w,const  float h) override
    {
        ofSetColor(ofColor::white);

        item.draw(x, y, w, h);
    }

    void update() override {
        update_gui_pos();

    };


    void drawMetadata(const float x,const  float y,const  float w,const  float h) override {
        if (item.isAllocated())

        dataptr->metadata.draw(x, y, w, h);
    };

    const Metadata* getMetaData() {
        return &(dataptr->metadata);
    }

    std::string getId() {
        return dataptr->filePath;
    }
};



class collectorItem : public labeledItem
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
    //ofxLabel label;

    void setup(){
 // Position the label at x=100, y=100
    }



    collectorItem() : labeledItem(MediaType::COLLECTOR, "Name") {
        grid.adjustToCenteredSquare(buffer_percentage);

    };

    void update() override {

        update_gui_pos();

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

        labeledItem::draw();

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