#include "cinder/app/AppBasic.h"
#include "cinder/Rand.h"
#include "cinder/MayaCamUI.h"
#include "cinder/Arcball.h"
#include "Octree.h"
#include "ParticleController.h"

using namespace ci;
using namespace ci::app;
using namespace std;


class OctreeTutorial : public AppBasic {
 public:
	void keyDown( KeyEvent event );
	void mouseDown( MouseEvent event );
	void mouseDrag( MouseEvent event );

	void resize();
	void draw();
	void update();
	void setup();
	void cleanUp();

	void printFunction();

	Arcball						mArcball;
	MayaCamUI					mMayaCam;
	Octree*						octree;
    ParticleController mParticleController;
	int particlesCounter;

    bool drawBoxes;
};

void OctreeTutorial::setup()
{
    drawBoxes = true;

	gl::enableAlphaBlending();
	gl::enableDepthRead();
	gl::enableDepthWrite();
	glEnable( GL_CULL_FACE );
	glFrontFace( GL_CW );

	octree = NULL;

	Vec3f p1(-BOX_SIZE / 2, -BOX_SIZE / 2, -BOX_SIZE / 2);
	Vec3f p2(BOX_SIZE / 2, BOX_SIZE / 2, BOX_SIZE / 2);
	octree = new Octree(p1, p2, 1);

	mParticleController.setOctreeRoot(octree);
	particlesCounter = 0;

	CameraPersp initialCam;
	initialCam.setPerspective( 45.0f, getWindowAspectRatio(), 0.1, 10000 );
	mMayaCam.setCurrentCam( initialCam );
}

void OctreeTutorial::resize()
{
	mArcball.setWindowSize( getWindowSize() );
	mArcball.setCenter( Vec2f( getWindowWidth() / 2.0f, getWindowHeight() / 2.0f ) );
	mArcball.setRadius( 150 );
}

void OctreeTutorial::mouseDown( MouseEvent event )
{
	if( event.isAltDown() ){
		mMayaCam.mouseDown( event.getPos() );
	}
}

void OctreeTutorial::mouseDrag( MouseEvent event )
{
	if( event.isAltDown() ){
		mMayaCam.mouseDrag( event.getPos(), event.isLeftDown(), event.isMiddleDown(), event.isRightDown() );
	}
}

void OctreeTutorial::keyDown( KeyEvent event )
{

    switch (event.getChar()) {
        case 'p':
            if(octree != NULL){
                mParticleController.addParticles( particlesCounter, particlesCounter+10 );
                particlesCounter += 10;
            }
            break;

        case 'b':
            drawBoxes = !drawBoxes;
            break;

        case 'f':
            setFullScreen( ! isFullScreen() );
            break;

        case 27:
            if(isFullScreen()) {
                setFullScreen( ! isFullScreen());
            } else {
                cleanUp();
                quit();
            }
        default:
            break;
    }

}

void OctreeTutorial::update(){
	mParticleController.update();
	octree->checkCollisions();
}


void OctreeTutorial::draw()
{
	gl::enableDepthWrite();
	gl::enableDepthRead();
	gl::clear( Color( 0.0f, 0.1f, 0.2f ) );
	glDisable( GL_CULL_FACE );

    gl::pushMatrices();
	gl::setMatrices( mMayaCam.getCamera() );

	gl::rotate( mArcball.getQuat() );

    if(drawBoxes) {
        octree->displayOctree(octree);
    }

	glColor3f( 1, 1, 1 );
	mParticleController.draw();
    gl::popMatrices();

	printFunction();
}

void OctreeTutorial::cleanUp(){
	if(octree != NULL){
		delete octree;
	}
}

void OctreeTutorial::printFunction(){

	Font mFont = Font( "Helvetica", 20.0f );
	char buffer [500];
	sprintf (buffer, "Particles: %i, FPS: %f", particlesCounter, getAverageFps());
	gl::drawString( buffer, Vec2f( 10.0f, 10.0f ), Color::white(), mFont );
}

CINDER_APP_BASIC( OctreeTutorial, RendererGl )
