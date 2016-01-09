#pragma once
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

using namespace std;
using namespace ci;

class Circumstellar;

namespace CS_Dust
{
	struct Dust
	{
		Dust(){}
		Dust(float pRadius, float pDist);

		void Step(float pDist);

		bool Live;
		vec2 Vel;	//x = linear, y = angular
		vec3 Pos;
		vec3 Angle; //x = Angle, y = Radius_0, z=Radius_1
		float Size;
	};

	class DustCloud;
	typedef shared_ptr<DustCloud> DustCloudRef;

	class DustCloud
	{
	public:
		static DustCloudRef create(string pTFShader, string pVertShader, string pFragShader, string pTexture, size_t pMax, float pDist, float pRadius, Circumstellar *pParent);
		void Update();
		void Draw(const CameraPersp &pCam);	//Debug billboards

	protected:
		DustCloud() {}
		DustCloud(string pTFShader, string pVertShader, string pFragShader, string pTexture, size_t pMax, float pDist, float pRadius, Circumstellar *pParent);

	private:
		size_t			mMaxDust;
		float			mMaxDist;
		float			mMaxRadius;

		vector<Dust>	mParticles;
		gl::GlslProgRef mShaderTF,
						mShaderRender;

		gl::Texture2dRef	mDustTex;


		gl::VaoRef					mVao[2];
		gl::TransformFeedbackObjRef	mTfo[2];
		gl::VboRef					mVboPos[2],
									mVboVel[2];

	};
}