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
    void setSourceRect  ( const ofRectangle& rect , bool bInvert = false);
    void update         ();
    void update         ( ofPoint* srcPoints, ofPoint* dstPoints );
    void reset          ();

    void show           ();
    void hide           ();
    void toggleShow     ();
    void draw           ();
    
    float* getMatrix    ();
    
    void onMouseDragged ( ofMouseEventArgs &mouseArgs );
    
    float matrix[ 16 ];
    
	ofPoint		srcPoints[ 4 ];
	ofPoint		dstPoints[ 4 ];
    
	ofxMSAInteractiveObject		anchors[ 4 ];
	int							anchorSize;
    
    bool bShow;
};