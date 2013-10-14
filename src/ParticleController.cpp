#include "ParticleController.h"


ParticleController::ParticleController(){

}

ParticleController::~ParticleController(){
	for(std::vector<int>::size_type i = 0; i != mParticles.size(); i++) {
		delete mParticles[i];
	}
}

void ParticleController::update()
{
	for(std::vector<int>::size_type i = 0; i != mParticles.size(); i++) {
		mParticles[i]->update();
		octree->objectMoved(mParticles[i], mParticles[i]->mPrevLoc);
	}
}

void ParticleController::draw()
{
	for(std::vector<int>::size_type i = 0; i != mParticles.size(); i++) {
		mParticles[i]->draw();
	}
}

void ParticleController::addParticles( int begin, int amt )
{
	for( int i = begin; i<amt; i++ ){
		float x = (BOX_SIZE-2) * Rand::randFloat() - ((BOX_SIZE-2)/2);
		float y = (BOX_SIZE-2) * Rand::randFloat() - ((BOX_SIZE-2)/2);
		float z = (BOX_SIZE-2) * Rand::randFloat() - ((BOX_SIZE-2)/2);

		Particle* p = new Particle (Vec3f( x, y, z ) );
		mParticles.push_back(p);
		octree->add(p);
	}
}

void ParticleController::removeParticles( int amt )
{
	for( int i=0; i<amt; i++ ){
		mParticles.pop_back();
	}
}

void ParticleController::setOctreeRoot(Octree* o){
	octree = o;
}
