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

//definte constants
#define NUM_BOIDS 1500
#define NUM_PREDS 10

using namespace ci;
using namespace ci::app;

class FlockingApp : public AppBasic {
public:
    void prepareSettings( Settings *settings );
    void keyDown( KeyEvent event );
    void setup();
    void update();
    void draw();
    
    // declare GUI object
    params::InterfaceGlRef	mParams;
    
    // declare camera variables (these will also be used for the predator-cam view)
    CameraPersp			mCam;
    Quatf				mSceneRotation;
    Vec3f				mEye, mCenter, mUp;
    float				mCameraDistance;
    
    ParticleSystem	mParticleSystem;
    float				mZoneRadius;
    float				mLowerThresh, mHigherThresh;
    float				mAttractStrength, mRepelStrength, mOrientStrength;
    
    bool				mCentralGravity;
    bool				mPredatorCam;
    
    gl::Texture mTexture;
    int mNumBoids;
    Vec3f *mPositions;
    float *mRadiuses;
    gl::GlslProg mShader;
};


// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> S E T T I N G S <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


void FlockingApp::prepareSettings( Settings *settings )
{
    settings->setWindowSize( 875, 600 );
    settings->setFrameRate( 60.0f );
}


// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> S E T U P <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


void FlockingApp::setup()
{
    Rand::randomize();
    gl::clear(Color(0,0,0));
    
    mCenter			= Vec3f( getWindowWidth() * 0.5f, getWindowHeight() * 0.5f, 0.0f );
    mCentralGravity = true;
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
    mParticleSystem.addBoids( NUM_BOIDS );
    mParticleSystem.addPredators( NUM_PREDS );
    
    mTexture = loadImage( loadAsset( "particle.png" ) );
    mShader = gl::GlslProg( loadAsset( "shader.vert"), loadAsset( "shader.frag" ) );
    
    mPositions = new Vec3f[ NUM_BOIDS ];
    mRadiuses = new float[ NUM_BOIDS ];
    
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> K E Y D O W N <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

void FlockingApp::keyDown( KeyEvent event )
{

}


// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> U P D A T E <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

void FlockingApp::update()
{
    if( mLowerThresh > mHigherThresh ) mHigherThresh = mLowerThresh;
    
    mParticleSystem.applyForceToPredators( mZoneRadius, 0.4f, 0.7f );
    mParticleSystem.applyForceToBoids( mZoneRadius, mLowerThresh, mHigherThresh, mAttractStrength, mRepelStrength, mOrientStrength );
    if( mCentralGravity ) mParticleSystem.pullToCenter( mCenter );
    mParticleSystem.update();

    gl::rotate( mSceneRotation );
    mCam.lookAt( mEye, mCenter, mUp );
    
    gl::setMatrices( mCam );
    
    int mNumParticles = mParticleSystem.mBoids.size();
    
    mParticleSystem.update();
    for( int i=0; i<mNumParticles; i++ ){
        mPositions[i] = mParticleSystem.mBoids[i].mPos;
        mRadiuses[i] = mParticleSystem.mBoids[i].mRadius;
    }
    
    
}


// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> D R A W <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


void FlockingApp::draw()
{
    gl::clear( Color( 0, 0, 0 ), true );
  //  gl::enableDepthRead();
  //  gl::enableDepthWrite();
    
    gl::enableAlphaBlending();
    
   // gl::color( ColorA( 1.0f, 1.0f, 1.0f, 1.0f ) );
    
    mParticleSystem.draw();
    
    
    // draw gui
    mParams->draw();
    
    //draw central planet
    gl::color(Color(1.,1.,1.));
    gl::drawSphere(Vec3f(0.,0.,0.),30);
    
    //draw point sprite shader
    mShader.bind();
    
    GLint particleRadiusLocation = mShader.getAttribLocation( "particleRadius" );
    glEnableVertexAttribArray(particleRadiusLocation);
    glVertexAttribPointer(particleRadiusLocation, 1, GL_FLOAT, false, 0, mRadiuses);
    
    glEnable(GL_POINT_SPRITE);
    glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    
    glEnableClientState(GL_VERTEX_ARRAY);
    
    glVertexPointer(2, GL_FLOAT, 0, mPositions);
    
    mTexture.enableAndBind();
    glDrawArrays( GL_POINTS, 0, NUM_BOIDS );
    mTexture.unbind();
    
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableVertexAttribArrayARB(particleRadiusLocation);
    mShader.unbind();
    
}

CINDER_APP_BASIC( FlockingApp, RendererGl )
