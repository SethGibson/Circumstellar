#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Camera.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;

/*
	Angle Actual, Radius Actual	(in vec3, out vec3)
	Pos							(out vec3)
	Age							(in float, out float)
*/

struct ptcl
{
	vec3	Pos;
	vec2	Polars;	//x=Angle Actual, z=Radius Initial
	vec2	Speeds;	//x=linear, y=angular
	vec2	Age;	//HACKHACKHACK x=force affect countdown y=(bool)alive/dead
	
	ptcl() {}
	ptcl(float pZ)
	{
		Speeds.x = randFloat(0.025f, 0.075f);
		Speeds.y = randFloat(0.025f, 0.075f);
		Age.x = randFloat(60.0f, 180.0f);
		Age.y = 1.0f;

		Polars.x = randFloat(0, 2.0f*M_PI);
		Polars.y = randFloat(0.5f, 3.0f);

		//need to initialize Pos to something to keep the shader simple
		Pos.x = math<float>::cos(Polars.x)*Polars.y;
		Pos.y = math<float>::sin(Polars.x)*Polars.y;
		Pos.z = pZ;
	}
};

struct cb
{
	vec3	Pos;
	float	Rad;
	size_t	Count;
	cb() {}
	cb(vec3 pPos) : Pos(pPos)
	{
		Rad = randFloat(0.25f, 1.0f);
		Count = (size_t)randInt(100, 500);
	}
};

static size_t S_NUM_PTCLS = 100000;

class BlackHoleGPUApp : public App
{
public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;

private:
	int mPong;
	float	mMaxDist;
	vector<ptcl>	mPoints;

	//TF
	gl::GlslProgRef				mShaderTF;
	gl::VaoRef					mAttribBuffers[2];
	gl::VboRef					mDataBuffers[2];
	gl::TransformFeedbackObjRef	mTF[2];

	//Render
	CameraPersp	mCamera;
	gl::GlslProgRef	mShaderRender;
};

void BlackHoleGPUApp::setup()
{
	setWindowSize(1280, 720);
	mPong = 1;
	mMaxDist = 5.0f;
	mCamera.setPerspective(60.0f, getWindowAspectRatio(), 0.1f, 100.0f);
	mCamera.lookAt(vec3(0, 0, mMaxDist), vec3(), vec3(0, 1, 0));

	//TF
	gl::GlslProg::Format tf;
	vector<string> tfVars{"o_Pos","o_Polars","gl_SkipComponents2","o_Age"};
	tf.vertex(loadAsset("tf.vert"))
		.feedbackFormat(GL_INTERLEAVED_ATTRIBS)
		.feedbackVaryings(tfVars)
		.attribLocation("i_Pos", 0)
		.attribLocation("i_Polars", 1)
		.attribLocation("i_Speeds", 2)
		.attribLocation("i_Age", 3);
	
	mShaderTF = gl::GlslProg::create(tf);
	mShaderTF->uniform("u_Distance", vec2(mMaxDist,0.5f));

	//Render
	gl::GlslProg::Format rdr;
	rdr.vertex(loadAsset("render.vert"))
		.fragment(loadAsset("render.frag"))
		.attribLocation("o_Pos", 0);
	mShaderRender = gl::GlslProg::create(rdr);

	//Buffers
	for (int i = 0; i < 100;++i)
		mPoints.push_back(ptcl(mMaxDist));

	mDataBuffers[0] = gl::Vbo::create(GL_ARRAY_BUFFER, mPoints, GL_DYNAMIC_DRAW);
	mDataBuffers[1] = gl::Vbo::create(GL_ARRAY_BUFFER, mPoints.size()*sizeof(ptcl), nullptr, GL_DYNAMIC_DRAW);

	for (int i = 0; i < 2; ++i)
	{
		mAttribBuffers[i] = gl::Vao::create();
		mAttribBuffers[i]->bind();
		mDataBuffers[i]->bind();
		gl::vertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ptcl), (const GLvoid *)offsetof(ptcl, Pos));
		gl::enableVertexAttribArray(0);
		gl::vertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ptcl), (const GLvoid *)offsetof(ptcl, Polars));
		gl::enableVertexAttribArray(1);
		gl::vertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(ptcl), (const GLvoid *)offsetof(ptcl, Speeds));
		gl::enableVertexAttribArray(2);
		gl::vertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(ptcl), (const GLvoid *)offsetof(ptcl, Age));
		gl::enableVertexAttribArray(3);

		mTF[i] = gl::TransformFeedbackObj::create();
		mTF[i]->bind();
		gl::bindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, mDataBuffers[i]);
		mTF[i]->unbind();
	}
}

void BlackHoleGPUApp::mouseDown( MouseEvent event )
{

}

void BlackHoleGPUApp::update()
{
	//Lifecycle management
	mPong = 1 - mPong;

	{
		gl::ScopedVao vao(mAttribBuffers[mPong]);
		gl::ScopedState st(GL_RASTERIZER_DISCARD, true);
		gl::ScopedGlslProg shd(mShaderTF);


		mTF[1 - mPong]->bind();
		mTF[1 - mPong]->begin(GL_POINTS);
		gl::drawArrays(GL_POINTS, 0, mPoints.size());
		mTF[1 - mPong]->end();
		mTF[1 - mPong]->unbind();
	}

	for (auto p = begin(mPoints); p != end(mPoints);)
	{
		if (p->Age.y <= 0)
			p = mPoints.erase(p);
		else
			++p;
	}

	if (mPoints.size() < S_NUM_PTCLS)
		mPoints.push_back(ptcl(mMaxDist));
	//TF
	mDataBuffers[mPong]->bufferData(mPoints.size()*sizeof(ptcl), mPoints.data(), GL_DYNAMIC_DRAW);
	

}

void BlackHoleGPUApp::draw()
{
	gl::setMatrices(mCamera);

	gl::clear( Color( 0, 0, 0 ) ); 

	{
		gl::color(Color::white());
		gl::ScopedVao vao(mAttribBuffers[mPong]);
		gl::ScopedGlslProg shd(mShaderRender);
		//gl::ScopedState ps(GL_PROGRAM_POINT_SIZE, true);
		gl::setDefaultShaderVars();
		gl::drawArrays(GL_POINTS, 0, mPoints.size());
	}
}

CINDER_APP( BlackHoleGPUApp, RendererGl )
