#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Camera.h"
#include "cinder/CameraUi.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;

static size_t S_MAX_DUST = 200;
static float S_SPAWN_R = 5.0f;

struct Dust_t
{
	vec3 Pos;
	float Vel;
	float Size;
	ColorA Shade;

	Dust_t(vec3 pPos) : Pos(pPos)
	{
		Vel = randFloat(0.001f, 0.01f);
		Size = randFloat(0.01f, 0.1f);
		Shade = ColorA(0.0f,randFloat(0.25f,0.75f), randFloat(0.25f, 0.75f), randFloat(0.1f,0.5f));
	}
};

struct Dust_s
{

};

class DustPreviz : public App
{
public:
	void setup() override;
	void mouseDown(MouseEvent event) override;
	void update() override;
	void draw() override;

private:
	vec3 getDustPos();

	CameraPersp	mCam;
	CameraUi	mCtrl;
	vec3	mAttractor,
		mEye;

	vector<Dust_t> mDust;
};

void DustPreviz::setup()
{
	mAttractor = vec3(0);
	mEye = vec3(0, 0, 10);

	mCam.setPerspective(90.0f, getWindowAspectRatio(), 0.1f, 100.0f);
	mCam.lookAt(mEye, mAttractor, vec3(0, 1, 0));

	mCtrl.setCamera(&mCam);
	mCtrl.connect(getWindow());
	mDust.push_back(Dust_t(getDustPos()));

	gl::clear(Color(0, 0, 0));
	//gl::enableDepth();
}

void DustPreviz::mouseDown(MouseEvent event)
{
}

void DustPreviz::update()
{
	if (mDust.size() < S_MAX_DUST)
		mDust.push_back(Dust_t(getDustPos()));

	for (auto d = begin(mDust); d != end(mDust);)
	{
		if (d->Pos.z <= 2.0f)
			d = mDust.erase(d);
		else
		{
			auto v = mAttractor - d->Pos;
			d->Pos += v*d->Vel;
			d->Pos.x += math<float>::sin(getElapsedFrames()*d->Vel)*0.01f;
			d->Pos.y += math<float>::cos(getElapsedFrames()*d->Vel)*0.02f;
			d++;
		}
	}
}

void DustPreviz::draw()
{
	gl::setMatricesWindow(getWindowSize());
	gl::color(ColorA(0, 0, 0, 0.01f));
	gl::drawSolidRect(Rectf(0,0,getWindowWidth(),getWindowHeight()));

	gl::setMatrices(mCam);
	gl::color(Color::white());
	gl::drawColorCube(mAttractor, vec3(0.5f));

	gl::pushModelMatrix();
	gl::rotate(getElapsedFrames()*0.01f);
	for (const auto &d : mDust)
	{
		gl::color(d.Shade);
		gl::drawCube(d.Pos, vec3(d.Size));
	}
	gl::popModelMatrix();
}

vec3 DustPreviz::getDustPos()
{
	auto theta = randFloat(0.0f, 2.0f*M_PI);
	auto x = math<float>::cos(theta)*S_SPAWN_R;
	auto y = math<float>::sin(theta)*S_SPAWN_R;

	return vec3(x, y, mEye.z);
}
///////////////////////////////////////////////////////////////
void prepareSettings(App::Settings *pSettings)
{
	pSettings->setWindowSize(1280, 720);
}

CINDER_APP(DustPreviz, RendererGl(RendererGl::Options().msaa(4)), prepareSettings)

