#include "cinder/app/AppBasic.h"
#include "cinder/Rand.h"
#include "cinder/Vector.h"
#include "ParticleSystem.h"

using namespace ci;
using std::list;

ParticleSystem::ParticleSystem()
{
	mPerlin = Perlin( 4 );
}

void ParticleSystem::applyForceToBoids( float zoneRadius, float lowerThresh, float higherThresh, float attractStrength, float repelStrength, float alignStrength  )
{
	float twoPI = M_PI * 2.0f;
	mParticleCentroid = Vec3f::zero();
	mNumParticles = mBoids.size();
	
	for(  std::vector<Boid>::iterator p1 = mBoids.begin(); p1 != mBoids.end(); ++p1 ){
		
		std::vector<Boid>::iterator p2 = p1;
		for( ++p2; p2 != mBoids.end(); ++p2 ) {
			Vec3f dir = p1->mPos - p2->mPos;
			float distSqrd = dir.lengthSquared();
			float zoneRadiusSqrd = zoneRadius * p1->mCrowdFactor * zoneRadius * p2->mCrowdFactor;
			
			if( distSqrd < zoneRadiusSqrd ){		// Neighbor is in the zone
				float per = distSqrd/zoneRadiusSqrd;
				p1->addNeighborPos( p2->mPos );
				p2->addNeighborPos( p1->mPos );
					
				if( per < lowerThresh ){			// Separation
					float F = ( lowerThresh/per - 1.0f ) * repelStrength;
					dir.normalize();
					dir *= F;
			
					p1->mAcc += dir;
					p2->mAcc -= dir;
				} else if( per < higherThresh ){	// Alignment
					float threshDelta	= higherThresh - lowerThresh;
					float adjPer		= ( per - lowerThresh )/threshDelta;
					float F				= ( 1.0f - ( cos( adjPer * twoPI ) * -0.5f + 0.5f ) ) * alignStrength;
			
					p1->mAcc += p2->mVelNormal * F;
					p2->mAcc += p1->mVelNormal * F;
					
				} else {							// Cohesion (prep)
					float threshDelta	= 1.0f - higherThresh;
					float adjPer		= ( per - higherThresh )/threshDelta;
					float F				= ( 1.0f - ( cos( adjPer * twoPI ) * -0.5f + 0.5f ) ) * attractStrength;
										
					dir.normalize();
					dir *= F;
			
					p1->mAcc -= dir;
					p2->mAcc += dir;
				}
			}
		}
		
		mParticleCentroid += p1->mPos;

        
		// ADD PERLIN NOISE INFLUENCE
		float scale = 0.002f;
		float multi = 0.01f;
		Vec3f perlin = mPerlin.dfBm( p1->mPos * scale ) * multi;
		p1->mAcc += perlin;
		
		
		// CHECK WHETHER THERE IS ANY BOID/PREDATOR INTERACTION
		float eatDistSqrd = 50.0f;
		float predatorZoneRadiusSqrd = zoneRadius * zoneRadius * 5.0f;
		for( std::vector<Predator>::iterator predator = mPredators.begin(); predator != mPredators.end(); ++predator ) {

			Vec3f dir = p1->mPos - predator->mPos[0];
			float distSqrd = dir.lengthSquared();
			
			if( distSqrd < predatorZoneRadiusSqrd ){
				if( distSqrd > eatDistSqrd ){
					float F = ( predatorZoneRadiusSqrd/distSqrd - 1.0f ) * 0.1f;
					p1->mFear += F * 0.1f;
					dir = dir.normalized() * F;
					p1->mAcc += dir;
					if( predator->mIsHungry )
						predator->mAcc += dir * 0.04f * predator->mHunger;
				} else {
					p1->mIsDead = true;
					predator->mHunger = 0.0f;
					predator->mIsHungry = false;
				}
			}
		}
		
	}
	mParticleCentroid /= (float)mNumParticles;
}


