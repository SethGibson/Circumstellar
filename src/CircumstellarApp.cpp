#include "CircumstellarApp.h"

const size_t kMaxDust = 20000;
const int kTimeCapture = 300;
const int kTimePaused = 500;

void Circumstellar::setup()
{
	mDrawGUI = true;
	mMaxDist = 3.0f;
	mIsCapturing = false;
	mTimeCapturing = kTimeCapture;
	mTimePaused = kTimePaused;

	setupGUI();
	
	mCamera.setPerspective(90.0f, getWindowAspectRatio(), 0.01f, 100.0f);
	mCamera.lookAt(vec3(0, 0, mMaxDist), vec3(), vec3(0, 1, 0));

	mBlackHole = CS_Dust::BlackHole::create();
	mDustCloud = CS_Dust::DustCloud::create(kMaxDust, mMaxDist, 1.0f, mCamera);
	mShaderDepth = gl::GlslProg::create(loadAsset("shaders/depth_tex.vert"), loadAsset("shaders/depth_tex.frag"));
	mShaderDepth->uniform("u_SamplerDepth", 0);

	setupDS();
	gl::enableAdditiveBlending();
}

void Circumstellar::mouseDown( MouseEvent event )
{
	mDustCloud->MouseSpawn(vec2(event.getPos()));
}

void Circumstellar::mouseDrag(MouseEvent event)
{
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

	if (!mIsCapturing) {
		if (mTimeCapturing > 0) {
			updateDepth();
			mTimeCapturing -= 1;
		}
		else if (mTimeCapturing==0) {
			mDustCloud->CapturePoints(mRGBDepth);
			mIsCapturing = true;
			mTimeCapturing = kTimeCapture;
		}
	}
	else {
		if (mIsCapturing) {
			if (mTimePaused > 0) {
				mTimePaused -= 1;
			}
			else {
				mIsCapturing = false;
				mTimePaused = kTimePaused;
			}
		}
	}

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

	gl::setMatricesWindow(getWindowSize());
	gl::color(Color::white());
	if (!mIsCapturing) {
		gl::pushModelMatrix();
		gl::translate(getWindowWidth(), 0);
		gl::scale(-1, 1);
		{
			auto tex = gl::Texture2d::create(*mRGBDepth);
			gl::ScopedTextureBind depthTex(tex, 0);
			gl::ScopedGlslProg depthShader(mShaderDepth);
			gl::drawSolidRect(Rectf(0, 0, getWindowWidth(), getWindowHeight()));
		}
		gl::popModelMatrix();
	}
	if (mDrawGUI) {
		gl::disableDepthRead();
		mGUI->draw();
	}
}

void Circumstellar::cleanup()
{
	mDS->stop();
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

void Circumstellar::setupDS()
{
	mDS = CinderDSAPI::create();
	mDS->init();

	mDS->initDepth(FrameSize::DEPTHQVGA, 60);
	mDS->initRgb(FrameSize::RGBVGA, 60);
	mRGBDepth = Surface8u::create(320, 240, false, SurfaceChannelOrder::RGB);

	mDS->start();
}

void Circumstellar::updateDepth()
{
	mDS->update();
	auto depth = mDS->getDepthFrame();
	auto iter = mRGBDepth->getIter();
	auto diter = depth->getIter();
	auto dd = mDS->getDepthSize();

	while (iter.line())
	{
		while (iter.pixel())
		{
			auto x = iter.x();
			auto y = iter.y();
			iter.r() = 0.0f;
			iter.g() = 0.0f;
			iter.b() = 0.0f;
			auto d = (float)depth->getValue(ivec2(x, y));
			if (d < 1500.0f)
			{
				Color8u c = (Color8u)mDS->getColorFromDepthImage((float)x, (float)y, d);
				iter.r() = c.r;
				iter.g() = c.g;
				iter.b() = c.b;
			}
		}
	}
}

void prepareSettings(App::Settings *pSettings)
{
	pSettings->setWindowSize(1280, 720);
	pSettings->setFrameRate(60);
}

CINDER_APP( Circumstellar, RendererGl(RendererGl::Options().msaa(4)), prepareSettings )
