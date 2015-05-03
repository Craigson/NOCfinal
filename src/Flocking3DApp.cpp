//Final Project for Nature of Code at ITP, NYU
//April 2015
//Created by Craig Pickard
//Built on top of Robert Hodgin's 'Welcome to Cinder' Tutorial
//Special thanks to Mimi Yin and Dan Shiffman

#include "cinder/app/AppBasic.h"
#include "cinder/Vector.h"
#include "cinder/Utilities.h"
#include "cinder/ImageIO.h"
#include "cinder/params/Params.h"
#include "cinder/Camera.h"
#include "cinder/Rand.h"
#include "ParticleSystem.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Texture.h"
#include "cinder/MayaCamUI.h"

#include <sstream>

//definte constants
#define NUM_BOIDS 1000
#define NUM_PREDS 5

using namespace ci;
using namespace ci::app;

class FlockingApp : public AppBasic {
public:
    void prepareSettings( Settings *settings );
    void keyUp( KeyEvent event );
    void mouseDown (MouseEvent event);
    void mouseDrag (MouseEvent event);
    void addMore (int amt);
    void setup();
    void update();
    void draw();
    
    // declare GUI object
    params::InterfaceGlRef	mParams;
    
    // declare camera variables (these will also be used for the predator-cam view)
    CameraPersp         mSceneCam;
    MayaCamUI           mMayaCam;
   // Quatf				mSceneRotation;
    Vec3f				mEye, mCenter, mUp;
    float				mCameraDistance;
    
    ParticleSystem	    mParticleSystem;
    float				mZoneRadius;
    float				mLowerThresh, mHigherThresh;
    float				mAttractStrength, mRepelStrength, mOrientStrength;
    float               rotateY;
    
    bool				mCentralGravity;
    bool				mPredatorCam;
    bool                addEm;
    
    gl::Texture         mTexture;
    int                 mNumBoids;
    Vec3f               *mPositions;
    float               *mRadiuses;
    gl::GlslProg        mShader;
    
    int                 mCurrentFrame;
    Matrix44f           mTransform;
};


// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> S E T T I N G S <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


void FlockingApp::prepareSettings( Settings *settings )
{
    settings->setWindowSize( 1280, 720 );
    settings->setFrameRate( 30.0f );
}


// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> S E T U P <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