void ParticleSystem::applyForceToPredators( float zoneRadius, float lowerThresh, float higherThresh )
{
	float twoPI = M_PI * 2.0f;
	for( std::vector<Predator>::iterator P1 = mPredators.begin(); P1 != mPredators.end(); ++P1 ){
	
		std::vector<Predator>::iterator P2 = P1;
		for( ++P2; P2 != mPredators.end(); ++P2 ) {
			Vec3f dir = P1->mPos[0] - P2->mPos[0];
			float distSqrd = dir.lengthSquared();
			float zoneRadiusSqrd = zoneRadius * zoneRadius * 4.0f;
			
			if( distSqrd < zoneRadiusSqrd ){		// Neighbor is in the zone
				float per = distSqrd/zoneRadiusSqrd;
				if( per < lowerThresh ){			// Separation
					float F = ( lowerThresh/per - 1.0f ) * 0.01f;
					dir.normalize();
					dir *= F;
			
					P1->mAcc += dir;
					P2->mAcc -= dir;
				} else if( per < higherThresh ){	// Alignment
					float threshDelta	= higherThresh - lowerThresh;
					float adjPer		= ( per - lowerThresh )/threshDelta;
					float F				= ( 1.0f - cos( adjPer * twoPI ) * -0.5f + 0.5f ) * 0.3f;
			
					P1->mAcc += P2->mVelNormal * F;
					P2->mAcc += P1->mVelNormal * F;
					
				} else {							// Cohesion
					float threshDelta	= 1.0f - higherThresh;
					float adjPer		= ( per - higherThresh )/threshDelta;
					float F				= ( 1.0f - ( cos( adjPer * twoPI ) * -0.5f + 0.5f ) ) * 0.1f;
										
					dir.normalize();
					dir *= F;
			
					P1->mAcc -= dir;
					P2->mAcc += dir;
				}
			}
		}
	}
}


void ParticleSystem::pullToCenter( const ci::Vec3f &center )
{
	for( std::vector<Boid>::iterator p = mBoids.begin(); p != mBoids.end(); ++p ){
		p->pullToCenter( center );
	}
	
	for( std::vector<Predator>::iterator p = mPredators.begin(); p != mPredators.end(); ++p ){
		p->pullToCenter( center );
	}
}

void ParticleSystem::update()
{
	for( std::vector<Boid>::iterator p = mBoids.begin(); p != mBoids.end(); ){
		if( p->mIsDead ){
			p = mBoids.erase( p );
		} else {
			p->update();
			++p;
		}
	}
	
	for( std::vector<Predator>::iterator p = mPredators.begin(); p != mPredators.end(); ++p ){
		p->update();
	}
}

void ParticleSystem::draw()
{	
	// DRAW PREDATOR ARROWS
	for( std::vector<Predator>::iterator p = mPredators.begin(); p != mPredators.end(); ++p ){
		float hungerColor = 1.0f - p->mHunger;
		gl::color( ColorA( 1.0f, hungerColor, hungerColor, 1.0f ) );
		p->draw();
	}
	
	// DRAW PARTICLE ARROWS
	gl::color( ColorA( 1.0f, 1.0f, 1.0f, 1.0f ) );
	//glBegin( GL_LINES );
	for( std::vector<Boid>::iterator p = mBoids.begin(); p != mBoids.end(); ++p ){
		p->draw();
	}
	//glEnd();
}

void ParticleSystem::addPredators( int amt )
{
	for( int i=0; i<amt; i++ )
	{
		Vec3f pos = Rand::randVec3f() * Rand::randFloat( 500.0f, 750.0f );
		Vec3f vel = Rand::randVec3f();
		mPredators.push_back( Predator( pos, vel ) );
	}
}

void ParticleSystem::addBoids( int amt )
{
	for( int i=0; i<amt; i++ )
	{
		Vec3f pos = Rand::randVec3f() * Rand::randFloat( 100.0f, 200.0f );
		Vec3f vel = Rand::randVec3f();
        float radius = Rand::randFloat(1.0f,20.0f);
		
		bool followed = false;
		if( mBoids.size() == 0 ) followed = true;
		
		mBoids.push_back( Boid( pos, vel, followed, radius ) );
	}
}

void ParticleSystem::removeBoids( int amt )
{
	for( int i=0; i<amt; i++ )
	{
		mBoids.pop_back();
	}
}

Vec3f ParticleSystem::getPos()
{
	return mBoids.begin()->mPos;
}
