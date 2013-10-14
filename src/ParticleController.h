#pragma once
#include <vector>
#include "Octree.h"
#include "Particle.h"
#include "define.h"

class ParticleController {
 public:
	ParticleController();
	~ParticleController();
	Octree* octree;
	void update();
	void draw();
	void addParticles( int begin, int amt );
	void removeParticles( int amt);

	void setOctreeRoot(Octree*);
	std::vector<Particle*>	mParticles;
};
