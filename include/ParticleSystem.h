#pragma once
#include "Boid.h"
#include "Predator.h"
#include "cinder/Perlin.h"
#include <list>

class ParticleSystem {
 public:
	ParticleSystem();
	void applyForceToBoids( float zoneRadius, float lowerThresh, float higherThresh, float attractStrength, float repelStrength, float orientStrength );
	void applyForceToPredators( float zoneRadius, float lowerThresh, float higherThresh );
	void pullToCenter( const ci::Vec3f &center );
	void update( );
	void draw();
	void addBoids( int amt );
	void addPredators( int amt );
	void removeBoids( int amt );
	ci::Vec3f getPos();
	
	ci::Perlin mPerlin;
	
	std::vector<Boid>	mBoids;
	std::vector<Predator> mPredators;
	ci::Vec3f mParticleCentroid;
	int mNumParticles;
    
    
};