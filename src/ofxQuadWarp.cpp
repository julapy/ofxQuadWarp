//
//  ofxQuadWarp.cpp
//  Created by lukasz karluk on 19/06/11.
//

#include "ofxQuadWarp.h"

ofxQuadWarp::ofxQuadWarp() {
    anchorSize = 10;
    anchorSizeHalf = anchorSize * 0.5;
    selectedCornerIndex = -1;
    
    bEnabled = false;
    bShow = false;
    firstStart = true;
}

ofxQuadWarp::~ofxQuadWarp() {
    disable();
}

//----------------------------------------------------- setup.
void ofxQuadWarp::setup() {
    enable();
    show();
}

//----------------------------------------------------- setters.
void ofxQuadWarp::setPosition(float x, float y) {
    position.x = x;
    position.y = y;
}

void ofxQuadWarp::setAnchorSize(float value) {
    anchorSize = value;
    anchorSizeHalf = anchorSize * 0.5;
}

//----------------------------------------------------- enable / disable.
void ofxQuadWarp::enable() {
    if(bEnabled) {
        return;
    }
    bEnabled = true;
    ofAddListener(ofEvents().mousePressed, this, &ofxQuadWarp::onMousePressed);
    ofAddListener(ofEvents().mouseDragged, this, &ofxQuadWarp::onMouseDragged);
    ofAddListener(ofEvents().mouseReleased, this, &ofxQuadWarp::onMouseReleased);
    ofAddListener(ofEvents().mouseMoved, this, &ofxQuadWarp::onMouseMoved);
    ofAddListener(ofEvents().keyPressed, this, &ofxQuadWarp::keyPressed);
}

void ofxQuadWarp::disable() {
    if(!bEnabled) {
        return;
    }
    try {
        bEnabled = false;
        ofRemoveListener(ofEvents().mousePressed, this, &ofxQuadWarp::onMousePressed);
        ofRemoveListener(ofEvents().mouseDragged, this, &ofxQuadWarp::onMouseDragged);
        ofRemoveListener(ofEvents().mouseReleased, this, &ofxQuadWarp::onMouseReleased);
        ofRemoveListener(ofEvents().keyPressed, this, &ofxQuadWarp::keyPressed);

    }
    catch(Poco::SystemException) {
        return; // we're leaving anyways so no need to delete
    }
}

//----------------------------------------------------- source / target points.
void ofxQuadWarp::setSourceRect(ofRectangle r) {
	srcPoints[0].set(r.x, r.y);
	srcPoints[1].set(r.x + r.width, r.y);
	srcPoints[2].set(r.x + r.width, r.y + r.height);
	srcPoints[3].set(r.x, r.y + r.height);
}

void ofxQuadWarp::setSourcePoints(vector<ofPoint> points) {
    int t = MIN(4, points.size());
    for(int i=0; i<t; i++) {
        srcPoints[i].set(points[i]);
    }
}

ofPoint* ofxQuadWarp::getSourcePoints() {
    return &srcPoints[0];
}

void ofxQuadWarp::setTargetRect(ofRectangle r) {
	dstPoints[0].set(r.x, r.y);
	dstPoints[1].set(r.x + r.width, r.y);
	dstPoints[2].set(r.x + r.width, r.y + r.height);
	dstPoints[3].set(r.x, r.y + r.height);
}


void ofxQuadWarp::setTargetPoints(vector<ofPoint> points) {
    int t = MIN(4, points.size());
    for(int i=0; i<t; i++) {
        dstPoints[i].set(points[i]);
    }
}

ofPoint* ofxQuadWarp::getTargetPoints() {
    return &dstPoints[0];
}

void ofxQuadWarp::setAnchorSize(int grabSize){
    anchorSize = grabSize;
}

//----------------------------------------------------- matrix.
ofMatrix4x4 ofxQuadWarp::getMatrix() {
    return getMatrix(&srcPoints[0], &dstPoints[0]);
}

ofMatrix4x4 ofxQuadWarp::getMatrixInverse() {
    return getMatrix(&dstPoints[0], &srcPoints[0]);
}

