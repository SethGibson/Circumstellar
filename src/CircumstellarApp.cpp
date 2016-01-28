#include "CircumstellarApp.h"

static size_t S_MAX_DUST = 5000;

void Circumstellar::setup()
{
	mMaxDist = 3.0f;
	mBlackHole = CS_Dust::BlackHole::create("meshes/MSH_CS.obj", "shaders/black_hole.vert", "shaders/black_hole.frag", "textures/TX_BlackHole.tif", this);
	mDustCloud = CS_Dust::DustCloud::create(S_MAX_DUST, mMaxDist, 1.0f, mCamera);
	
	mCamera.setPerspective(90.0f, getWindowAspectRatio(), 0.01f, 100.0f);
	mCamera.lookAt(vec3(0,0,mMaxDist), vec3(), vec3(0, 1, 0));

	//mCtrl.setCamera(&mCamera);
	gl::enableAdditiveBlending();
}

void Circumstellar::mouseDown( MouseEvent event )
{
	//mCtrl.mouseDown(event);
	mDustCloud->MouseSpawn(vec2(event.getPos()), vec2(getWindowSize()));
}

void Circumstellar::mouseDrag(MouseEvent event)
{
	//mCtrl.mouseDrag(event);
	mouseDown(event);
}

void Circumstellar::update()
{
	mDustCloud->Update();
}

void Circumstellar::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
	gl::setMatrices(mCamera);

	gl::color(Color::white());
	mDustCloud->Draw(mCamera);

	gl::pushModelMatrix();
	gl::scale(vec3(1.5));
	mBlackHole->Draw(static_cast<float>(getElapsedFrames()));
	gl::popModelMatrix();

}

void prepareSettings(App::Settings *pSettings)
{
	pSettings->setWindowSize(1280, 720);
	pSettings->setFrameRate(60);
}

CINDER_APP( Circumstellar, RendererGl(RendererGl::Options().msaa(4)), prepareSettings )