void FlockingApp::setup()
{
    Rand::randomize();
    gl::clear(Color(0,0,0));
    
    mCenter			= Vec3f( getWindowWidth() * 0.5f, getWindowHeight() * 0.5f, 0.0f );
    mCentralGravity = true;
    mZoneRadius		= 80.0f;
    mLowerThresh	= 0.3f;
    mHigherThresh	= 0.9f;
    mAttractStrength	= 0.004f;
    mRepelStrength		= 0.01f;
    mOrientStrength		= 0.01f;
    
    // initialise camera with values
    mCameraDistance = 1000.0f;
    mEye			= Vec3f( 0.0f, 0.0f, mCameraDistance );
    mCenter			= Vec3f::zero();
    mUp				= Vec3f::yAxis();
    
    /* setting for Cemera Matrix */
    mSceneCam.setPerspective(45.0f, getWindowAspectRatio(), 0.1, 10000);
    Vec3f mEye        = Vec3f( 0, 0, 3000 );
    Vec3f mCenter     = Vec3f(Vec3f(0, 0, 0));
    Vec3f mUp         = Vec3f::yAxis();
    mSceneCam.lookAt( mEye, mCenter, mUp );
    mSceneCam.setCenterOfInterestPoint(Vec3f(0, 0, 0));
    mMayaCam.setCurrentCam(mSceneCam);
    
    // create GUI control elements
    mParams = params::InterfaceGl::create( "Flocking", Vec2i( 200, 310 ) );
    mParams->addParam( "Center Gravity", &mCentralGravity);
    mParams->addSeparator();
    mParams->addParam( "Zone Radius", &mZoneRadius, "min=10.0 max=100.0 step=1.0" );
    mParams->addParam( "Lower Thresh", &mLowerThresh, "min=0.025 max=1.0 step=0.025");
    mParams->addParam( "Higher Thresh", &mHigherThresh, "min=0.025 max=1.0 step=0.025");
    mParams->addSeparator();
    mParams->addParam( "Atrraction", &mAttractStrength, "min=0.001 max=0.1 step=0.001 " );
    mParams->addParam( "Repulsion", &mRepelStrength, "min=0.001 max=0.1 step=0.001" );
    mParams->addParam( "Alignment", &mOrientStrength, "min=0.001 max=0.1 step=0.001" );
    
    //
    mParticleSystem.addBoids( NUM_BOIDS );
    mParticleSystem.addPredators( NUM_PREDS );
    
    //initialise variables used by the shader
    mTexture = loadImage( loadAsset( "particle.png" ) );
    mShader = gl::GlslProg( loadAsset( "shader.vert"), loadAsset( "shader.frag" ) );
    mPositions = new Vec3f[ NUM_BOIDS ];
    mRadiuses = new float[ NUM_BOIDS ];
    
    mCurrentFrame = 0;
    
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> K E Y D O W N <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

void FlockingApp::keyUp( KeyEvent event )
{
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> M O U S E D O W N <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
void FlockingApp::mouseDown(MouseEvent event){
    mMayaCam.mouseDown(event.getPos());
}


// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> M O U S E D R A G <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
void FlockingApp::mouseDrag(MouseEvent event){
    mMayaCam.mouseDrag(event.getPos(), event.isLeftDown(), event.isMiddleDown(), event.isRightDown());
}


// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> U P D A T E <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

void FlockingApp::update()
{
    if( mLowerThresh > mHigherThresh ) mHigherThresh = mLowerThresh;
    
    mParticleSystem.applyForceToPredators( mZoneRadius, 0.4f, 0.7f );
    mParticleSystem.applyForceToBoids( mZoneRadius, mLowerThresh, mHigherThresh, mAttractStrength, mRepelStrength, mOrientStrength );
    if( mCentralGravity ) mParticleSystem.pullToCenter( mCenter );
    mParticleSystem.update();
    
    gl::pushMatrices();
    {
    
    int mNumParticles = mParticleSystem.mBoids.size();
    
    mParticleSystem.update();
    for( int i=0; i<mNumParticles; i++ ){
        mPositions[i] = mParticleSystem.mBoids[i].mPos;
        mRadiuses[i] = mParticleSystem.mBoids[i].mRadius;
    }
    
    
    
    
    /*
     if (mCurrentFrame > 300){
    writeImage( getDocumentsDirectory() / "flocking_images2" / "saveImage_" / ( toString( mCurrentFrame ) + ".png" ), copyWindowSurface() );
    mCurrentFrame++;
     }
    
    if (mCurrentFrame > 1200){
        exit(0);
    }
     */
    }
 
    

}


// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> D R A W <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


void FlockingApp::draw()
{
    //apply camera matrices before drawing
    gl::setMatrices(mMayaCam.getCamera());
    
    /* draw my objects and make camera animation */
    gl::pushMatrices();
    {
        gl::translate(0, 0, 0);
        gl::pushMatrices();
        {
               gl::rotate(Vec3f(0, rotateY, 0));
    
    gl::clear( Color( 0, 0, 0 ), true );
    
    gl::enableAlphaBlending();
    gl::enableAdditiveBlending();
    
    //draw central planet
    gl::drawSphere(Vec3f(0.,0.,0.),8, 50);
    
    //draw axes
    gl::color(ColorA(0.,0.,1.,0.4));
    gl::drawLine(Vec3f(-1500.,0.,0.),Vec3f(1500.,0.,0.));
    gl::color(ColorA(1.,0.,0.,0.4));
    gl::drawLine(Vec3f(0.,-1500.,0.),Vec3f(0.,1500.,0.));
    gl::color(ColorA(0.,1.,0.,0.4));
    gl::drawLine(Vec3f(0.,0.,-1500.),Vec3f(0.,0.,1500.));
    
    
    gl::color( ColorA( 1.0f, 1.0f, 1.0f, 0.1f ) );
    
    mParticleSystem.draw();
    
    // draw gui
    //  mParams->draw();
    
    
    
    //draw point sprite shader
    mShader.bind();
    
    GLint particleRadiusLocation = mShader.getAttribLocation( "particleRadius" );
    glEnableVertexAttribArray(particleRadiusLocation);
    glVertexAttribPointer(particleRadiusLocation, 1, GL_FLOAT, false, 0, mRadiuses);
    
    glEnable(GL_POINT_SPRITE);
    glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    
    glEnableClientState(GL_VERTEX_ARRAY);
    
    glVertexPointer(3, GL_FLOAT, 0, mPositions);
    
    mTexture.enableAndBind();
    glDrawArrays( GL_POINTS, 0, NUM_BOIDS );
    mTexture.unbind();
    
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableVertexAttribArrayARB(particleRadiusLocation);
    mShader.unbind();
        }
    }
    gl::popMatrices();
rotateY -= 0.6;

}

void FlockingApp::addMore(int amt){
}
CINDER_APP_BASIC( FlockingApp, RendererGl )