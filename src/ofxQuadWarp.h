//
//  ofxQuadWarp.h
//  Created by lukasz karluk on 19/06/11.
//
//  nay added hack to invert on 2011/06/21
//
#pragma once

#include "ofxOpenCv.h"

#include "ofVectorMath.h"
#include "ofRectangle.h"

#include "ofEvent.h"
#include "ofEvents.h"
#include "ofEventUtils.h"

#include <string>
#include <vector>

class ofxQuadWarp {
public: 
    ofxQuadWarp();
    ~ofxQuadWarp();
    
    void setup();
    
    void setPosition(float x, float y);
    void setAnchorSize(float value);
    
    void setSourceRect(const ofRectangle &rect);
    void setSourcePoints(const std::vector<glm::vec2> &points);
    std::vector<glm::vec2> &getSourcePoints();
    const std::vector<glm::vec2> &getSourcePoints() const;
    
    void setTargetRect(const ofRectangle &rect);
    void setTargetPoints(const std::vector<glm::vec2> &points);
    std::vector<glm::vec2> &getTargetPoints();
    const std::vector<glm::vec2> &getTargetPoints() const;

    void enableMouseControls();
    void disableMouseControls();
    bool isMouseControlsEnabled() const { return bMouseEnabled; }
    
    void enableKeyboardShortcuts();
    void disableKeyboardShortcuts();
    bool isKeyboardShortcutsEnabled() const { return bKeyboardShortcuts; }

    void update();
    void reset();
    
    glm::mat4 getMatrix() const;
    glm::mat4 getMatrixInverse() const;
    glm::mat4 getMatrix(const std::vector<glm::vec2> &srcPoints,
                        const std::vector<glm::vec2> &dstPoints) const;
    
    void setCorners(const std::vector<glm::vec2> &corners);
    void setCorner(const glm::vec2& p, std::size_t cornerIndex);
    void setTopLeftCornerPosition(const glm::vec2 &p);
    void setTopRightCornerPosition(const glm::vec2 &p);
    void setBottomRightCornerPosition(const glm::vec2 &p);
    void setBottomLeftCornerPosition(const glm::vec2 &p);
    bool isCornerSelected(std::size_t cornerIndex) const { return selectedCornerIndex == cornerIndex; }

    void show();
    void hide();
    void toggleShow();
    bool isShowing() const;
    
    void save(const std::string &path="quadwarp.json") const;
    void load(const std::string &path="quadwarp.json");
    
    void draw() const;
    void drawQuadOutline() const;
    void drawCorners() const;
    void drawHighlightedCorner() const;
    void drawSelectedCorner() const;
    void drawCornerAt(const glm::vec2 &point) const;
    
    std::vector<glm::vec2> srcPoints;
    std::vector<glm::vec2> dstPoints;
    
protected:

    void onMouseMoved(ofMouseEventArgs &mouseArgs);
    void onMousePressed(ofMouseEventArgs &mouseArgs);
    void onMouseDragged(ofMouseEventArgs &mouseArgs);
    void onMouseReleased(ofMouseEventArgs &mouseArgs);
    void keyPressed(ofKeyEventArgs &keyArgs);
    
    glm::vec2 position;
    
	float anchorSize;
    float anchorSizeHalf;
    std::size_t selectedCornerIndex;
    std::size_t highlightCornerIndex;
    
    bool bMouseEnabled;
    bool bKeyboardShortcuts;
    bool bShow;
};