ofMatrix4x4 ofxQuadWarp::getMatrix(ofPoint * srcPoints, ofPoint * dstPoints) {
    
    if (bShow || firstStart) {
        firstStart = false; //make sure this runs at least once on load! otherwise you'd have an empty matrix
        
        
	//we need our points as opencv points
	//be nice to do this without opencv?
	CvPoint2D32f cvsrc[4];
	CvPoint2D32f cvdst[4];	
    
	//we set the warp coordinates
	//source coordinates as the dimensions of our window
	cvsrc[0].x = srcPoints[0].x;
	cvsrc[0].y = srcPoints[0].y;
	cvsrc[1].x = srcPoints[1].x;
	cvsrc[1].y = srcPoints[1].y;
	cvsrc[2].x = srcPoints[2].x;
	cvsrc[2].y = srcPoints[2].y;
	cvsrc[3].x = srcPoints[3].x;
	cvsrc[3].y = srcPoints[3].y;
    
	cvdst[0].x = dstPoints[0].x;
	cvdst[0].y = dstPoints[0].y;
	cvdst[1].x = dstPoints[1].x;
	cvdst[1].y = dstPoints[1].y;
	cvdst[2].x = dstPoints[2].x;
	cvdst[2].y = dstPoints[2].y;
	cvdst[3].x = dstPoints[3].x;
	cvdst[3].y = dstPoints[3].y;
    
	//we create a matrix that will store the results
	//from openCV - this is a 3x3 2D matrix that is
	//row ordered
	CvMat * translate = cvCreateMat(3,3,CV_32FC1);
	
	//this is the slightly easier - but supposidly less
	//accurate warping method 
	//cvWarpPerspectiveQMatrix(cvsrc, cvdst, translate); 
    
    
	//for the more accurate method we need to create
	//a couple of matrixes that just act as containers
	//to store our points  - the nice thing with this 
	//method is you can give it more than four points!
	
	CvMat* src_mat = cvCreateMat(4, 2, CV_32FC1);
	CvMat* dst_mat = cvCreateMat(4, 2, CV_32FC1);
    
	//copy our points into the matrixes
	cvSetData(src_mat, cvsrc, sizeof(CvPoint2D32f));
	cvSetData(dst_mat, cvdst, sizeof(CvPoint2D32f));
    
	//figure out the warping!
	//warning - older versions of openCV had a bug
	//in this function.
	cvFindHomography(src_mat, dst_mat, translate);
    
	//get the matrix as a list of floats
	float *mat = translate->data.fl;
    
    
	//we need to copy these values
	//from the 3x3 2D openCV matrix which is row ordered
	//
	// ie:   [0][1][2] x
	//       [3][4][5] y
	//       [6][7][8] w
	
	//to openGL's 4x4 3D column ordered matrix
	//        x  y  z  w   
	// ie:   [0][3][ ][6]
	//       [1][4][ ][7]
	//		 [ ][ ][ ][ ]
	//       [2][5][ ][9]
	//       
    
    ofMatrix4x4 matrixTemp;
	matrixTemp.getPtr()[0]  = mat[0];
	matrixTemp.getPtr()[4]  = mat[1];
	matrixTemp.getPtr()[12] = mat[2];
	
	matrixTemp.getPtr()[1]  = mat[3];
	matrixTemp.getPtr()[5]  = mat[4];
	matrixTemp.getPtr()[13] = mat[5];	
	
	matrixTemp.getPtr()[3]  = mat[6];
	matrixTemp.getPtr()[7]  = mat[7];
	matrixTemp.getPtr()[15] = mat[8];
    
    cvReleaseMat(&translate);
    cvReleaseMat(&src_mat);
    cvReleaseMat(&dst_mat);
        
        computedMatrix = matrixTemp;
        //this only updates when the drag points and debug are on screen/bShow is true - otherwise it would be wasting cpu
        return computedMatrix;
    }else{
        //This returns the pre-computed matrix so it doesnt have to update every fram when you're not using it
        return computedMatrix;
    }


}

void ofxQuadWarp::update() {
    //
}

void ofxQuadWarp::reset() {
    dstPoints[0].set(srcPoints[0]);
    dstPoints[1].set(srcPoints[1]);
    dstPoints[2].set(srcPoints[2]);
    dstPoints[3].set(srcPoints[3]);
}

