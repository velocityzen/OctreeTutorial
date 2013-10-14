#include "Particle.h"

using namespace ci;

Particle::Particle(){

}

Particle::Particle( Vec3f loc )
{
	mLoc	= loc;
	mVel	= Rand::randVec3f() * 0.1;

	mRadius	= 0.2f;

	r = 1;
	g = 1;
	b = 1;

	w = cinder::app::getWindowWidth();
	h = cinder::app::getWindowHeight();
}

Particle::Particle( Vec3f loc, Vec3f vel)
{
	mLoc	= loc;
	mVel	= vel;
	mRadius	= 6.0f;

	r = 1;
	g = 1;
	b = 1;

	w = cinder::app::getWindowWidth();
	h = cinder::app::getWindowHeight();
}

void Particle::update()
{
	mPrevLoc = mLoc;
	mLoc += mVel;
}

void Particle::draw()
{
	gl::color( Color(r, g, b) );
	gl::drawSphere( mLoc, mRadius );
	gl::color( Color(1, 1, 1) );
}

/*
Naive collision detection: checks every particles with every other particle
On collision, the direction is reflected

To advance collision:
	- Collision detection data structure, e.g quadtree
	- Mass associated with particles as a factor of velocity and energy transfer
*/
void Particle::checkCollision(Particle* p2){
	Vec3f p1_to_p2 = Vec3f(p2->mLoc.x - mLoc.x, p2->mLoc.y -  mLoc.y, p2->mLoc.z - mLoc.z);
	float distanceBetweenP1P2 = sqrt( (p1_to_p2.x*p1_to_p2.x) + (p1_to_p2.y*p1_to_p2.y) );
	float distanceOfTotalRadius = mRadius + p2->mRadius;

	if(distanceBetweenP1P2 < distanceOfTotalRadius){ //particles are close!
		//But are they moving into each other or away from each other?
		Vec3f netVelocity = mVel - p2->mVel;
		Vec3f displacement = (mLoc - p2->mLoc);

		if(netVelocity.dot(displacement) < 0){
			//Now collide!

			//Naive bounce
			/*mDir = -mDir;
			p2->mDir = -p2->mDir;*/

			//Better bounce, to make it more advanced we would add mass to the particles as a factor of velocity
			displacement.normalize();

			Vec3f properVelocity = mVel;
			Vec3f p2_properVelocity = p2->mVel;

			mVel -= 2 * displacement * properVelocity.dot(displacement);
			p2->mVel -= 2 * displacement * p2_properVelocity.dot(displacement);

			//Energy transfer:
			//On colliosion, faster balls get slower, slower balls get faster
			//Still naive, as there is no mass

			if(g == 1){
				g = 0; b = 0;
			}else{
				g = 1; b = 1;
			}

			if(p2->g == 1){
				p2->g = 0; p2->b = 0;
			}else{
				p2->g = 1; p2->b = 1;
			}

		}
	}
}

