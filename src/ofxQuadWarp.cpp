//
//  ofxQuadWarp.cpp
//  Created by lukasz karluk on 19/06/11.
//

#include "ofxQuadWarp.h"

#include "ofLog.h"
#include "ofJson.h"

ofxQuadWarp::ofxQuadWarp() {
    anchorSize = 10;
    anchorSizeHalf = anchorSize * 0.5;
    selectedCornerIndex = -1;
    highlightCornerIndex = -1;
    
    bMouseEnabled = false;
    bKeyboardShortcuts = false;
    bShow = false;
    
    srcPoints.resize(4);
    dstPoints.resize(4);
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
    catch(std::exception) {
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
    catch(std::exception) {
        return;
    }
}

//----------------------------------------------------- source / target points.
void ofxQuadWarp::setSourceRect(const ofRectangle &r) {
    srcPoints[0] = { r.x, r.y };
    srcPoints[1] = { r.x + r.width, r.y };
    srcPoints[2] = { r.x + r.width, r.y + r.height };
    srcPoints[3] = { r.x, r.y + r.height };
}

void ofxQuadWarp::setSourcePoints(const std::vector<glm::vec2> &points) {
    if(points.size() < 4) {
        ofLogWarning("ofxQuadWarp::setSourcePoints") << "num of given points is less than 4. num of given is: " << points.size();
    }
    
    auto t = std::min<std::size_t>(4, points.size());
    for(auto i = 0; i < t; i++) {
        srcPoints[i] = points[i];
    }
}

std::vector<glm::vec2> &ofxQuadWarp::getSourcePoints() {
    return srcPoints;
}

const std::vector<glm::vec2> &ofxQuadWarp::getSourcePoints() const {
    return srcPoints;
}

void ofxQuadWarp::setTargetRect(const ofRectangle &r) {
    dstPoints[0] = { r.x, r.y };
    dstPoints[1] = { r.x + r.width, r.y };
    dstPoints[2] = { r.x + r.width, r.y + r.height };
    dstPoints[3] = { r.x, r.y + r.height };
}

void ofxQuadWarp::setTargetPoints(const std::vector<glm::vec2> &points) {
    if(points.size() < 4) {
        ofLogWarning("ofxQuadWarp::setTargetPoints") << "num of given points is less than 4. num of given is: " << points.size();
    }
    
    std::size_t t = std::min<std::size_t>(4, points.size());
    for(auto i = 0; i < t; i++) {
        dstPoints[i] = { points[i].x, points[i].y };
    }
}

std::vector<glm::vec2> &ofxQuadWarp::getTargetPoints() {
    return dstPoints;
}

const std::vector<glm::vec2> &ofxQuadWarp::getTargetPoints() const {
    return dstPoints;
}

//----------------------------------------------------- matrix.
glm::mat4 ofxQuadWarp::getMatrix() const {
    return getMatrix(srcPoints, dstPoints);
}

glm::mat4 ofxQuadWarp::getMatrixInverse() const {
    return getMatrix(dstPoints, srcPoints);
}

glm::mat4 ofxQuadWarp::getMatrix(const std::vector<glm::vec2> &srcPoints,
                                 const std::vector<glm::vec2> &dstPoints) const
{
	auto &&homo = cv::findHomography((const std::vector<cv::Point2f> &)srcPoints,
                                     (const std::vector<cv::Point2f> &)dstPoints);
    
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
    
    glm::mat4 matrix;
    matrix[0][0] = homo.at<double>(0, 0);
	matrix[1][0] = homo.at<double>(0, 1);
    matrix[3][0] = homo.at<double>(0, 2);
	
    matrix[0][1] = homo.at<double>(1, 0);
    matrix[1][1] = homo.at<double>(1, 1);
    matrix[3][1] = homo.at<double>(1, 2);
	
    matrix[0][3] = homo.at<double>(2, 0);
    matrix[1][3] = homo.at<double>(2, 1);
    matrix[3][3] = homo.at<double>(2, 2);
    
    return matrix;
}

void ofxQuadWarp::update() {
    //
}

void ofxQuadWarp::reset() {
    dstPoints[0] = srcPoints[0];
    dstPoints[1] = srcPoints[1];
    dstPoints[2] = srcPoints[2];
    dstPoints[3] = srcPoints[3];
}

//----------------------------------------------------- interaction.
void ofxQuadWarp::onMouseMoved(ofMouseEventArgs &mouseArgs) {
    if(bShow == false) {
        return;
    }
    
    glm::vec2 mousePoint(mouseArgs.x, mouseArgs.y);
    mousePoint -= position;
	for(int i=0; i<4; i++) {
        auto &dstPoint = dstPoints[i];
		if(glm::distance(mousePoint, dstPoint) <= anchorSizeHalf) {
            highlightCornerIndex = i;
            return;
		}
	}
    highlightCornerIndex = -1;
}

void ofxQuadWarp::onMousePressed(ofMouseEventArgs &mouseArgs) {
    if(bShow == false) {
        return;
    }
    
    glm::vec2 mousePoint(mouseArgs.x, mouseArgs.y);
    mousePoint -= position;
	for(int i=0; i<4; i++) {
        auto &dstPoint = dstPoints[i];
		if(glm::distance(mousePoint, dstPoint) <= anchorSizeHalf) {
			dstPoint = mousePoint;
            selectedCornerIndex = i;
            return;
		}
	}
    selectedCornerIndex = -1;
}

void ofxQuadWarp::onMouseDragged(ofMouseEventArgs &mouseArgs) {
    if(bShow == false) {
        return;
    }
    if(selectedCornerIndex < 0 || selectedCornerIndex > 3) {
        return;
    }
    
    glm::vec2 mousePoint(mouseArgs.x, mouseArgs.y);
    mousePoint -= position;
	dstPoints[selectedCornerIndex] = mousePoint;
}

void ofxQuadWarp::onMouseReleased(ofMouseEventArgs &mouseArgs) {
    if(bShow == false) {
        return;
    }
    if(selectedCornerIndex < 0 || selectedCornerIndex > 3) {
        return;
    }
    
    glm::vec2 mousePoint(mouseArgs.x, mouseArgs.y);
    mousePoint -= position;
	dstPoints[selectedCornerIndex] = mousePoint;
}

void ofxQuadWarp::keyPressed(ofKeyEventArgs &keyArgs) {
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
    auto &selectedPoint = dstPoints[selectedCornerIndex];
    
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
void ofxQuadWarp::setCorners(const std::vector<glm::vec2> &corners) {
    if(corners.size() < 4) {
        ofLogWarning("ofxQuadWarp::setCorners") << "num of given corners is less than 4. num of given is: " << corners.size();
    }
    for(auto i = 0; i < std::min<std::size_t>(4, corners.size()); ++i) {
        setCorner(corners[i], i);
    }
}

void ofxQuadWarp::setCorner(const glm::vec2 &p, std::size_t cornerIndex) {
    cornerIndex = ofClamp(cornerIndex, 0, 3);
    dstPoints[cornerIndex] = p;
}

void ofxQuadWarp::setTopLeftCornerPosition(const glm::vec2 &p) {
    setCorner(p, 0);
}

void ofxQuadWarp::setTopRightCornerPosition(const glm::vec2 &p) {
    setCorner(p, 1);
}

void ofxQuadWarp::setBottomRightCornerPosition(const glm::vec2 &p) {
    setCorner(p, 2);
}

void ofxQuadWarp::setBottomLeftCornerPosition(const glm::vec2 &p) {
    setCorner(p, 3);
}

//----------------------------------------------------- show / hide.
void ofxQuadWarp::show() {
    if(isShowing()) return;
    toggleShow();
}

void ofxQuadWarp::hide() {
    if(!isShowing()) return;
    toggleShow();
}

void ofxQuadWarp::toggleShow() {
    bShow = !bShow;
}

bool ofxQuadWarp::isShowing() const {
    return bShow;
}

//----------------------------------------------------- save / load.
void ofxQuadWarp::save(const string &path) const {
    auto &&json = ofLoadJson(path);

    json["quadwarp"]["src"] = ofJson::array();
    json["quadwarp"]["dst"] = ofJson::array();

    for(auto &&p : srcPoints) {
        ofJson &&j = ofJson::object();
        j["x"] = p.x;
        j["y"] = p.y;
        json["quadwarp"]["src"].emplace_back(std::move(j));
    }

    for(auto &&p : dstPoints) {
        ofJson &&j = ofJson::object();
        j["x"] = p.x;
        j["y"] = p.y;
        json["quadwarp"]["dst"].emplace_back(std::move(j));
    }
    
    ofSavePrettyJson(path,json);
}

void ofxQuadWarp::load(const string &path) {
    auto &&json = ofLoadJson(path);
    if(json.is_null()) {
        return;
    }

    if(!json["quadwarp"].is_null()) {
        auto &&j = json["quadwarp"];
        if(!j["src"].is_null()) {
            std::size_t i = 0;
            for(auto &&p : j["src"]) {
                srcPoints[i].x = p["x"].get<float>();
                srcPoints[i].y = p["y"].get<float>();
                ++i;
            }
        }
        
        if(!j["dst"].is_null()) {
            std::size_t i = 0;
            for (auto &&p : j["dst"]) {
                dstPoints[i].x = p["x"].get<float>();
                dstPoints[i].y = p["y"].get<float>();
                ++i;
            }
        }
    }
}

//----------------------------------------------------- show / hide.
void ofxQuadWarp::draw() const {
    if(bShow == false) {
        return;
    }
    
    drawQuadOutline();
    drawCorners();
    drawHighlightedCorner();
    drawSelectedCorner();
}

void ofxQuadWarp::drawQuadOutline() const {
    if(bShow == false) {
        return;
    }
    
    for(std::size_t i = 0; i < 4; i++) {
        std::size_t j = (i + 1) % 4;
        ofDrawLine(dstPoints[i].x + position.x,
                   dstPoints[i].y + position.y,
                   dstPoints[j].x + position.x,
                   dstPoints[j].y + position.y);
    }
}

void ofxQuadWarp::drawCorners() const {
    if(bShow == false) {
        return;
    }

	for(std::size_t i = 0; i < 4; i++) {
        const auto &point = dstPoints[i];
        drawCornerAt(point);
	}
}

void ofxQuadWarp::drawHighlightedCorner() const {
    if(bShow == false) {
        return;
    }
    if(highlightCornerIndex < 0 || 3 < highlightCornerIndex) {
        return;
    }

    const auto &point = dstPoints[highlightCornerIndex];
    drawCornerAt(point);
}

void ofxQuadWarp::drawSelectedCorner() const {
    if(bShow == false) {
        return;
    }
    if(selectedCornerIndex < 0 || 3 < selectedCornerIndex) {
        return;
    }
    
    const auto &point = dstPoints[selectedCornerIndex];
    drawCornerAt(point);
}

void ofxQuadWarp::drawCornerAt(const glm::vec2 &point) const {
    ofDrawRectangle(point.x + position.x - anchorSizeHalf,
                    point.y + position.y - anchorSizeHalf,
                    anchorSize, anchorSize);
}
