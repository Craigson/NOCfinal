//
//  PredCam.h
//  Flocking3D
//
//  Created by Craig Pickard on 4/30/15.
//
//

#ifndef __Flocking3D__PredCam__
#define __Flocking3D__PredCam__

#include <stdio.h>
#include "cinder/Camera.h"
#include "Predator.h"

using namespace ci;

class PredCam {
public:
    
    //PredCam();
    PredCam(float td);
    
    void update(Vec3f const &vel, Vec3f const &loc);
    
    CameraPersp			mPredCam;
    Quatf				mSceneRotation;
    Vec3f				mEye, mCenter, mUp;
    Vec3f               mTarget;
    Vec3f               mRefPoint;
    float				mTargetDistance;
    
};

#endif /* defined(__Flocking3D__PredCam__) */
