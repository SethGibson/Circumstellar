#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/Camera.h"
#include "cinder/CameraUi.h"
#include "cinder/gl/gl.h"
#include "cinder/ObjLoader.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class Circumstellar : public App 
{
public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void mouseDrag(MouseEvent event) override;
	void update() override;
	void draw() override;

private:
	gl::VboMeshRef		mDiskMesh;
	gl::GlslProgRef		mDiskShader;
	gl::Texture2dRef	mDiskTex,
						mMaskTex;

	CameraPersp		mCamera;
	CameraUi		mCtrl;
};