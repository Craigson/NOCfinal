#include "Predator.h"
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"
#include "cinder/app/AppBasic.h"

using namespace ci;

Predator::Predator()
{
}

Predator::Predator( Vec3f pos, Vec3f vel )
{
	mLen			= 5;
	mInvLen			= 1.0f/(float)mLen;

	for( int i=0; i<mLen; ++i ) {
		mPos.push_back( pos );
	}
	
    //loc             = pos;
	mVel			= vel;
	mVelNormal		= Vec3f::yAxis();
	mAcc			= Vec3f::zero();
	
	mNeighborPos	= Vec3f::zero();
	mNumNeighbors	= 0;
	mMaxSpeed		= Rand::randFloat( 4.0f, 12.f );
	mMaxSpeedSqrd	= mMaxSpeed * mMaxSpeed;
	mMinSpeed		= Rand::randFloat( 1.0f, 5.f );
	mMinSpeedSqrd	= mMinSpeed * mMinSpeed;

	mColor			= ColorA( 1.0f, 0.0f, 0.0f, 1.0f );
	
	mDecay			= 0.99f;
	mRadius			= 2.0f;
	mLength			= 25.0f;
	mHunger			= 1.0f;
	
	mIsDead			= false;
	mIsHungry		= true;
    
    
    tailLength       = 200;
    
    for(int i = 0; i < tailLength; i++){
        tailPos.push_back(Vec3f::zero());
    }
    
    target          = Vec3f::zero();
   
}

void Predator::pullToCenter( const Vec3f &center )
{
	Vec3f dirToCenter	= mPos[0] - center;
	float distToCenter	= dirToCenter.length();
	float maxDistance	= 1000.0f;
	
	if( distToCenter > maxDistance ){
		float pullStrength = 0.001f;
		mVel -= dirToCenter.normalized() * ( ( distToCenter - maxDistance ) * pullStrength );
	}
}	

void Predator::update( )
{	
	mVel += mAcc;
	mVelNormal = mVel.safeNormalized();
   // mVel += mAcc;
    
	limitSpeed();
	
	
	for( int i=mLen-1; i>0; i-- ){
		mPos[i] = mPos[i-1];
	}
    mPos[0] += mVel;
	//loc += mVel;
	
	mVel *= mDecay;
	
	mAcc = Vec3f::zero();
	mNeighborPos = Vec3f::zero();
	mNumNeighbors = 0;
	
	mHunger += 0.001f;
	mHunger = constrain( mHunger, 0.0f, 1.0f );
	
	if( mHunger > 0.5f ) mIsHungry = true;
    
    
    //update my trail array
    for(int arrayIndex = (tailLength -1); arrayIndex >= 0; arrayIndex--){
        if(arrayIndex != 0 ){
            tailPos[arrayIndex] = tailPos[arrayIndex - 1];
        } else if (arrayIndex == 0){
            tailPos[arrayIndex] = mPos[0];
        }
    }
    
    target = mPos[0] + mVelNormal*100;
    
    //update target for predCam
    Vec3f tempVel = mVel;
    tempVel.safeNormalize();
    tempVel*50;
    Vec3f target = mPos[0] + tempVel;
}

void Predator::limitSpeed()
{
	float maxSpeed = mMaxSpeed + mHunger * 3.0f;
	float maxSpeedSqrd = maxSpeed * maxSpeed;
	float vLengthSqrd = mVel.lengthSquared();
	if( vLengthSqrd > maxSpeedSqrd ){
		mVel = mVelNormal * maxSpeed;
		
	} else if( vLengthSqrd < mMinSpeedSqrd ){
		mVel = mVelNormal * mMinSpeed;
	}
}

void Predator::draw()
{
	glColor4f( mColor );
    gl::drawSphere(mPos[0],3,8);
    drawTail();

}

void Predator::drawTail()
{
    
    glBegin(GL_LINE_STRIP);
    glPointSize( 5.0 );
    {
        for(int i = 1; i < tailLength; i++){
            float alpha = 1. - 0.005 * i;
            gl::color(ColorAf(1., 0., 0., alpha ) );
            gl::vertex(tailPos[i]);
        }
    }
    glEnd();
}

void Predator::addNeighborPos( Vec3f pos )
{
	mNeighborPos += pos;
	mNumNeighbors ++;
}


ci::Vec3f Predator::returnPos(){
    return mPos[0];
}

ci::Vec3f Predator::returnVel(){
    return mVel;
}

