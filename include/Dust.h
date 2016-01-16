#pragma once
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

using namespace std;
using namespace ci;

class Circumstellar;


namespace CS_Dust
{
	struct DustVel //vec2
	{
		float Linear;
		float Angular;
		DustVel() {}
		DustVel(float pX, float pY) : Linear(pX), Angular(pY) {}
		DustVel(vec2 pVel) : Linear(pVel.x), Angular(pVel.y) {}
	};

	struct DustAngRad	//vec3
	{
		float Angle;
		float Radius_0;
		float Radius_1;
		DustAngRad() {}
		DustAngRad(float pX, float pY) : Angle(pX), Radius_0(pY) {}
		DustAngRad(vec2 pVel) : Angle(pVel.x), Radius_0(pVel.y) {}
	};

	struct DustDepth	//vec2
	{
		float Initial;
		float Current;
		DustDepth() {}
		DustDepth(float pDepth) : Initial(pDepth), Current(pDepth) {}
	};

	struct Dust
	{
		Dust(){}
		Dust(float pLinear, float pAngular, float pAngle, float pRadius, float pDepth, float pSize);

		void Step();

		bool		Live;
		DustVel		Speed;
		DustAngRad	Polars;
		DustDepth	Distance;
		float		Size;

		vec3		DrawPos;
		
	};

	class DustCloud;
	typedef shared_ptr<DustCloud> DustCloudRef;

	class DustCloud
	{
	public:
		static DustCloudRef create(string pTFShader, string pVertShader, string pFragShader, string pTexture, size_t pMax, float pDist, float pRadius, Circumstellar *pParent);
		void Update();
		void Draw(const CameraPersp &pCam);	//Debug billboards

		// DEBUG
		void MouseSpawn(const vec2 &pMousePos, const vec2 &pWindowSize, const CameraPersp & pCam);
		//
	protected:
		DustCloud() {}
		DustCloud(string pTFShader, string pVertShader, string pFragShader, string pTexture, size_t pMax, float pDist, float pRadius, Circumstellar *pParent);

	private:
		size_t			mMaxDust;
		float			mMaxDist,
						mMaxRadius,
						mTargetZ;

		vector<Dust>	mParticles;
		gl::GlslProgRef mShaderRender;

		gl::Texture2dRef	mDustTex;

		/*
		Tranform feedback the following attrs:
			vec2 Velocities <-> Dust.DustVel;
			vec3 AngleRadius <->Dust.DustAngRad;
			vec2 Distances <-> Dust.DustDepth;
			vec3 Positions -> DrawPos;
		*/
		gl::GlslProgRef				mShaderTF;
		gl::VaoRef					mVao[2];
		gl::TransformFeedbackObjRef	mTfo[2];
		gl::VboRef					mVboPos[2],
									mVboVel[2];

	};
}