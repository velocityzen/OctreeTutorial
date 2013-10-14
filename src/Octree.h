#pragma once

#include <set>
#include "Particle.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace std;


class Octree{
	private:
		Octree(); //Default constructor

		//Every segment needs to know its boundaries,
		//To know the boundaries, keep track of two corners opposite each other in the cube
		Vec3f backLowerCorner;
		Vec3f frontUpperCorner;

		//It's also useful to know the center of the cube so when we subdivide the cube
		//into smaller partitions, we know which partition each object belongs to
		Vec3f center; //(backLowerCorner.x+frontUpperCorner.x)/2, same for y and z.

		//Your set of objects you're comparing with in each partition
		set<Particle*> collidableObjects;

		//Total number of objects inside partition and every child within partition; essentially all objects below it on the tree
		int numberOfObjects;

		//The depth of this node in the tree
		int depth;

	   //Now time for some methods
	   //First, we need to add objects to our octree; so this function basically goes through finding which cube partition this object belongs to based on its pasition
	   //We have a "addObject" boolean component to also make this method have a dual purpose of removing objects
	   void storeObject(Particle* o, Vec3f position, bool addObject);
	   //Now, when a partition has too many objects inside it, we need to split the partition into eight, and delegate each object into their new octree partition
	   void spawnPartitions();

	   void destroyChildren();

	//public methods to access octree stuff
	public:
		//This is our main constructor; give it the backLowerCorner for p1, frontUpperCorner for p2, and the depth of the tree, which starts at 1 typically
		Octree(Vec3f p1, Vec3f p2, int depth);

		//On cleanup, you'll want to clear out all of your children
		~Octree();

		//Now we have our children
		Octree* children[2][2][2];
		//[0][0][0] refers to minx,y,z portion of the cube, [0][0][1] refers to minx,y, maxz, and so on for each corner
		//This is where you separate your children into each partition
		//Here is a picture of a given partition in the octree to bring home the point
        //https://sites.google.com/site/mddn442/_/rsrc/1363230801206/research-topics/collision-detection/minmaxOctreeQuad.png

		//Helpful check to see if there are any objects in this partition at all
		bool hasChildren;

		//Now some public methods for you to deal with your data structure
		void checkCollisions();
		bool testParticleCollision(Particle*, Particle*);
		void add(Particle* object);
		void remove(Particle* object, Vec3f position);
		void objectMoved(Particle* object, Vec3f oldPosition);
		void collectParticles(set<Particle*> &bs);
		void potentialPairs(vector<ParticlePair> &collisions);
		void potentialParticleWallCollisions(vector<ParticleWallPair> &cs, Wall w, char coord, int dir);
		void potentialParticleWallCollisions(vector<ParticleWallPair> &collisions);
		bool testParticleWallCollision(Particle* p, Wall wall);
		Vec3f wallDirection(Wall wall);
		void displayOctree(Octree* currentOctree);
};
