//Final Project for Nature of Code at ITP, NYU
//April 2015
//Created by Craig Pickard
//Based on Robert Hodgin's 'Welcome to Cinder' Tutorial
//Special thanks to Mimi Yin and Dan Shiffman

#include "cinder/app/AppBasic.h"
#include "cinder/Vector.h"
#include "cinder/Utilities.h"
#include "cinder/ImageIO.h"
#include "cinder/params/Params.h"
#include "cinder/Camera.h"
#include "cinder/Rand.h"
#include "ParticleController.h"
#include "cinder/gl/gl.h"
#include "PredCam.h"

//definte constants
#define NUM_INITIAL_PARTICLES 1500
#define NUM_INITIAL_PREDATORS 9
#define NUM_PARTICLES_TO_SPAWN 15

using namespace ci;
using namespace ci::app;

class FlockingApp : public AppBasic {
public:
    void prepareSettings( Settings *settings );
    void keyDown( KeyEvent event );
    void setup();
    void update();
    void draw();
    void followPred();
    
    // declare GUI object
    params::InterfaceGlRef	mParams;
    
    // declare camera variables (these will also be used for the predator-cam view)
    CameraPersp			mCam;
    Quatf				mSceneRotation;
    Vec3f				mEye, mCenter, mUp;
    float				mCameraDistance;
    
    ParticleController	mParticleController;
    PredCam*            predCam;
    float				mZoneRadius;
    float				mLowerThresh, mHigherThresh;
    float				mAttractStrength, mRepelStrength, mOrientStrength;
    
    bool				mCentralGravity;
    bool				mPredatorCam;
};

/*
 predator cam:
 >randomly select one of the predators
 >assign camera position to predator position
 >create a vector that points ahead of the predator
 >set the lookat point at this vector
 >update the position of the camera
 */








void FlockingApp::prepareSettings( Settings *settings )
{
    settings->setWindowSize( 875, 600 );
    settings->setFrameRate( 60.0f );
}

void FlockingApp::setup()
{
    Rand::randomize();
    gl::clear(Color(0,0,0));
    
    mCenter			= Vec3f( getWindowWidth() * 0.5f, getWindowHeight() * 0.5f, 0.0f );
    mCentralGravity = true;
    mPredatorCam    = false;
    mZoneRadius		= 80.0f;
    mLowerThresh	= 0.5f;
    mHigherThresh	= 0.8f;
    mAttractStrength	= 0.004f;
    mRepelStrength		= 0.01f;
    mOrientStrength		= 0.01f;
    
    // initialise camera with values
    mCameraDistance = 1000.0f;
    mEye			= Vec3f( 0.0f, 0.0f, mCameraDistance );
    mCenter			= Vec3f::zero();
    mUp				= Vec3f::yAxis();
    mCam.setPerspective( 75.0f, getWindowAspectRatio(), 5.0f, 5000.0f );
    
    // create GUI elements
    mParams = params::InterfaceGl::create( "Flocking", Vec2i( 200, 310 ) );
    mParams->addParam( "Scene Rotation", &mSceneRotation, "opened=1" );
    mParams->addSeparator();
    mParams->addParam( "Camera Distance", &mCameraDistance, "min=100.0 max=2000.0 step=50.0 keyIncr=s keyDecr=w" );
    mParams->addParam( "Center Gravity", &mCentralGravity, "keyIncr=g" );
    mParams->addParam( "Predator-Cam", &mPredatorCam);
    mParams->addSeparator();
    mParams->addParam( "Zone Radius", &mZoneRadius, "min=10.0 max=100.0 step=1.0 keyIncr=z keyDecr=Z" );
    mParams->addParam( "Lower Thresh", &mLowerThresh, "min=0.025 max=1.0 step=0.025 keyIncr=l keyDecr=L" );
    mParams->addParam( "Higher Thresh", &mHigherThresh, "min=0.025 max=1.0 step=0.025 keyIncr=h keyDecr=H" );
    mParams->addSeparator();
    mParams->addParam( "Atrraction", &mAttractStrength, "min=0.001 max=0.1 step=0.001 keyIncr=a keyDecr=A" );
    mParams->addParam( "Repulsion", &mRepelStrength, "min=0.001 max=0.1 step=0.001 keyIncr=r keyDecr=R" );
    mParams->addParam( "Alignment", &mOrientStrength, "min=0.001 max=0.1 step=0.001 keyIncr=o keyDecr=O" );
    
    //
    mParticleController.addParticles( NUM_INITIAL_PARTICLES );
    mParticleController.addPredators( NUM_INITIAL_PREDATORS );
    
    predCam = new PredCam(10.);
    
}

void FlockingApp::keyDown( KeyEvent event )
{
    if( event.getChar() == 'p' ){
        mParticleController.addParticles( NUM_PARTICLES_TO_SPAWN );
    }
}


void FlockingApp::update()
{
    if( mLowerThresh > mHigherThresh ) mHigherThresh = mLowerThresh;
    
    mParticleController.applyForceToPredators( mZoneRadius, 0.4f, 0.7f );
    mParticleController.applyForceToParticles( mZoneRadius, mLowerThresh, mHigherThresh, mAttractStrength, mRepelStrength, mOrientStrength );
    if( mCentralGravity ) mParticleController.pullToCenter( mCenter );
    mParticleController.update();
    
    /*
    if(something yo click){
    mEye	= Vec3f( 0.0f, 0.0f, mCameraDistance );
    } else {
        mEye = yourPredatorClass->returnLoc();
    }
    */
    

    gl::rotate( mSceneRotation );
    
    if (mPredatorCam){
        predCam->update(mParticleController.mPredators.front().mVel,mParticleController.mPredators.front().loc);
    } else {
        mCam.lookAt( mEye, mCenter, mUp );
        gl::setMatrices( mCam );
    }
}

void FlockingApp::draw()
{
    gl::clear( Color( 0, 0, 0 ), true );
    gl::enableDepthRead();
    gl::enableDepthWrite();
    
    gl::enableAlphaBlending();
    
    gl::color( ColorA( 1.0f, 1.0f, 1.0f, 1.0f ) );
    mParticleController.draw();
    
    // draw gui
    mParams->draw();
}

CINDER_APP_BASIC( FlockingApp, RendererGl )
