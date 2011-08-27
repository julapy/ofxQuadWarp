//
//  ofxQuadWarp.cpp
//  emptyExample
//
//  Created by lukasz karluk on 19/06/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "ofxQuadWarp.h"

ofxQuadWarp :: ofxQuadWarp ()
{
    anchorSize = 10;
    
    bShow = true;
    
    setSourceRect( ofRectangle( 0, 0, ofGetWidth(), ofGetHeight() ) );
    reset();
    
    ofAddListener( ofEvents.mouseDragged, this, &ofxQuadWarp :: onMouseDragged );
}

ofxQuadWarp :: ~ofxQuadWarp ()
{
    if( bShow )
    {
        try 
        {
            ofRemoveListener( ofEvents.mouseDragged, this, &ofxQuadWarp :: onMouseDragged );
        }
        catch( Poco::SystemException ) 
        {
            return; // we're leaving anyways so no need to delete
        }
    }
}

void ofxQuadWarp :: setSourceRect ( const ofRectangle& r )
{
	srcPoints[ 0 ].set( r.x,  r.y );
	srcPoints[ 1 ].set( r.x + r.width, r.y );
	srcPoints[ 2 ].set( r.x + r.width, r.y + r.height );
	srcPoints[ 3 ].set( r.x,  r.y + r.height );
}

ofMatrix4x4 ofxQuadWarp :: getMatrix ()
{
    return getMatrix( &srcPoints[ 0 ], &dstPoints[ 0 ] );
}

ofMatrix4x4 ofxQuadWarp :: getMatrixInverse ()
{
    return getMatrix( &dstPoints[ 0 ], &srcPoints[ 0 ] );
}

ofMatrix4x4 ofxQuadWarp :: getMatrix ( ofPoint* srcPoints, ofPoint* dstPoints )
{
	//we need our points as opencv points
	//be nice to do this without opencv?
	CvPoint2D32f cvsrc[4];
	CvPoint2D32f cvdst[4];	
    
	//we set the warp coordinates
	//source coordinates as the dimensions of our window
	cvsrc[ 0 ].x = srcPoints[ 0 ].x;
	cvsrc[ 0 ].y = srcPoints[ 0 ].y;
	cvsrc[ 1 ].x = srcPoints[ 1 ].x;
	cvsrc[ 1 ].y = srcPoints[ 1 ].y;
	cvsrc[ 2 ].x = srcPoints[ 2 ].x;
	cvsrc[ 2 ].y = srcPoints[ 2 ].y;
	cvsrc[ 3 ].x = srcPoints[ 3 ].x;
	cvsrc[ 3 ].y = srcPoints[ 3 ].y;
    
	cvdst[ 0 ].x = dstPoints[ 0 ].x;
	cvdst[ 0 ].y = dstPoints[ 0 ].y;
	cvdst[ 1 ].x = dstPoints[ 1 ].x;
	cvdst[ 1 ].y = dstPoints[ 1 ].y;
	cvdst[ 2 ].x = dstPoints[ 2 ].x;
	cvdst[ 2 ].y = dstPoints[ 2 ].y;
	cvdst[ 3 ].x = dstPoints[ 3 ].x;
	cvdst[ 3 ].y = dstPoints[ 3 ].y;
    
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
	
	CvMat* src_mat = cvCreateMat( 4, 2, CV_32FC1 );
	CvMat* dst_mat = cvCreateMat( 4, 2, CV_32FC1 );
    
	//copy our points into the matrixes
	cvSetData( src_mat, cvsrc, sizeof(CvPoint2D32f));
	cvSetData( dst_mat, cvdst, sizeof(CvPoint2D32f));
    
	//figure out the warping!
	//warning - older versions of openCV had a bug
	//in this function.
	cvFindHomography( src_mat, dst_mat, translate );
    
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
	matrixTemp.getPtr()[ 0 ]    = mat[ 0 ];
	matrixTemp.getPtr()[ 4 ]    = mat[ 1 ];
	matrixTemp.getPtr()[ 12 ]	= mat[ 2 ];
	
	matrixTemp.getPtr()[ 1 ]    = mat[ 3 ];
	matrixTemp.getPtr()[ 5 ]    = mat[ 4 ];
	matrixTemp.getPtr()[ 13 ]   = mat[ 5 ];	
	
	matrixTemp.getPtr()[ 3 ]    = mat[ 6 ];
	matrixTemp.getPtr()[ 7 ]    = mat[ 7 ];
	matrixTemp.getPtr()[ 15 ]   = mat[ 8 ];

    return matrixTemp;
}

