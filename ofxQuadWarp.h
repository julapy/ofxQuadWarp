//
//  ofxQuadWarp.h
//  emptyExample
//
//  Created by lukasz karluk on 19/06/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
//  nay added hack to invert on 2011/06/21
//
#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxMSAInteractiveObject.h"

class ofxQuadWarp 
{
public: 
    
     ofxQuadWarp();
    ~ofxQuadWarp();
    
    bool bInvert;//hack to invert
    void setSourceRect      ( const ofRectangle& rect , bool bInvert = false);
    ofMatrix4x4 getMatrix   ();
    ofMatrix4x4 getMatrix   ( ofPoint* srcPoints, ofPoint* dstPoints );
    void update             ();
    void reset              ();
    
    void setCorners                     ( ofPoint* corners );
    void setCorner                      ( ofPoint p, int cornerIndex );
    void setTopLeftCornerPosition       ( ofPoint p );
    void setTopRightCornerPosition      ( ofPoint p );
    void setBottomRightCornerPosition   ( ofPoint p );
    void setBottomLeftCornerPosition    ( ofPoint p );

    void show           ();
    void hide           ();
    void toggleShow     ();
    void draw           ();
    
    void onMouseDragged ( ofMouseEventArgs &mouseArgs );
    
	ofPoint srcPoints[ 4 ];
	ofPoint	dstPoints[ 4 ];
    
	ofxMSAInteractiveObject		anchors[ 4 ];
	int							anchorSize;
    
    bool bShow;
};