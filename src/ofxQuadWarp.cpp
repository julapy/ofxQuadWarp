//
//  ofxQuadWarp.cpp
//  Created by lukasz karluk on 19/06/11.
//

#include "ofxQuadWarp.h"

ofxQuadWarp::ofxQuadWarp() {
    anchorSize = 10;
    anchorSizeHalf = anchorSize * 0.5;
    selectedCornerIndex = -1;
    highlightCornerIndex = -1;
    
    bMouseEnabled = false;
    bKeyboardShortcuts = false;
    bShow = false;
}

ofxQuadWarp::~ofxQuadWarp() {
    disableMouseControls();
    disableKeyboardShortcuts();
}

//----------------------------------------------------- setup.
void ofxQuadWarp::setup() {
    enableMouseControls();
    enableKeyboardShortcuts();
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
void ofxQuadWarp::enable() {    // DEPRECATED.
    enableMouseControls();
}

void ofxQuadWarp::disable() {   // DEPRECATED.
    disableMouseControls();
}

void ofxQuadWarp::enableMouseControls() {
    if(bMouseEnabled == true) {
        return;
    }
    bMouseEnabled = true;
    ofAddListener(ofEvents().mouseMoved, this, &ofxQuadWarp::onMouseMoved);
    ofAddListener(ofEvents().mousePressed, this, &ofxQuadWarp::onMousePressed);
    ofAddListener(ofEvents().mouseDragged, this, &ofxQuadWarp::onMouseDragged);
    ofAddListener(ofEvents().mouseReleased, this, &ofxQuadWarp::onMouseReleased);
}

void ofxQuadWarp::disableMouseControls() {
    if(bMouseEnabled == false) {
        return;
    }
    bMouseEnabled = false;
    try {
        ofRemoveListener(ofEvents().mouseMoved, this, &ofxQuadWarp::onMouseMoved);
        ofRemoveListener(ofEvents().mousePressed, this, &ofxQuadWarp::onMousePressed);
        ofRemoveListener(ofEvents().mouseDragged, this, &ofxQuadWarp::onMouseDragged);
        ofRemoveListener(ofEvents().mouseReleased, this, &ofxQuadWarp::onMouseReleased);
    }
    catch(Poco::SystemException) {
        return;
    }
}

void ofxQuadWarp::enableKeyboardShortcuts() {
    if(bKeyboardShortcuts == true) {
        return;
    }
    bKeyboardShortcuts = true;
    ofAddListener(ofEvents().keyPressed, this, &ofxQuadWarp::keyPressed);
}

void ofxQuadWarp::disableKeyboardShortcuts() {
    if(bKeyboardShortcuts == false) {
        return;
    }
    bKeyboardShortcuts = false;
    try {
        ofRemoveListener(ofEvents().keyPressed, this, &ofxQuadWarp::keyPressed);
    }
    catch(Poco::SystemException) {
        return;
    }
}

//----------------------------------------------------- source / target points.
void ofxQuadWarp::setSourceRect(const ofRectangle& r) {
	srcPoints[0].set(r.x, r.y);
	srcPoints[1].set(r.x + r.width, r.y);
	srcPoints[2].set(r.x + r.width, r.y + r.height);
	srcPoints[3].set(r.x, r.y + r.height);
}

void ofxQuadWarp::setSourcePoints(const vector<ofPoint>& points) {
    int t = MIN(4, points.size());
    for(int i=0; i<t; i++) {
        srcPoints[i].set(points[i]);
    }
}

ofPoint* ofxQuadWarp::getSourcePoints() {
    return &srcPoints[0];
}

void ofxQuadWarp::setTargetRect(const ofRectangle& r) {
	dstPoints[0].set(r.x, r.y);
	dstPoints[1].set(r.x + r.width, r.y);
	dstPoints[2].set(r.x + r.width, r.y + r.height);
	dstPoints[3].set(r.x, r.y + r.height);
}

void ofxQuadWarp::setTargetPoints(const vector<ofPoint>& points) {
    int t = MIN(4, points.size());
    for(int i=0; i<t; i++) {
        dstPoints[i].set(points[i]);
    }
}

ofPoint* ofxQuadWarp::getTargetPoints() {
    return &dstPoints[0];
}

//----------------------------------------------------- matrix.
ofMatrix4x4 ofxQuadWarp::getMatrix() const {
    return getMatrix(&srcPoints[0], &dstPoints[0]);
}

ofMatrix4x4 ofxQuadWarp::getMatrixInverse() const {
    return getMatrix(&dstPoints[0], &srcPoints[0]);
}

ofMatrix4x4 ofxQuadWarp::getMatrix(const ofPoint* srcPoints, const ofPoint* dstPoints) const {
    
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

    return matrixTemp;
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
void ofxQuadWarp::onMouseMoved(ofMouseEventArgs& mouseArgs) {
    if(bShow == false) {
        return;
    }
    
    ofPoint mousePoint(mouseArgs.x, mouseArgs.y);
    mousePoint -= position;
	for(int i=0; i<4; i++) {
        ofPoint & dstPoint = dstPoints[i];
		if(mousePoint.distance(dstPoint) <= anchorSizeHalf) {
            highlightCornerIndex = i;
            return;
		}
	}
    highlightCornerIndex = -1;
}

void ofxQuadWarp::onMousePressed(ofMouseEventArgs& mouseArgs) {
    if(bShow == false) {
        return;
    }
    
    ofPoint mousePoint(mouseArgs.x, mouseArgs.y);
    mousePoint -= position;
	for(int i=0; i<4; i++) {
        ofPoint & dstPoint = dstPoints[i];
		if(mousePoint.distance(dstPoint) <= anchorSizeHalf) {
			dstPoint.set(mousePoint);
            selectedCornerIndex = i;
            return;
		}
	}
    selectedCornerIndex = -1;
}

void ofxQuadWarp::onMouseDragged(ofMouseEventArgs& mouseArgs) {
    if(bShow == false) {
        return;
    }
    if(selectedCornerIndex < 0 || selectedCornerIndex > 3) {
        return;
    }
    
    ofPoint mousePoint(mouseArgs.x, mouseArgs.y);
    mousePoint -= position;
	dstPoints[selectedCornerIndex].set(mousePoint);
}

void ofxQuadWarp::onMouseReleased(ofMouseEventArgs& mouseArgs) {
    if(bShow == false) {
        return;
    }
    if(selectedCornerIndex < 0 || selectedCornerIndex > 3) {
        return;
    }
    
    ofPoint mousePoint(mouseArgs.x, mouseArgs.y);
    mousePoint -= position;
	dstPoints[selectedCornerIndex].set(mousePoint);
}

void ofxQuadWarp::keyPressed(ofKeyEventArgs& keyArgs) {
    if(bShow == false) {
        return;
    }
    
    switch (keyArgs.key) {
        case '1':
            selectedCornerIndex = 0;
            break;
        case '2':
            selectedCornerIndex = 1;
            break;
        case '3':
            selectedCornerIndex = 2;
            break;
        case '4':
            selectedCornerIndex = 3;
            break;
        default:
            break;
    }
    
    if(selectedCornerIndex < 0 || selectedCornerIndex > 3) {
        return;
    }
    
    float nudgeAmount = 0.3;
    ofPoint & selectedPoint = dstPoints[selectedCornerIndex];
    
    switch (keyArgs.key) {
        case OF_KEY_LEFT:
            selectedPoint.x -= nudgeAmount;
            break;
        case OF_KEY_RIGHT:
            selectedPoint.x += nudgeAmount;
            break;
        case OF_KEY_UP:
            selectedPoint.y -= nudgeAmount;
            break;
        case OF_KEY_DOWN:
            selectedPoint.y += nudgeAmount;
            break;
        default:
            break;
    }
}

//----------------------------------------------------- corners.
void ofxQuadWarp::setCorners(const vector<ofPoint>& corners) {
    vector<ofPoint> _corners = corners;
    _corners.resize(4);
    setTopLeftCornerPosition(_corners[0]);
    setTopRightCornerPosition(_corners[1]);
    setBottomRightCornerPosition(_corners[2]);
    setBottomLeftCornerPosition(_corners[3]);
}

void ofxQuadWarp::setCorner(const ofPoint& p, int cornerIndex) {
    cornerIndex = ofClamp(cornerIndex, 0, 3);
    dstPoints[cornerIndex].set(p);
}

void ofxQuadWarp::setTopLeftCornerPosition(const ofPoint& p) {
    setCorner(p, 0);
}

void ofxQuadWarp::setTopRightCornerPosition(const ofPoint& p) {
    setCorner(p, 1);
}

void ofxQuadWarp::setBottomRightCornerPosition(const ofPoint& p) {
    setCorner(p, 2);
}

void ofxQuadWarp::setBottomLeftCornerPosition(const ofPoint& p) {
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
    bShow = !bShow;
}

bool ofxQuadWarp::isShowing() {
    return bShow;
}

//----------------------------------------------------- save / load.
void ofxQuadWarp::save(const string& path) {
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

void ofxQuadWarp::load(const string& path) {
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
}

//----------------------------------------------------- show / hide.
void ofxQuadWarp::draw() {
    if(bShow == false) {
        return;
    }
    
    drawQuadOutline();
    drawCorners();
    drawHighlightedCorner();
    drawSelectedCorner();
}

void ofxQuadWarp::drawQuadOutline() {
    if(bShow == false) {
        return;
    }
    
    for(int i=0; i<4; i++) {
        int j = (i+1) % 4;
        ofDrawLine(dstPoints[i].x + position.x,
                   dstPoints[i].y + position.y,
                   dstPoints[j].x + position.x,
                   dstPoints[j].y + position.y);
    }
}

void ofxQuadWarp::drawCorners() {
    if(bShow == false) {
        return;
    }

	for(int i=0; i<4; i++) {
        ofPoint & point = dstPoints[i];
        drawCornerAt(point);
	}
}

void ofxQuadWarp::drawHighlightedCorner() {
    if(bShow == false) {
        return;
    }
    if(highlightCornerIndex < 0 || highlightCornerIndex > 3) {
        return;
    }

    ofPoint & point = dstPoints[highlightCornerIndex];
    drawCornerAt(point);
}

void ofxQuadWarp::drawSelectedCorner() {
    if(bShow == false) {
        return;
    }
    if(selectedCornerIndex < 0 || selectedCornerIndex > 3) {
        return;
    }
    
    ofPoint & point = dstPoints[selectedCornerIndex];
    drawCornerAt(point);
}

void ofxQuadWarp::drawCornerAt(const ofPoint & point) {
    ofDrawRectangle(point.x + position.x - anchorSizeHalf,
                    point.y + position.y - anchorSizeHalf,
                    anchorSize, anchorSize);
}
