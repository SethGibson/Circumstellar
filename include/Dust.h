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
		Dust(float pLinear, float pAngular, float pAngle, float pRadius, float pDepth, float pSize);

		vec3		AngleRadii;	//x=Angle, y=radius init z=radius actual
		vec3		DrawPos;
		vec2		Speed; //x = Linear, y = angular
		vec2		Distance;	//x=initial, y=actual
		float		Size;
		bool		Live;
	};

	class DustCloud;
	typedef shared_ptr<DustCloud> DustCloudRef;

	class DustCloud
	{
	public:
		static DustCloudRef create(string pVertShader, string pFragShader, string pTexture, size_t pMax, float pDist, float pRadius, Circumstellar *pParent);
		void Update();
		void Draw(const CameraPersp &pCam);	//Debug billboards

		// DEBUG
		void MouseSpawn(const vec2 &pMousePos, const vec2 &pWindowSize, const CameraPersp & pCam);
		//
	protected:
		DustCloud() {}
		DustCloud(string pVertShader, string pFragShader, string pTexture, size_t pMax, float pDist, float pRadius, Circumstellar *pParent);

	private:
		void setupTF(string pTFShader);
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