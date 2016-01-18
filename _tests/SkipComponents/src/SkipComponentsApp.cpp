#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class SkipComponentsApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
};

void SkipComponentsApp::setup()
{
}

void SkipComponentsApp::mouseDown( MouseEvent event )
{
}

void SkipComponentsApp::update()
{
}

void SkipComponentsApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP( SkipComponentsApp, RendererGl )
