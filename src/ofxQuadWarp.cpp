//
//  ofxQuadWarp.cpp
//  Created by lukasz karluk on 19/06/11.
//

#include "ofxQuadWarp.h"
#include "opencv2/calib3d.hpp"

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
    
    ofRemoveListener(ofEvents().mouseMoved, this, &ofxQuadWarp::onMouseMoved);
    ofRemoveListener(ofEvents().mousePressed, this, &ofxQuadWarp::onMousePressed);
    ofRemoveListener(ofEvents().mouseDragged, this, &ofxQuadWarp::onMouseDragged);
    ofRemoveListener(ofEvents().mouseReleased, this, &ofxQuadWarp::onMouseReleased);
    
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
    
    ofRemoveListener(ofEvents().keyPressed, this, &ofxQuadWarp::keyPressed);
    
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

	//Store the source and destination points into cv::Mat matrices
	cv::Mat src_mat = cv::Mat(4, 2, CV_32FC1);
	cv::Mat dst_mat = cv::Mat(4, 2, CV_32FC1);
	for (int i = 0; i < 4; i++) {
		src_mat.at<float>(i, 0) = srcPoints[i].x;
		src_mat.at<float>(i, 1) = srcPoints[i].y;
		dst_mat.at<float>(i, 0) = dstPoints[i].x;
		dst_mat.at<float>(i, 1) = dstPoints[i].y;
	}

	//figure out the warping!
	//warning - older versions of openCV had a bug
	//in this function.
	cv::Mat translate = cv::findHomography(src_mat, dst_mat);
	
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
	matrixTemp.getPtr()[0]  = translate.at<double>(0);
	matrixTemp.getPtr()[4]  = translate.at<double>(1);
	matrixTemp.getPtr()[12] = translate.at<double>(2);
	
	matrixTemp.getPtr()[1]  = translate.at<double>(3);
	matrixTemp.getPtr()[5]  = translate.at<double>(4);
	matrixTemp.getPtr()[13] = translate.at<double>(5);
	
	matrixTemp.getPtr()[3]  = translate.at<double>(6);
	matrixTemp.getPtr()[7]  = translate.at<double>(7);
	matrixTemp.getPtr()[15] = translate.at<double>(8);
	
	translate.release();
	src_mat.release();
	dst_mat.release();
	
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
    xml.appendChild("quadwarp");
    
    ofXml src = xml.getChild("quadwarp").appendChild("src");
    for (int i = 0; i < 4; i++) {
        auto t = src.appendChild("point");
        t.setAttribute("x", ofToString(srcPoints[i].x));
        t.setAttribute("y", ofToString(srcPoints[i].y));
    }
	
    ofXml dst = xml.getChild("quadwarp").appendChild("dst");
    for (int i = 0; i < 4; i++) {
        auto t = dst.appendChild("point");
        t.setAttribute("x", ofToString(dstPoints[i].x));
        t.setAttribute("y", ofToString(dstPoints[i].y));
    }
    
    xml.save(path);
}

void ofxQuadWarp::load(const string& path) {
    ofXml xml;
    if (!xml.load(path)) return;
    
    auto src = xml.getChild("quadwarp").getChild("src");
    int i = 0;
    for (auto it = src.getChildren().begin(); it != src.getChildren().end(); it++) {
        srcPoints[i].x = it->getAttribute("x").getFloatValue();
        srcPoints[i].y = it->getAttribute("y").getFloatValue();
        i++;
    }
    
    auto dst = xml.getChild("quadwarp").getChild("dst");
    i = 0;
    for (auto it = dst.getChildren().begin(); it != dst.getChildren().end(); it++) {
        dstPoints[i].x = it->getAttribute("x").getFloatValue();
        dstPoints[i].y = it->getAttribute("y").getFloatValue();
        i++;
    }
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
