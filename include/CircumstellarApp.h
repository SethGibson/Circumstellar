#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/Camera.h"
#include "cinder/CameraUi.h"
#include "cinder/gl/gl.h"
#include "cinder/Log.h"
#include "cinder/ObjLoader.h"

#include "pxcsensemanager.h"
#include "pxchandmodule.h"
#include "pxchandconfiguration.h"
#include "pxchanddata.h"

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
	void update() override;
	void draw() override;
	void cleanup() override;

private:
	void setupHand();
	void updateHand();

	CS_Dust::BlackHoleRef	mBlackHole;
	CS_Dust::DustCloudRef	mDustCloud;

	CameraPersp		mCamera;
	CameraUi		mCtrl;

	float			mMaxDist;

	PXCSenseManager	*mPXC;
	vec2			mHandDims;
};