//----------------------------------------------------- interaction.
void ofxQuadWarp::onMousePressed(ofMouseEventArgs& mouseArgs) {
        if(bShow){
            ofPoint mousePoint(mouseArgs.x, mouseArgs.y);
            mousePoint -= position;
            for(int i=0; i<4; i++) {
                ofPoint & dstPoint = dstPoints[i];
                if(mousePoint.distance(dstPoint) <= anchorSizeHalf) {
                    dstPoint.set(mousePoint);
                    selectedCornerIndex = i;
                }
            }
        }
}

void ofxQuadWarp::onMouseDragged(ofMouseEventArgs& mouseArgs) {
        if(bShow){
            if(selectedCornerIndex < 0 || selectedCornerIndex > 3) {
                return;
            }
            ofPoint mousePoint(mouseArgs.x, mouseArgs.y);
            mousePoint -= position;
            dstPoints[selectedCornerIndex].set(mousePoint);
        }
}

void ofxQuadWarp::onMouseReleased(ofMouseEventArgs& mouseArgs) {
    if(bShow){
        if(selectedCornerIndex < 0 || selectedCornerIndex > 3) {
            return;
        }
        ofPoint mousePoint(mouseArgs.x, mouseArgs.y);
        mousePoint -= position;    
        dstPoints[selectedCornerIndex].set(mousePoint);
        //selectedCornerIndex = -1;
    }
}

void ofxQuadWarp::onMouseMoved(ofMouseEventArgs& mouseArgs) {
  
    
    
    if(bShow){
        ofPoint mousePoint(mouseArgs.x, mouseArgs.y);
        currentMouseLoc = mousePoint;
        mousePoint -= position;
        
        for(int i=0; i<4; i++) {
            ofPoint & dstPoint = dstPoints[i];
            if(mousePoint.distance(dstPoint) <= anchorSizeHalf) {
                nearCornerIndex = i;
                return;
            }else{
                nearCornerIndex= -1;
            }
        }
    }

    
}

void ofxQuadWarp::keyPressed(ofKeyEventArgs& keyArgs) {
   
    float nudgeAmount = 0.3;
    
    
    if(bShow){
        if(selectedCornerIndex > 0 || selectedCornerIndex <= 3) {
            //OMG nudging - but only if there is a corner selected
            switch (keyArgs.key) {
                case OF_KEY_LEFT:
                    dstPoints[selectedCornerIndex].x = (dstPoints[selectedCornerIndex].x-nudgeAmount);
                    break;
                case OF_KEY_RIGHT:
                    dstPoints[selectedCornerIndex].x = (dstPoints[selectedCornerIndex].x+nudgeAmount);
                    break;
                case OF_KEY_UP:
                    dstPoints[selectedCornerIndex].y = (dstPoints[selectedCornerIndex].y-nudgeAmount);
                    break;
                case OF_KEY_DOWN:
                    dstPoints[selectedCornerIndex].y = (dstPoints[selectedCornerIndex].y+nudgeAmount);
                    break;
                default:
                    break;
            }
        }
        
        //snap points to mouse location
        switch (keyArgs.key) {
            case '1':
                if (bShow) {
                    dstPoints[0] = currentMouseLoc;
                    selectedCornerIndex=0;
                }
                break;
            case '2':
                if (bShow) {
                    dstPoints[1] = currentMouseLoc;
                    selectedCornerIndex=1;
                }
                break;
            case '3':
                if (bShow) {
                    dstPoints[2] = currentMouseLoc;
                    selectedCornerIndex=2;
                }
                break;
            case '4':
                if (bShow) {
                    dstPoints[3] = currentMouseLoc;
                    selectedCornerIndex=3;
                }
                break;
            default:
                break;

        }

    }
}

//----------------------------------------------------- corners.
void ofxQuadWarp::setCorners(vector<ofPoint> corners) {
    corners.resize(4);
    setTopLeftCornerPosition(corners[0]);
    setTopRightCornerPosition(corners[1]);
    setBottomRightCornerPosition(corners[2]);
    setBottomLeftCornerPosition(corners[3]);
}

void ofxQuadWarp::setCorner(ofPoint p, int cornerIndex) {
    cornerIndex = ofClamp(cornerIndex, 0, 3);
    dstPoints[cornerIndex].set(p);
}

void ofxQuadWarp::setTopLeftCornerPosition(ofPoint p) {
    setCorner(p, 0);
}

void ofxQuadWarp::setTopRightCornerPosition(ofPoint p) {
    setCorner(p, 1);
}

