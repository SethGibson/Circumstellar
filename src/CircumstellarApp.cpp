#include "CircumstellarApp.h"

void Circumstellar::setup()
{
	mMaxDist = 3.0f;
	mBlackHole = CS_Dust::BlackHole::create("meshes/MSH_CS.obj", "shaders/black_hole.vert", "shaders/black_hole.frag", "textures/TX_BlackHole.tif", this);

	mCamera.setPerspective(90.0f, getWindowAspectRatio(), 0.1f, 100.0f);
	mCamera.lookAt(vec3(0,0,mMaxDist), vec3(), vec3(0, 1, 0));

	mCtrl.setCamera(&mCamera);

	gl::enableDepth();

	
}

void Circumstellar::mouseDown( MouseEvent event )
{
	mCtrl.mouseDown(event);
}

void Circumstellar::mouseDrag(MouseEvent event)
{
	mCtrl.mouseDrag(event);
}

void Circumstellar::update()
{
}

void Circumstellar::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
	gl::setMatrices(mCamera);

	gl::color(Color::white());
	mBlackHole->Draw(static_cast<float>(getElapsedFrames()));
}

void prepareSettings(App::Settings *pSettings)
{
	pSettings->setWindowSize(1280, 720);
	pSettings->setFrameRate(60);
}

CINDER_APP( Circumstellar, RendererGl(RendererGl::Options().msaa(4)), prepareSettings )
