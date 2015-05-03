#include "Boid.h"
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"
#include "cinder/app/AppBasic.h"

using namespace ci;
using std::vector;

Boid::Boid()
{
}

Boid::Boid( Vec3f pos, Vec3f vel, bool followed, float radius )
{
	mPos			= pos;
	mTailPos		= pos;
	mVel			= vel;
	mVelNormal		= Vec3f::yAxis();
	mAcc			= Vec3f::zero();
	
	mNeighborPos	= Vec3f::zero();
	mNumNeighbors	= 0;
	mMaxSpeed		= Rand::randFloat( 2.5f, 7.0f );
	mMaxSpeedSqrd	= mMaxSpeed * mMaxSpeed;
	mMinSpeed		= Rand::randFloat( 1.0f, 1.5f );
	mMinSpeedSqrd	= mMinSpeed * mMinSpeed;
	
	mColor			= ColorA( 1.0f, 1.0f, 1.0f, 1.0f );
		
	mDecay			= 0.99f;
	//mRadius			= 1.0f;
	mLength			= 5.0f;
	mFear			= 1.0f;
	mCrowdFactor	= 1.0f;
	
	mIsDead			= false;
	mFollowed		= followed;
    
    mRadius         = radius;
    
    tailLength       =  40;
    
    for(int i = 0; i < tailLength; i++){
        locs.push_back(Vec3f::zero());
    }
}

void Boid::pullToCenter( const Vec3f &center )
{
	Vec3f dirToCenter = mPos - center;
	float distToCenter = dirToCenter.length();
	float distThresh = 600.0f;
	
	if( distToCenter > distThresh ){
		dirToCenter.normalize();
		float pullStrength = 0.00025f;
		mVel -= dirToCenter * ( ( distToCenter - distThresh ) * pullStrength );
	}
}


void Boid::update( )
{	
	mCrowdFactor -= ( mCrowdFactor - ( 1.0f - mNumNeighbors * 0.02f ) ) * 0.1f;
	mCrowdFactor = constrain( mCrowdFactor, 0.5f, 1.0f );
	
	mFear -= ( mFear - 0.0f ) * 0.2f;
	

	
	mVel += mAcc;
	mVelNormal = mVel.normalized();
	
	limitSpeed();
	
	
	mPos += mVel;
	
	mTailPos = mPos - ( mVelNormal * mLength );
	mVel *= mDecay;
	
	float c = math<float>::min( mNumNeighbors/50.0f, 1.0f );
	mColor = ColorA( CM_HSV, 1.0f - c, c, c * 0.5f + 0.5f, 1.0f );
	
	mAcc = Vec3f::zero();
	mNeighborPos = Vec3f::zero();
	mNumNeighbors = 0;
    
    //update my trail array
    for(int arrayIndex = (tailLength -1); arrayIndex >= 0; arrayIndex--){
        if(arrayIndex != 0 ){
            locs[arrayIndex] = locs[arrayIndex - 1];
        } else if (arrayIndex == 0){
            locs[arrayIndex] = mPos;
        }
    }
}

void Boid::limitSpeed()
{
	float maxSpeed = mMaxSpeed + mCrowdFactor;
	float maxSpeedSqrd = maxSpeed * maxSpeed;
	
	float vLengthSqrd = mVel.lengthSquared();
	if( vLengthSqrd > maxSpeedSqrd ){
		mVel = mVelNormal * maxSpeed;
		
	} else if( vLengthSqrd < mMinSpeedSqrd ){
		mVel = mVelNormal * mMinSpeed;
	}
	mVel *= (1.0 + mFear );
}

void Boid::draw()
{
	glColor4f( mColor );
   // drawTail();
	
}

void Boid::drawTail()
{
    
        glBegin(GL_LINE_STRIP);
    {
        for(int i = 1; i < tailLength; i++){
            float alpha = 0.8 - 0.05 * i;
            gl::color(ColorAf(1., 1., 1., alpha ) );
            gl::vertex(locs[i]);
        }
    }
    glEnd();
}

void Boid::addNeighborPos( Vec3f pos )
{
	mNeighborPos += pos;
	mNumNeighbors ++;
}


