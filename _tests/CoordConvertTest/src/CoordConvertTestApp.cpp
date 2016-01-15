#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/Camera.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

static float S_PLANE_Z = 8.0f;

struct orbiter
{
	float Angle;
	float Radius;
	float Vel;
	float Depth;

	orbiter(float pAngle, float pRadius, float pVel) : Angle(pAngle), Radius(pRadius), Vel(pVel), Depth(S_PLANE_Z) {}
	void Draw()
	{
		Depth -= Vel;
		Angle -= Vel;
		float newR = lerp<float>(0.0f, Radius, Depth / S_PLANE_Z);
		auto x = math<float>::sin(Angle)*newR;
		auto y = math<float>::cos(Angle)*newR;

		gl::color(Color(0, 1, 0));
		gl::drawSphere(vec3(x, y, Depth), 0.25f);
	}
};

class CoordConvertTestApp : public App
{
public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;

private:
	vec3	mRayPos;
	vec2 mMousePos;

	CameraPersp mCamera;

	vector<orbiter>	mPoints;
};

void CoordConvertTestApp::setup()
{
	getWindow()->setSize(1280, 720);
	mCamera.setPerspective(90.0f, getWindowAspectRatio(), 0.0326f, 33.674f);
	mCamera.lookAt(vec3(0,0,10), vec3(), vec3(0,1,0));
}

void CoordConvertTestApp::mouseDown( MouseEvent event )
{
	mMousePos = event.getPos();
	auto ray = mCamera.generateRay(mMousePos, getWindowSize());

	float dist;
	if (ray.calcPlaneIntersection(vec3(0,0,S_PLANE_Z), vec3(0, 0, -1), &dist))
		mRayPos = ray.calcPosition(dist);

	auto angle = math<float>::atan2(mRayPos.x, mRayPos.y);
	if (angle < 0)
		angle += (2.0f*M_PI);

	auto rad = length(vec2(mRayPos));

	mPoints.push_back(orbiter(angle, rad, 0.01f));
}

void CoordConvertTestApp::update()
{
}

void CoordConvertTestApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 

	gl::setMatrices(mCamera);
	gl::color(Color::white());
	gl::drawSphere(mRayPos, 0.1f);

	for (auto &o : mPoints)
		o.Draw();
}

CINDER_APP( CoordConvertTestApp, RendererGl )
