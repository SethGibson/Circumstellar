#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class TFMultiAttrApp : public App 
{
public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;

private:
	vector<vec3>					mPoints;
	gl::GlslProgRef					mShaderTF, mShaderRender;

	gl::VaoRef						mVao[2];
	gl::TransformFeedbackObjRef		mFeedbackObj[2];
	gl::VboRef						mVbo[2];
};

void TFMultiAttrApp::setup()
{
	gl::GlslProg::Format tf;
	tf.vertex(loadAsset("tf.vert"))
		.feedbackVaryings({ "newPos" })
		.attribLocation("oldPos", 0)
		.feedbackFormat(GL_INTERLEAVED_ATTRIBS);

	mShaderTF = gl::GlslProg::create(tf);
	mShaderRender = gl::GlslProg::create(loadAsset("render.vert"), loadAsset("render.frag"));

	// Buffers
	//mVbo[0] = gl::Vbo::create(GL_ARRAY_BUFFER, )
}


void TFMultiAttrApp::mouseDown( MouseEvent event )
{
}

void TFMultiAttrApp::update()
{
}

void TFMultiAttrApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP( TFMultiAttrApp, RendererGl )
