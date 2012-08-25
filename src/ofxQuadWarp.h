//
//  ofxQuadWarp.h
//  Created by lukasz karluk on 19/06/11.
//
//  nay added hack to invert on 2011/06/21
//
#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"

class ofxQuadWarp 
{
public: 
    
     ofxQuadWarp();
    ~ofxQuadWarp();
    
    void setup();
    
    void setPosition(float x, float y);
    void setAnchorSize(float value);
    
    void setSourceRect(ofRectangle rect);
    void setSourcePoints(vector<ofPoint> points);
    ofPoint* getSourcePoints();
    void setTargetRect(ofRectangle rect);
    void setTargetPoints(vector<ofPoint> points);
    ofPoint* getTargetPoints();
    
    void enable();
    void disable();
    
    void update();
    void reset();
    
    ofMatrix4x4 getMatrix();
    ofMatrix4x4 getMatrixInverse();
    ofMatrix4x4 getMatrix(ofPoint * srcPoints, ofPoint * dstPoints);
    
    void setCorners(vector<ofPoint> corners);
    void setCorner(ofPoint p, int cornerIndex);
    void setTopLeftCornerPosition(ofPoint p);
    void setTopRightCornerPosition(ofPoint p);
    void setBottomRightCornerPosition(ofPoint p);
    void setBottomLeftCornerPosition(ofPoint p);
    bool isCornerSelected(int cornerIndex) { return selectedCornerIndex == cornerIndex; }

    void show();
    void hide();
    void toggleShow();

    void draw();
    void drawCorners();
    void drawQuadOutline();
    
	ofPoint srcPoints[4];
	ofPoint	dstPoints[4];
    
protected:
    
    void onMousePressed(ofMouseEventArgs& mouseArgs);
    void onMouseDragged(ofMouseEventArgs &mouseArgs);
    void onMouseReleased(ofMouseEventArgs& mouseArgs);
    
    ofPoint position;
    
	float anchorSize;
    float anchorSizeHalf;
    int selectedCornerIndex;
    
    bool bEnabled;
    bool bShow;
};