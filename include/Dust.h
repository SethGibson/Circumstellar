#pragma once
#include "cinder/gl/gl.h"

using namespace std;
using namespace ci;

class Circumstellar;

namespace CS_Dust
{
	struct Dust
	{

	};

	class DustCloud;
	typedef shared_ptr<DustCloud> DustCloudRef;

	class DustCloud
	{
	public:
		static DustCloudRef create(string pTFShader, string pVertShader, string pFragShader, Circumstellar *pParent);
		void Update();
		void Draw();

	protected:
		DustCloud() {}
		DustCloud(string pTFShader, string pVertShader, string pFragShader, Circumstellar *pParent);

	private:
		vector<Dust>	mParticles;
		gl::GlslProgRef mShaderTF;
		gl::GlslProgRef	mShaderRender;

		gl::VaoRef					mVao[2];
		gl::TransformFeedbackObjRef	mTfo[2];
		gl::VboRef					mVboPos[2],
									mVboVel[2];

	};
}