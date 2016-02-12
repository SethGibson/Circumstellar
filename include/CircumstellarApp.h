#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/Camera.h"
#include "cinder/CameraUi.h"
#include "cinder/gl/gl.h"
#include "cinder/Log.h"
#include "cinder/ObjLoader.h"
#include "cinder/params/Params.h"
#include "BlackHole.h"
#include "Dust.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class Circumstellar : public App 
{
public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void mouseDrag(MouseEvent event) override;
	void keyDown(KeyEvent event) override;
	void update() override;
	void draw() override;
	void cleanup() override;

private:
	void setupGUI();

	CS_Dust::BlackHoleRef	mBlackHole;
	CS_Dust::DustCloudRef	mDustCloud;

	CameraPersp		mCamera;
	CameraUi		mCtrl;

	bool			mDrawGUI;

	float			mMaxDist;

	float			mParamK1, mParamK2, mParamK3, mParamK4;
	float			mParamV1, mParamV2, mParamV3;
	float			mParamS1, mParamS2, mParamS3;

	Color			mParamD1a, mParamD1b,
					mParamD2a, mParamD2b,
					mParamD3a, mParamD3b;

	params::InterfaceGlRef	mGUI;
};