void ofxQuadWarp::setBottomRightCornerPosition(ofPoint p) {
    setCorner(p, 2);
}

void ofxQuadWarp::setBottomLeftCornerPosition(ofPoint p) {
    setCorner(p, 3);
}

//----------------------------------------------------- show / hide.
void ofxQuadWarp::show() {
    if(bShow) {
        return;
    }
    toggleShow();
}

void ofxQuadWarp::hide() {
    if(!bShow) {
        return;
    }
    toggleShow();
}

void ofxQuadWarp::toggleShow() {
    if(!bShow)
        enable();
    if(bShow)
        disable();
    bShow = !bShow;
}

//----------------------------------------------------- save / load.
void ofxQuadWarp::save(string path) {
    ofXml xml;
    xml.addChild("quadwarp");
    xml.setTo("quadwarp");
    
    xml.addChild("src");
    xml.setTo("src");
    for(int i=0; i<4; i++) {
        xml.addChild("point");
        xml.setToChild(i);
        xml.setAttribute("x", ofToString(srcPoints[i].x));
        xml.setAttribute("y", ofToString(srcPoints[i].y));
        xml.setToParent();
    }
    xml.setToParent();
    
    xml.addChild("dst");
    xml.setTo("dst");
    for(int i=0; i<4; i++) {
        xml.addChild("point");
        xml.setToChild(i);
        xml.setAttribute("x", ofToString(dstPoints[i].x));
        xml.setAttribute("y", ofToString(dstPoints[i].y));
        xml.setToParent();
    }
    xml.setToParent();
    
    xml.setToParent();
    xml.save(path);
}

void ofxQuadWarp::load(string path) {
    ofXml xml;
    bool bOk = xml.load(path);
    if(bOk == false) {
        return;
    }
    
    bOk = xml.setTo("quadwarp");
    if(bOk == false) {
        return;
    }
    
    bOk = xml.setTo("src");
    if(bOk == false) {
        return;
    }
    
    for(int i=0; i<xml.getNumChildren(); i++) {
        bOk = xml.setToChild(i);
        if(bOk == false) {
            continue;
        }
        srcPoints[i].x = ofToFloat(xml.getAttribute("x"));
        srcPoints[i].y = ofToFloat(xml.getAttribute("y"));
        xml.setToParent();
    }
    xml.setToParent();
    
    bOk = xml.setTo("dst");
    if(bOk == false) {
        return;
    }
    
    for(int i=0; i<xml.getNumChildren(); i++) {
        bOk = xml.setToChild(i);
        if(bOk == false) {
            continue;
        }
        dstPoints[i].x = ofToFloat(xml.getAttribute("x"));
        dstPoints[i].y = ofToFloat(xml.getAttribute("y"));
        xml.setToParent();
    }
    xml.setToParent();
    xml.setToParent();
    
    //after loading, compute the matrix
    computedMatrix = getMatrix();
}

//----------------------------------------------------- show / hide.
void ofxQuadWarp::draw() {
    if(!bShow) {
        return;
    }
    ofPushStyle();
    
    drawQuadOutline();
    drawCorners();
    ofPopStyle();
}

void ofxQuadWarp::drawCorners() {
    if(!bShow) {
        return;
    }

	for(int i=0; i<4; i++) {
        if (i==nearCornerIndex) {
            ofSetColor(255);
            ofNoFill();

            ofCircle(dstPoints[i].x + position.x - anchorSizeHalf*.325,
                     dstPoints[i].y + position.y - anchorSizeHalf*.325,
                     anchorSize*1.5);
        }else if(i==selectedCornerIndex){
            ofSetColor(255, 255, 0);
        }else{
            ofSetColor(255,0,255);
        }
        ofFill();

        ofRect(dstPoints[i].x + position.x - anchorSizeHalf, 
               dstPoints[i].y + position.y - anchorSizeHalf, 
               anchorSize, anchorSize);
	}
}

void ofxQuadWarp::drawQuadOutline() {
    if(!bShow) {
        return;
    }
    
    ofSetColor(255, 0, 255);
    for(int i=0; i<4; i++) {
        int j = (i+1) % 4;
        ofLine(dstPoints[i].x + position.x, 
               dstPoints[i].y + position.y, 
               dstPoints[j].x + position.x, 
               dstPoints[j].y + position.y);
    }
}
