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
    
    void setSourceRect(const ofRectangle& rect);
    void setSourcePoints(const vector<ofPoint>& points);
    ofPoint* getSourcePoints();
    const ofPoint* getSourcePoints() const;
    void setTargetRect(const ofRectangle& rect);
    void setTargetPoints(const vector<ofPoint>& points);
    ofPoint* getTargetPoints();
    const ofPoint* getTargetPoints() const;

    OF_DEPRECATED_MSG("Use enableMouseControls() and enableKeyboardShortcuts() instead.", void enable());
    OF_DEPRECATED_MSG("Use disableMouseControls() and disableKeyboardShortcuts() instead.", void disable());
    
    void enableMouseControls();
    void disableMouseControls();
    bool isMouseControlsEnabled() const { return bMouseEnabled; }
    
    void enableKeyboardShortcuts();
    void disableKeyboardShortcuts();
    bool isKeyboardShortcutsEnabled() const { return bKeyboardShortcuts; }

    void update();
    void reset();
    
    ofMatrix4x4 getMatrix() const;
    ofMatrix4x4 getMatrixInverse() const;
    ofMatrix4x4 getMatrix(const ofPoint * srcPoints, const ofPoint * dstPoints) const;
    
    void setCorners(const vector<ofPoint>& corners);
    void setCorner(const ofPoint& p, int cornerIndex);
    void setTopLeftCornerPosition(const ofPoint& p);
    void setTopRightCornerPosition(const ofPoint& p);
    void setBottomRightCornerPosition(const ofPoint& p);
    void setBottomLeftCornerPosition(const ofPoint& p);
    bool isCornerSelected(int cornerIndex) const { return selectedCornerIndex == cornerIndex; }

    void show();
    void hide();
    void toggleShow();
    bool isShowing() const;
    
    void save(const string& path="quadwarp.xml") const;
    void load(const string& path="quadwarp.xml");
    
    void draw() const;
    void drawQuadOutline() const;
    void drawCorners() const;
    void drawHighlightedCorner() const;
    void drawSelectedCorner() const;
    void drawCornerAt(const ofPoint& point) const;
    
    ofPoint srcPoints[4];
    ofPoint dstPoints[4];
    
protected:

    void onMouseMoved(ofMouseEventArgs & mouseArgs);
    void onMousePressed(ofMouseEventArgs & mouseArgs);
    void onMouseDragged(ofMouseEventArgs & mouseArgs);
    void onMouseReleased(ofMouseEventArgs & mouseArgs);
    void keyPressed(ofKeyEventArgs & keyArgs);
    
    ofPoint position;
    
	float anchorSize;
    float anchorSizeHalf;
    int selectedCornerIndex;
    int highlightCornerIndex;
    
    bool bMouseEnabled;
    bool bKeyboardShortcuts;
    bool bShow;
};
