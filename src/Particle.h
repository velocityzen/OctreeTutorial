#pragma once
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"
#include "cinder/Vector.h"
#include "define.h"

using namespace cinder;

class Particle {
 public:
	Particle();
	Particle( Vec3f loc );
	Particle( Vec3f loc, Vec3f vel);
	void update();
	void draw();
	void checkCollision(Particle*);
	void checkWallCollision();
	bool checkWall(Vec3f, float);
	void calculateDirection(Vec3f);

	ci::Vec3f	mLoc;
	ci::Vec3f	mPrevLoc;
	ci::Vec3f	mVel;
	float		mRadius;
	float		r,g,b;

	int			w;
	int			h;
};

struct ParticlePair{
	Particle* o1;
	Particle* o2;
};

struct ParticleWallPair{
	Particle* p;
	Wall wall;
};
