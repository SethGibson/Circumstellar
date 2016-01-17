#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;

struct ptcl
{
	vec3 Pos;
	float Vel;
	ptcl() {}
	ptcl(vec3 pPos) : Pos(pPos)
	{
		Vel = randFloat(1, 5);
	}
};

class TFMultiAttrApp : public App 
{
public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;

private:
	int								mPong;
	vector<ptcl>					mPoints;
	gl::GlslProgRef					mShaderTF, mShaderRender;

	gl::VaoRef						mVao[2];
	gl::TransformFeedbackObjRef		mFeedback[2];
	gl::VboRef						mPtclData[2];
};

void TFMultiAttrApp::setup()
{
	setWindowSize(1280, 720);
	mPong = 1;

	vector<string> fvars{ "OutPos","OutVel" };
	gl::GlslProg::Format tf;
	tf.vertex(loadAsset("tf.vert"))
		.feedbackVaryings(fvars)
		.attribLocation("InPos", 0)
		.attribLocation("InVel", 1)
		.feedbackFormat(GL_INTERLEAVED_ATTRIBS);
	mShaderTF = gl::GlslProg::create(tf);

	for (int i = 0; i < 100; ++i)
	{
		auto x = randFloat(getWindowWidth());
		auto y = getWindowHeight();
		mPoints.push_back( ptcl(vec3(x,y,0)) );
	}

	mPtclData[0] = gl::Vbo::create(GL_ARRAY_BUFFER, mPoints, GL_STATIC_DRAW);
	mPtclData[1] = gl::Vbo::create(GL_ARRAY_BUFFER, mPoints.size()*sizeof(ptcl), nullptr, GL_STATIC_DRAW);

	for (int i = 0; i < 2; ++i)
	{
		mVao[i] = gl::Vao::create();
		mVao[i]->bind();
		mPtclData[i]->bind();
		gl::vertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ptcl), (const GLvoid *)0);
		gl::enableVertexAttribArray(0);
		gl::vertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(ptcl), (const GLvoid *)offsetof(ptcl,Vel));
		gl::enableVertexAttribArray(1);

		mFeedback[i] = gl::TransformFeedbackObj::create();
		mFeedback[i]->bind();
		gl::bindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, mPtclData[i]);
		mFeedback[i]->unbind();
	}
}


void TFMultiAttrApp::mouseDown( MouseEvent event )
{
}

void TFMultiAttrApp::update()
{
	mPong = 1 - mPong;

	{
		gl::ScopedGlslProg	shader(mShaderTF);
		gl::ScopedVao		vao(mVao[mPong]);
		gl::ScopedState		rDiscard(GL_RASTERIZER_DISCARD, true);

		mFeedback[1 - mPong]->bind();
		gl::beginTransformFeedback(GL_POINTS);
		gl::drawArrays(GL_POINTS, 0, 100);
		gl::endTransformFeedback();
	}
}

void TFMultiAttrApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
	gl::setMatricesWindow(getWindowSize());
	gl::color(Color::white());

	{
		auto pData = reinterpret_cast<ptcl *>(mPtclData[mPong]->map(GL_READ_ONLY));
		mPtclData[mPong]->unmap();
		for (int i = 0; i < 100; ++i)
		{
			gl::drawSolidCircle(vec2(pData[i].Pos), 5);
		}
	}
}

CINDER_APP( TFMultiAttrApp, RendererGl )
