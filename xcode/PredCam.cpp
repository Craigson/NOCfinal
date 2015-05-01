//
//  PredCam.cpp
//  Flocking3D
//
//  Created by Craig Pickard on 4/30/15.
//
//

#include "PredCam.h"

using namespace ci;

//PredCam::PredCam(){}
PredCam::PredCam(float td){
    
    mTargetDistance = td;
    mEye			= Vec3f::zero();
    mCenter			= Vec3f::zero();
    mUp				= Vec3f::yAxis();
    mTarget         = Vec3f::zero();
    mPredCam.setPerspective( 75.0f, cinder::app::getWindowAspectRatio(), 5.0f, 5000.0f );
}

void PredCam::update(Vec3f const &vel, Vec3f const &loc){
    
    gl::setMatrices( mPredCam );
    mTarget = vel;
    mTarget.safeNormalize();
    mTarget * mTargetDistance;
    mEye = loc;
    mPredCam.lookAt( mEye, mTarget, mUp );
    //std::cout << mEye << std::endl;
}



