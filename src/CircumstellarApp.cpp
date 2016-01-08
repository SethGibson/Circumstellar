#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class CircumstellarApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
};

void CircumstellarApp::setup()
{
}

void CircumstellarApp::mouseDown( MouseEvent event )
{
}

void CircumstellarApp::update()
{
}

void CircumstellarApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP( CircumstellarApp, RendererGl )
