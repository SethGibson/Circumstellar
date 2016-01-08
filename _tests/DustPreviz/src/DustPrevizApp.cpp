#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Camera.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;

static size_t S_MAX_DUST = 200;

struct Dust_t
{
	vec3 Pos;
	float Vel;

	Dust_t(vec3 pPos) : Pos(pPos)
	{
		Vel = randFloat(0.001f, 0.01f);
	}
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
}

void DustPreviz::mouseDown(MouseEvent event)
{
}

void DustPreviz::update()
{
}

void DustPreviz::draw()
{
	gl::clear(Color(0, 0, 0));
	gl::setMatrices(mCam);

	gl::color(Color::white());
	gl::drawColorCube(mAttractor, vec3(0.5f));
}

vec3 DustPreviz::getDustPos()
{

}
///////////////////////////////////////////////////////////////
void prepareSettings(App::Settings *pSettings)
{
	pSettings->setWindowSize(1280, 720);
}

CINDER_APP(DustPreviz, RendererGl, prepareSettings)

