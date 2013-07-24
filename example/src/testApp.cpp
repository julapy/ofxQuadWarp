#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup()
{
    ofBackground( 50 );
    ofSetFrameRate( 60 );
    ofSetVerticalSync( true );
    ofEnableSmoothing();
    
    img.loadImage( "quad_warp_kittens.png" );

    int x = ( ofGetWidth()  - img.width  ) * 0.5;   // center on screen.
    int y = ( ofGetHeight() - img.height ) * 0.5;   // center on screen.
    int w = img.width;
    int h = img.height;
    
    fbo.allocate( w, h );
    
    warper.setSourceRect( ofRectangle( 0, 0, w, h ) );              // this is the source rectangle which is the size of the image and located at ( 0, 0 )
    warper.setTopLeftCornerPosition( ofPoint( x, y ) );             // this is position of the quad warp corners, centering the image on the screen.
    warper.setTopRightCornerPosition( ofPoint( x + w, y ) );        // this is position of the quad warp corners, centering the image on the screen.
    warper.setBottomLeftCornerPosition( ofPoint( x, y + h ) );      // this is position of the quad warp corners, centering the image on the screen.
    warper.setBottomRightCornerPosition( ofPoint( x + w, y + h ) ); // this is position of the quad warp corners, centering the image on the screen.
    warper.setup();
}

//--------------------------------------------------------------
void testApp::update()
{
    if( ofGetFrameNum() % 5 != 0 )  // only update every 5 frames.
        return;
    
    for( int i=0; i<10; i++ )       // randomise points over the image area.
    {
        points[ i ].x = ofRandom( img.width );
        points[ i ].y = ofRandom( img.height );
    }
}

//--------------------------------------------------------------
void testApp::draw()
{
    ofSetColor( ofColor :: white );
    
    //======================== draw image into fbo.
    
    fbo.begin();
    {
        img.draw( 0, 0 );
    }
    fbo.end();
    
    //======================== get our quad warp matrix.
    
    ofMatrix4x4 mat = warper.getMatrix();
    
    //======================== use the matrix to transform our fbo.
    
    glPushMatrix();
    glMultMatrixf( mat.getPtr() );
    {
        fbo.draw( 0, 0 );
    }
    glPopMatrix();
    
    //======================== use the matrix to transform points.

    ofSetLineWidth( 2 );
    ofSetColor( ofColor :: cyan );
    
    for( int i=0; i<9; i++ )
    {
        int j = i + 1;
        
        ofVec3f p1 = mat.preMult( ofVec3f( points[ i ].x, points[ i ].y, 0 ) );
        ofVec3f p2 = mat.preMult( ofVec3f( points[ j ].x, points[ j ].y, 0 ) );
        
        ofLine( p1.x, p1.y, p2.x, p2.y );
    }
    
    //======================== draw quad warp ui.
    
    ofSetColor( ofColor :: magenta );
    warper.draw();
    
    //======================== info.
    
    ofSetColor( ofColor :: white );
    ofDrawBitmapString( "to warp the image, drag the corners of the image.", 20, 30 );
    ofDrawBitmapString( "press 's' to toggle quad warp UI. this will also disable quad warp interaction.", 20, 50 );
}

//--------------------------------------------------------------
void testApp::keyPressed(int key)
{
    if( key == 's' )
        warper.toggleShow();
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}