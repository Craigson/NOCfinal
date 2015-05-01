#pragma once
#include "cinder/Vector.h"
#include "cinder/Color.h"
#include <vector>
using namespace ci;

class Predator {
 public:
	Predator();
	Predator( ci::Vec3f pos, ci::Vec3f vel );
	void pullToCenter( const ci::Vec3f &center );
	void update( );
	void limitSpeed();
	void draw();
	void drawTail();
	void addNeighborPos( ci::Vec3f pos );
    
    ci::Vec3f returnPos();
    ci::Vec3f returnVel();
	
	
	int			mLen;
	float		mInvLen;
	std::vector<ci::Vec3f> mPos;
	
    ci::Vec3f   loc;
	ci::Vec3f	mVel;
	ci::Vec3f	mVelNormal;
	ci::Vec3f	mAcc;
	
	ci::Color	mColor;
	
	ci::Vec3f	mNeighborPos;
	int			mNumNeighbors;
	
	float		mDecay;
	float		mRadius;
	float		mLength;
	float		mMaxSpeed, mMaxSpeedSqrd;
	float		mMinSpeed, mMinSpeedSqrd;
	float		mHunger;

	bool		mIsHungry;
	bool		mIsDead;
    
    //variables for tail
    std::vector<Vec3f>       tailPos;
    int                      tailLength;
    
    
};