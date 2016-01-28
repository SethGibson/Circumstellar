#include "CircumstellarApp.h"

static size_t S_MAX_DUST = 5000;

void Circumstellar::setup()
{
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

void Circumstellar::update()
{
	updateHand();
	mDustCloud->Update();
}

void Circumstellar::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
	gl::setMatrices(mCamera);

	gl::color(Color::white());
	mDustCloud->Draw();

	gl::pushModelMatrix();
	gl::scale(vec3(1.5));
	mBlackHole->Draw(static_cast<float>(getElapsedFrames()));
	gl::popModelMatrix();

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
