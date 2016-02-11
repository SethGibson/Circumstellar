#include "CircumstellarApp.h"

static size_t S_MAX_DUST = 5000;

void Circumstellar::setup()
{
	setupGUI();
	mDrawGUI = true;

	mMaxDist = 3.0f;
	mCamera.setPerspective(90.0f, getWindowAspectRatio(), 0.01f, 100.0f);
	mCamera.lookAt(vec3(0, 0, mMaxDist), vec3(), vec3(0, 1, 0));

	mBlackHole = CS_Dust::BlackHole::create();
	mDustCloud = CS_Dust::DustCloud::create(S_MAX_DUST, mMaxDist, 1.0f, mCamera);
	

	//mCtrl.setCamera(&mCamera);
	gl::enableAdditiveBlending();

	setupHand();
}

void Circumstellar::mouseDown( MouseEvent event )
{
	//mCtrl.mouseDown(event);
	mDustCloud->MouseSpawn(vec2(event.getPos()));
}

void Circumstellar::mouseDrag(MouseEvent event)
{
	//mCtrl.mouseDrag(event);
	mouseDown(event);
}

void Circumstellar::keyDown(KeyEvent event)
{
	auto pressed = event.getCode();

	switch(pressed)
	{
	case KeyEvent::KEY_d:
		mDrawGUI = !mDrawGUI;
		mDrawGUI ? mGUI->show() : mGUI->hide();
		break;
	case KeyEvent::KEY_f:
		setFullScreen(!isFullScreen());
		break;
	}
}

void Circumstellar::update()
{
	updateHand();
	mDustCloud->Update();

	vector<Color> colorList
	{
		mParamD1a,mParamD1b,
		mParamD2a,mParamD2b,
		mParamD3a,mParamD3b
	};
	mBlackHole->SetElapsed(static_cast<float>(getElapsedFrames()));
	mBlackHole->SetK(vec4(mParamK1, mParamK2, mParamK3, mParamK4));
	mBlackHole->SetV(vec3(mParamV1, mParamV2, mParamV3));
	mBlackHole->SetS(vec3(mParamS1, mParamS2, mParamS3));
	mBlackHole->SetColors(colorList);
}

void Circumstellar::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
	gl::setMatrices(mCamera);

	gl::color(Color::white());
	mDustCloud->Draw();

	gl::pushModelMatrix();
	gl::scale(vec3(1.5));
	mBlackHole->Draw();
	gl::popModelMatrix();

	if (mDrawGUI) {
		gl::setMatricesWindow(getWindowSize());
		gl::disableDepthRead();
		mGUI->draw();
	}
}

void Circumstellar::setupGUI()
{
	mGUI = params::InterfaceGl::create("Black Hole Params", ivec2(200, 400));

	mParamK1 = 0.25f;
	mParamK2 = 0.25f;
	mParamK3 = 0.75f;
	mParamK4 = 1.0f;

	mParamV1 = 1.0f;
	mParamV2 = 2.0f;
	mParamV3 = 3.f;

	mParamS1 = 4.0f;
	mParamS2 = 2.f;
	mParamS3 = 1.f;

	mParamD1a = Color(1, 0, 0);
	mParamD1b = Color(0, 1, 1);
	mParamD2a = Color(0, 1, 0);
	mParamD2b = Color(1, 0, 1);
	mParamD3a = Color(0, 0, 1);
	mParamD3b = Color(1, 1, 0);

	mGUI->addSeparator("Brightness");
	mGUI->addText("lBrightness", "label='Brightness'");
	mGUI->addParam("paramK1", &mParamK1, "label='Layer 1'", false);
	mGUI->addParam("paramK2", &mParamK2, "label='Layer 2'", false);
	mGUI->addParam("paramK3", &mParamK3, "label='Layer 3'", false);
	mGUI->addParam("paramK4", &mParamK4, "label='Global'", false);
	mGUI->addSeparator("Speed");
	mGUI->addText("lSpeed", "label='Speed'");
	mGUI->addParam("paramV1", &mParamV1, "label='Layer 1'", false);
	mGUI->addParam("paramV2", &mParamV2, "label='Layer 2'", false);
	mGUI->addParam("paramV3", &mParamV3, "label='Layer 3'", false);
	mGUI->addSeparator("Tiling");
	mGUI->addText("lTiling", "label='Tiling'");
	mGUI->addParam("paramS1", &mParamS1, "label='Layer 1'", false);
	mGUI->addParam("paramS2", &mParamS2, "label='Layer 2'", false);
	mGUI->addParam("paramS3", &mParamS3, "label='Layer 3'", false);
	mGUI->addSeparator("Colors");
	mGUI->addText("lColors", "label='Colors'");
	mGUI->addParam("paramD1a", &mParamD1a, "label='L1 Color1'", false);
	mGUI->addParam("paramD1b", &mParamD1b, "label='L1 Color2'", false);
	mGUI->addParam("paramD2a", &mParamD2a, "label='L2 Color1'", false);
	mGUI->addParam("paramD2b", &mParamD2b, "label='L2 Color2'", false);
	mGUI->addParam("paramD3a", &mParamD3a, "label='L3 Color1'", false);
	mGUI->addParam("paramD3b", &mParamD3b, "label='L3 Color2'", false);
}

void Circumstellar::cleanup()
{
	mPXC->Close();
}

void Circumstellar::setupHand()
{
	mPXC = PXCSenseManager::CreateInstance();
	if (mPXC)
	{
		mPXC->EnableHand();
		mPXC->Init();
		auto cfg = mPXC->QueryHand()->CreateActiveConfiguration();
		if (cfg)
		{
			cfg->SetTrackingMode(PXCHandData::TRACKING_MODE_CURSOR);
			cfg->SetSmoothingValue(1);
			cfg->ApplyChanges();
			cfg->Update();
			cfg->Release();
		}
		auto dd = mPXC->QueryCaptureManager()->QueryImageSize(PXCCapture::STREAM_TYPE_DEPTH);
		mHandDims.x = dd.width;
		mHandDims.y = dd.height;
	}
}

void Circumstellar::updateHand()
{
	if (mPXC->AcquireFrame(false) >= PXC_STATUS_NO_ERROR) {
		auto hand = mPXC->QueryHand()->CreateOutput();
		hand->Update();
		auto n = hand->QueryNumberOfHands();
		if (n > 0) {
			for (int i = 0; i < n; ++i) {
				PXCHandData::IHand *hd;
				if (hand->QueryHandData(PXCHandData::ACCESS_ORDER_NEAR_TO_FAR, i, hd) >= PXC_STATUS_NO_ERROR) {
					if (hd->HasCursor()) {
						PXCHandData::ICursor *hc;
						if (hd->QueryCursor(hc) >= PXC_STATUS_NO_ERROR) {
							auto pt = hc->QueryPointImage();
							if (pt.z < 0.45f) {
								auto hx = lmap<float>(pt.x, mHandDims.x, 0, 0, getWindowWidth());
								auto hy = lmap<float>(pt.y, 0, mHandDims.y, 0, getWindowHeight());
								mDustCloud->MouseSpawn(vec2(hx,hy));
							}
						}
					}
				}
			}
		}
		hand->Release();
		mPXC->ReleaseFrame();
	}
}

void prepareSettings(App::Settings *pSettings)
{
	pSettings->setWindowSize(1280, 720);
	pSettings->setFrameRate(60);
}

CINDER_APP( Circumstellar, RendererGl(RendererGl::Options().msaa(4)), prepareSettings )
