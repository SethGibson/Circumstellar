#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;

static size_t S_NUM_PTCL = 100;

struct ptcl
{
	vec2	Pos;
	vec2	AngleRadii;	//x=Angle, y=Radius
	float	Speed;

	ptcl() {}
	ptcl(float pSpeed, vec2 pAR) : Speed(pSpeed), AngleRadii(pAR){}
};

class SkipComponentsApp : public App
{
public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;

private:
	int				mPong;
	vector<ptcl>	mPoints;

	gl::GlslProgRef		mShaderTF,
						mShaderRender;

	// Transform Feedback
	gl::VaoRef					mAttribBuffers[2];
	gl::VboRef					mDataBuffers[2];
	gl::TransformFeedbackObjRef	mTFBuffers[2];
};

void SkipComponentsApp::setup()
{
	setWindowSize(640, 360);
	mPong = 1;
	mShaderRender = gl::GlslProg::create(loadAsset("render.vert"), loadAsset("render.frag"));

	gl::GlslProg::Format tf;
	vector<string> tfVars
	{
		"o_Pos",
		"o_Polars",
		"gl_SkipComponents1"
	};

	tf.vertex(loadAsset("tf.vert"))
		.feedbackFormat(GL_INTERLEAVED_ATTRIBS)
		.feedbackVaryings(tfVars)
		.attribLocation("i_Polars", 0)
		.attribLocation("i_Speed", 1);

	mShaderTF = gl::GlslProg::create(tf);

	for (int i = 0; i < S_NUM_PTCL; ++i)
	{
		float speed = randFloat(0.01f, 0.1f);
		float angle = randFloat(0.0, 2.0f*M_PI);
		float rad_0 = randFloat(10.0f, getWindowHeight()*0.5f);
		mPoints.push_back(ptcl(speed, vec2(angle, rad_0)));
	}

	//Buffers
	mDataBuffers[0] = gl::Vbo::create(GL_ARRAY_BUFFER, mPoints, GL_STATIC_DRAW);
	mDataBuffers[1] = gl::Vbo::create(GL_ARRAY_BUFFER, mPoints.size()*sizeof(ptcl), nullptr, GL_STATIC_DRAW);

	for (int i = 0; i < 2; ++i)
	{
		mAttribBuffers[i] = gl::Vao::create();
		mAttribBuffers[i]->bind();
		mDataBuffers[i]->bind();
		gl::vertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ptcl), (const GLvoid *)offsetof(ptcl, AngleRadii));
		gl::enableVertexAttribArray(0);
		gl::vertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(ptcl), (const GLvoid *)offsetof(ptcl, Speed));
		gl::enableVertexAttribArray(1);

		mTFBuffers[i] = gl::TransformFeedbackObj::create();
		mTFBuffers[i]->bind();
		gl::bindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, mDataBuffers[i]);
		mTFBuffers[i]->unbind();
	}
}

void SkipComponentsApp::mouseDown( MouseEvent event )
{
}

void SkipComponentsApp::update()
{
	{
		mPong = 1 - mPong;
		{
			gl::ScopedGlslProg tf(mShaderTF);
			gl::ScopedVao	attribs(mAttribBuffers[mPong]);
			gl::ScopedState rDiscard(GL_RASTERIZER_DISCARD, GL_TRUE);

			mTFBuffers[1 - mPong]->bind();
			gl::beginTransformFeedback(GL_POINTS);
			gl::drawArrays(GL_POINTS, 0, S_NUM_PTCL);
			gl::endTransformFeedback();
		}
	}
}

void SkipComponentsApp::draw()
{
	auto points = reinterpret_cast<ptcl *>(mDataBuffers[mPong]->map(GL_READ_ONLY));
	mDataBuffers[mPong]->unmap();

	gl::clear( Color( 0, 0, 0 ) ); 
	gl::color(Color::white());
	gl::setMatricesWindow(getWindowSize());
	gl::pushModelMatrix();
	gl::translate(vec2(getWindowSize())*0.5f);

	for (int j = 0; j < S_NUM_PTCL;++j)
		gl::drawSolidCircle(points[j].Pos, 2.5f);

	gl::popModelMatrix();
}

CINDER_APP( SkipComponentsApp, RendererGl )