void ofxQuadWarp :: update ()
{
    //
}

void ofxQuadWarp :: reset()
{
	dstPoints[ 0 ] = srcPoints[ 0 ];
	dstPoints[ 1 ] = srcPoints[ 1 ];
	dstPoints[ 2 ] = srcPoints[ 2 ];
	dstPoints[ 3 ] = srcPoints[ 3 ];
    
	for( int i=0; i<4; i++ )
	{
		anchors[ i ].setPosAndSize
		(
            dstPoints[ i ].x - anchorSize * 0.5,
            dstPoints[ i ].y - anchorSize * 0.5,
            anchorSize,
            anchorSize
        );
		anchors[ i ].enableMouseEvents();
	}
}

void ofxQuadWarp :: onMouseDragged( ofMouseEventArgs& mouseArgs )
{
	for( int i=0; i<4; i++ )
	{
		if( anchors[ i ].isMouseDown() )
		{
			dstPoints[ i ].x = mouseArgs.x;
            dstPoints[ i ].y = mouseArgs.y;
            
			anchors[ i ].setPos
			(
                mouseArgs.x - anchorSize * 0.5,
                mouseArgs.y - anchorSize * 0.5
            );
		}
	}
}

//----------------------------------------------------- corners.
void ofxQuadWarp :: setCorners ( ofPoint* corners )
{
    setTopLeftCornerPosition( corners[ 0 ] );
    setTopRightCornerPosition( corners[ 1 ] );
    setBottomRightCornerPosition( corners[ 2 ] );
    setBottomLeftCornerPosition( corners[ 3 ] );
}

void ofxQuadWarp :: setCorner( ofPoint p, int cornerIndex )
{
    cornerIndex = ofClamp( cornerIndex, 0, 3 );
    dstPoints[ cornerIndex ] = p;
    anchors[ cornerIndex ].setPos( p.x - anchorSize * 0.5, p.y - anchorSize * 0.5 );
}

void ofxQuadWarp :: setTopLeftCornerPosition ( ofPoint p )
{
    setCorner( p, 0 );
}

void ofxQuadWarp :: setTopRightCornerPosition ( ofPoint p )
{
    setCorner( p, 1 );
}

void ofxQuadWarp :: setBottomRightCornerPosition ( ofPoint p )
{
    setCorner( p, 2 );
}

void ofxQuadWarp :: setBottomLeftCornerPosition ( ofPoint p )
{
    setCorner( p, 3 );
}

//----------------------------------------------------- show / hide.
void ofxQuadWarp :: show ()
{
    if( bShow )
        return;
    
    ofAddListener( ofEvents.mouseDragged, this, &ofxQuadWarp :: onMouseDragged );
    
    bShow = true;
}

void ofxQuadWarp :: hide ()
{
    if( !bShow )
        return;
    
    ofRemoveListener( ofEvents.mouseDragged, this, &ofxQuadWarp :: onMouseDragged );
    
    bShow = false;
}

void ofxQuadWarp :: toggleShow ()
{
    if( bShow )
        hide();
    else
        show();
}

void ofxQuadWarp :: draw ()
{
    if( !bShow )
        return;
    
    ofFill();
    
	for( int i=0; i<4; i++ )
	{
        int s = anchorSize;
        ofRect( dstPoints[ i ].x - s * 0.5, dstPoints[ i ].y - s * 0.5, s, s );
	}
	
	ofLine( dstPoints[ 0 ].x, dstPoints[ 0 ].y, dstPoints[ 1 ].x, dstPoints[ 1 ].y );
	ofLine( dstPoints[ 1 ].x, dstPoints[ 1 ].y, dstPoints[ 2 ].x, dstPoints[ 2 ].y );
	ofLine( dstPoints[ 2 ].x, dstPoints[ 2 ].y, dstPoints[ 3 ].x, dstPoints[ 3 ].y );
	ofLine( dstPoints[ 3 ].x, dstPoints[ 3 ].y, dstPoints[ 0 ].x, dstPoints[ 0 ].y );
}