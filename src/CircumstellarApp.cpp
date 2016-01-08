#include "CircumstellarApp.h"

void Circumstellar::setup()
{
	mDiskMesh = gl::VboMesh::create(ObjLoader(loadAsset("meshes/MSH_CS.obj")));
	mDiskTex = gl::Texture2d::create(loadImage(loadAsset("textures/TX_Dust.png")));
	mDiskTex->setWrap(GL_REPEAT, GL_REPEAT);
	mMaskTex = gl::Texture2d::create(loadImage(loadAsset("textures/TX_DustMask.png")));
	mDiskShader = gl::GlslProg::create(loadAsset("shaders/disk_dust.vert"), loadAsset("shaders/disk_dust.frag"));
	mDiskShader->uniform("u_samplerRGB", 0);
	mDiskShader->uniform("u_samplerMSK", 1);

	mCamera.setPerspective(90.0f, getWindowAspectRatio(), 0.1f, 100.0f);
	mCamera.lookAt(vec3(0,0,3), vec3(0,0,0), vec3(0, 1, 0));

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
	gl::drawColorCube(vec3(), vec3(1));

	//mDiskShader->bind();
	gl::ScopedGlslProg shader(mDiskShader);
	mDiskShader->uniform("TexOffset", getElapsedFrames()*0.001f);
	mDiskTex->bind(0);
	mMaskTex->bind(1);
	gl::draw(mDiskMesh);
	mMaskTex->unbind();
	mDiskTex->unbind();
}

void prepareSettings(App::Settings *pSettings)
{
	pSettings->setWindowSize(1280, 720);
	pSettings->setFrameRate(60);
}

CINDER_APP( Circumstellar, RendererGl(RendererGl::Options().msaa(4)), prepareSettings )
