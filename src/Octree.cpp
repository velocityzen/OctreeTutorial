#include "Octree.h"

//Now, our .cpp file!
Octree::Octree(Vec3f p1, Vec3f p2, int depth){
	backLowerCorner = p1;
	frontUpperCorner = p2;

	center = Vec3f((p1.x + p2.x)/2, (p1.y + p2.y)/2, (p1.z + p2.z)/2);

	this->depth = depth;
	numberOfObjects = 0;
	hasChildren = false;
}

Octree::~Octree(){
	if (hasChildren){
		destroyChildren();
	}
}

//Public methods
void Octree::add(Particle* object){
	numberOfObjects++;
	if (!hasChildren && depth < MAX_OCTREE_DEPTH && numberOfObjects > MAX_PER_OCTREE) {
		spawnPartitions();
	}

	if (hasChildren) {
		storeObject(object, object->mLoc, true);
	}
	else {
		collidableObjects.insert(object);
	}
}

void Octree::remove(Particle* object, Vec3f mLoc){
	numberOfObjects--;

	if (hasChildren && numberOfObjects < MIN_PER_OCTREE) {
		destroyChildren();
	}

	if (hasChildren) {
		storeObject(object, mLoc, false);
	}
	else {
		collidableObjects.erase(object);
	}
}

void Octree::objectMoved(Particle* object, Vec3f oldmLoc){
	remove(object, oldmLoc);
	add(object);
}


//Private methods
void Octree::storeObject(Particle* object, Vec3f mLoc, bool addObject){
	for(int x = 0; x < 2; x++) {
		if (x == 0) {
			if (mLoc.x - object->mRadius > center.x) {
				continue;
			}
		}
		else if (mLoc.x + object->mRadius < center.x) {
			continue;
		}

		for(int y = 0; y < 2; y++) {
			if (y == 0) {
				if (mLoc.y - object->mRadius > center.y) {
					continue;
				}
			}
			else if (mLoc.y + object->mRadius < center.y) {
				continue;
			}

			for(int z = 0; z < 2; z++) {
				if (z == 0) {
					if (mLoc.z - object->mRadius > center.z) {
						continue;
					}
				}
				else if (mLoc.z + object->mRadius < center.z) {
					continue;
				}

				//Add or remove the object
				if (addObject) {
					children[x][y][z]->add(object);
				}
				else {
					children[x][y][z]->remove(object, mLoc);
				}
			}
		}
	}
}

void Octree::spawnPartitions(){
	for(int x = 0; x < 2; x++) {
		float minX;
		float maxX;
		if (x == 0) {
			minX = backLowerCorner[0];
			maxX = center[0];
		}
		else {
			minX = center[0];
			maxX = frontUpperCorner[0];
		}

		for(int y = 0; y < 2; y++) {
			float minY;
			float maxY;
			if (y == 0) {
				minY = backLowerCorner[1];
				maxY = center[1];
			}
			else {
				minY = center[1];
				maxY = frontUpperCorner[1];
			}

			for(int z = 0; z < 2; z++) {
				float minZ;
				float maxZ;
				if (z == 0) {
					minZ = backLowerCorner[2];
					maxZ = center[2];
				}
				else {
					minZ = center[2];
					maxZ = frontUpperCorner[2];
				}

				children[x][y][z] = new Octree(Vec3f(minX, minY, minZ),
											   Vec3f(maxX, maxY, maxZ),
											   depth + 1);
			}
		}
	}

	//Remove all Particles from "Particles" and add them to the new children
	for(set<Particle*>::iterator it = collidableObjects.begin(); it != collidableObjects.end();
			it++) {
		Particle* object = *it;
		storeObject(object, object->mLoc, true);
	}
	collidableObjects.clear();

	hasChildren = true;
}

void Octree::destroyChildren(){
	//Move all particles in descendants of this to the "CollidableObject" set
	collectParticles(collidableObjects);

	for(int x = 0; x < 2; x++) {
		for(int y = 0; y < 2; y++) {
			for(int z = 0; z < 2; z++) {
				delete children[x][y][z];
			}
		}
	}

	hasChildren = false;
}

//Takes particles in children of this partition, and places into the particleSet. Used for reducing
//the depth of the tree.
void Octree::collectParticles(set<Particle*> &particleSet){
	if (hasChildren){
		for(int x = 0; x < 2; x++){
			for(int y = 0; y < 2; y++){
				for(int z = 0; z < 2; z++){
					children[x][y][z]->collectParticles(particleSet);
				}
			}
		}
	}
	else{
		for(set<Particle*>::iterator it = collidableObjects.begin(); it != collidableObjects.end();	it++){
			Particle* object = *it;
			particleSet.insert(object);
		}
	}
}

//Collision detection

