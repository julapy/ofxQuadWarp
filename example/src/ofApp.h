#pragma once

#include "ofMain.h"
#include "ofxQuadWarp.h"

class ofApp : public ofBaseApp {
    
public:
		
    void setup();
    void update();
    void draw();
    void exit();

    void keyPressed(int key);


    ofxQuadWarp warper;
    
    ofImage img;
    ofFbo fbo;

    ofPoint points[10];
};
