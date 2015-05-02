#pragma once
#include "cinder/Vector.h"
#include "cinder/Color.h"
#include <vector>

using namespace ci;

class Boid {
 public:
	Boid();
	Boid( ci::Vec3f pos, ci::Vec3f vel, bool followed, float radius );
	void pullToCenter( const ci::Vec3f &center );
	void update( );
	void draw();
	void drawTail();
	void limitSpeed();
	void addNeighborPos( ci::Vec3f pos );

	ci::Vec3f	mPos;
	ci::Vec3f	mTailPos;
	ci::Vec3f	mVel;
	ci::Vec3f	mVelNormal;
	ci::Vec3f	mAcc;
	
	ci::Vec3f	mNeighborPos;
	int			mNumNeighbors;

	ci::Color	mColor;
		
	float		mDecay;
	float		mRadius;
	float		mLength;
	float		mMaxSpeed, mMaxSpeedSqrd;
	float		mMinSpeed, mMinSpeedSqrd;
	float		mFear;
	float		mCrowdFactor;
    float       radius;

	bool		mIsDead;
	bool		mFollowed;
    
    
    //variables for tail
    std::vector<Vec3f>       locs;
    int                 tailLength;

};