void Octree::checkCollisions(){
	//check particle collision
	vector<ParticlePair> pair;
	potentialPairs(pair);
	for(unsigned int i = 0; i < pair.size(); i++) {
		ParticlePair bp = pair[i];

		Particle* p1 = bp.o1;
		Particle* p2 = bp.o2;
		if (testParticleCollision(p1, p2)) {
			Vec3f displacement = (p1->mLoc - p2->mLoc);
			displacement.normalize();
			p1->mVel -= 2 * displacement * p1->mVel.dot(displacement);
			p2->mVel -= 2 * displacement * p2->mVel.dot(displacement);

			//change colours of particles
			if(p1->g == 1){
				p1->g = 0; p1->b = 0;
			}else{
				p1->g = 1; p1->b = 1;
			}

			if(p2->g == 1){
				p2->g = 0; p2->b = 0;
			}else{
				p2->g = 1; p2->b = 1;
			}
		}
	}

	//check wall collision
	vector<ParticleWallPair> particleWall;
	potentialParticleWallCollisions(particleWall);
	for(unsigned int i = 0; i < particleWall.size(); i++) {
		ParticleWallPair particleWallInstance = particleWall[i];

		Particle* p = particleWallInstance.p;
		Wall w = particleWallInstance.wall;
		if (testParticleWallCollision(p, w)) {
			//reflect
			Vec3f dir = wallDirection(w);//.normalize();
			dir.normalize();
			p->mVel -= 2 * dir * p->mVel.dot(dir);
		}
	}
}

//Particle/Particle
bool Octree::testParticleCollision(Particle* p1, Particle* p2){
	float r = p1->mRadius + p2->mRadius;
	Vec3f displacement = p1->mLoc - p2->mLoc;
	if (displacement.length() < r) {
		Vec3f netVelocity = p1->mVel - p2->mVel;
		Vec3f displacement = p1->mLoc - p2->mLoc;
		return netVelocity.dot(displacement) < 0;
	}
	else
		return false;
}


void Octree::potentialPairs(vector<ParticlePair> &collisions) {
	if (hasChildren) {
		for(int x = 0; x < 2; x++) {
			for(int y = 0; y < 2; y++) {
				for(int z = 0; z < 2; z++) {
					children[x][y][z]->potentialPairs(collisions);
				}
			}
		}
	}
	else {
		for(set<Particle*>::iterator it = collidableObjects.begin(); it != collidableObjects.end(); it++) {
			Particle *p1 = *it;
			for(set<Particle*>::iterator it2 = collidableObjects.begin();
					it2 != collidableObjects.end(); it2++) {
				Particle *p2 = *it2;
				//This test makes sure that we only add each pair once
				if (p1 < p2) {
					ParticlePair bp;
					bp.o1 = p1;
					bp.o2 = p2;
					collisions.push_back(bp);
				}
			}
		}
	}
}

//Particle/Wall

//checks if it's in direction of a wall and whether it has past it
bool Octree::testParticleWallCollision(Particle* p, Wall wall)
{
	Vec3f dir = wallDirection(wall);
	return (p->mLoc.dot(dir) + p->mRadius) > BOX_SIZE / 2 && (p->mVel.dot(dir) > 0);
}

void Octree::potentialParticleWallCollisions(vector<ParticleWallPair> &cs, Wall w, char coord, int dir) {
	if (hasChildren) {
		//Checks particle with associated wall
		for(int dir2 = 0; dir2 < 2; dir2++) {
			for(int dir3 = 0; dir3 < 2; dir3++) {
				Octree *child;
				switch (coord) {
					case 'x':
						child = children[dir][dir2][dir3];
						break;
					case 'y':
						child = children[dir2][dir][dir3];
						break;
					case 'z':
						child = children[dir2][dir3][dir];
						break;
				}

				child->potentialParticleWallCollisions(cs, w, coord, dir);
			}
		}
	}
	else {
		for(set<Particle*>::iterator it = collidableObjects.begin(); it != collidableObjects.end(); it++) {
			Particle *particle = *it;
			ParticleWallPair bwp;
			bwp.p = particle;
			bwp.wall = w;
			cs.push_back(bwp);
		}
	}
}


void Octree::potentialParticleWallCollisions(vector<ParticleWallPair> &collisions)
{
	potentialParticleWallCollisions(collisions, WALL_LEFT, 'x', 0);
	potentialParticleWallCollisions(collisions, WALL_RIGHT, 'x', 1);
	potentialParticleWallCollisions(collisions, WALL_BOTTOM, 'y', 0);
	potentialParticleWallCollisions(collisions, WALL_TOP, 'y', 1);
	potentialParticleWallCollisions(collisions, WALL_FAR, 'z', 0);
	potentialParticleWallCollisions(collisions, WALL_NEAR, 'z', 1);
}

//Returns the direction from the origin to the wall
Vec3f Octree::wallDirection(Wall wall){
	Vec3f w (0, 0, 0);

	switch (wall)
	{
	case WALL_LEFT:
		w.x = -1;
		break;
	case WALL_RIGHT:
		w.x = 1;
		break;
	case WALL_FAR:
		w.z = -1;
		break;
	case WALL_NEAR:
		w.z = 1;
		break;
	case WALL_TOP:
		w.y = 1;
		break;
	case WALL_BOTTOM:
		w.y = -1;
		break;
	default:
		break;
	}
	return w;
}

//Final display: optional
void Octree::displayOctree(Octree* currentOctree){
	for(unsigned int j = 0; j < 2; j++){
		for(unsigned int k = 0; k < 2; k++){
			for(unsigned int l = 0; l <2; l++){
				if(currentOctree->hasChildren == true){
					displayOctree(currentOctree->children[j][k][l]);
				}
				glPushMatrix();
				glColor3f(0, 1, 0);
                gl::drawStrokedCube(currentOctree->center, currentOctree->backLowerCorner - currentOctree->frontUpperCorner);
				glPopMatrix();
			}
		}
	}
}

