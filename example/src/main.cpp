#include "ofMain.h"
#include "ofApp.h"

int main()
{
	ofSetupOpenGL(900, 768, OF_WINDOW);
    ofRunApp(std::make_shared<ofApp>());
}
