#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Camera.h"
#include "cinder/Rand.h"
#include "cinder/Log.h"

using namespace ci;
using namespace ci::app;
using namespace std;

struct Dust
{
	Dust();
	Dust(float pRadius, float pDist)
	{
		Vel = randFloat(0.01f, 0.1f);
		RVel = randFloat(0.005f, 0.05f);
		Ang = randFloat(0.0f, 2.0f*M_PI);

		Size = randFloat(0.05f, 0.25f);
		Pos = vec3(math<float>::cos(Ang)*pRadius, math<float>::sin(Ang)*pRadius, pDist);
	}

	vec3 Pos;
	float Size;
	float Vel;
	float RVel;
	float Ang;
};

class DustTestApp : public App
{
public:
	DustTestApp();
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;

private:
	void	calcNewPosition(Dust& pDust);

	gl::GlslProgRef	mShader;
	gl::Texture2dRef	mTex;
	size_t			mMaxDust;
	float			mRadius;
	float			mMaxDist;

	vec3			mAttractor;
	CameraPersp		mCam;
	vector<Dust>	mDust;
};

DustTestApp::DustTestApp() : mRadius(5), mAttractor(vec3()), mMaxDust(500), mMaxDist(10)
{
	mCam.setPerspective(90.0f, getWindowAspectRatio(), 0.01f, 100.0f);
	mCam.lookAt(vec3(0, 0, mMaxDist), vec3(), vec3(0, 1, 0));
}

void DustTestApp::setup()
{
	mDust.push_back(Dust(mRadius, mMaxDist));
	mTex = gl::Texture2d::create(loadImage(loadAsset("TX_Sprite.png")));
	mShader = gl::GlslProg::create(loadAsset("sprite.vert"), loadAsset("sprite.frag"));
	mShader->uniform("u_Sampler", 0);
	mShader->uniform("u_Max", mMaxDist);
	gl::enableAdditiveBlending();
}

void DustTestApp::mouseDown( MouseEvent event )
{
}

void DustTestApp::update()
{
	if (mDust.size() < mMaxDust)
		mDust.push_back(Dust(mRadius, mMaxDist));

	for (auto p = begin(mDust);p!=end(mDust);)
	{
		if (p->Pos.z <= 0.1f)
			p = mDust.erase(p);
		else
		{
			calcNewPosition(*p);
			++p;
		}
	}
}

void DustTestApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
	gl::setMatrices(mCam);

	gl::color(Color::white());
	{
		gl::ScopedGlslProg shader(mShader);
		gl::ScopedTextureBind tex(mTex, 0);
		vec3 up, right;
		mCam.getBillboardVectors(&right, &up);
		for (const auto &d : mDust)
			gl::drawBillboard(d.Pos, vec2(d.Size), -d.Ang, right, up);
	}
}

void DustTestApp::calcNewPosition(Dust& pDust)
{
	pDust.Ang += pDust.RVel;
	pDust.Pos.z -= pDust.Vel;

	auto newR = mRadius*(pDust.Pos.z / mMaxDist);
	pDust.Pos.x = math<float>::cos(pDust.Ang)*newR;
	pDust.Pos.y = math<float>::sin(pDust.Ang)*newR;
}

void prepareSettings(App::Settings *pSettings)
{
	pSettings->setWindowSize(1280, 720);
	pSettings->setFrameRate(60);
}

CINDER_APP( DustTestApp, RendererGl, prepareSettings )
