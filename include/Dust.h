#pragma once
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "CiLibRS.h"

using namespace std;
using namespace ci;

class Circumstellar;

namespace CS_Dust
{
	struct Dust
	{
		Dust(){}
		Dust(float pLinear, float pAngular, float pAngle, float pRadius, float pDepth, float pSize, int pStartOffset, Color pTint);

		vec3		AngleRadii;	//x=Angle, y=radius init z=radius actual
		vec3		DrawPos;
		vec2		Speed; //x = Linear, y = angular
		vec2		Distance;	//x=initial, y=actual
		float		Size;
		float		Alpha;
		bool		Live;
		int			Start;
		Color		Tint;
	};

	class DustCloud;
	typedef shared_ptr<DustCloud> DustCloudRef;

	class DustCloud
	{
	public:
		static DustCloudRef create(size_t pMax, float pDist, float pRadius, const CameraPersp & pCam);
		void Update();
		void Draw(const CameraPersp &pCam);	//Debug billboards

		// DEBUG
		void MouseSpawn(const vec2 &pMousePos, const vec2 &pWindowSize);
		//
	protected:
		DustCloud() {}
		DustCloud(size_t pMax, float pDist, float pRadius, const CameraPersp & pCam);

	private:
		void setupDust();
		void setupLibRS();

		void getDrawPoints();
		void capturePoints();

		const CameraPersp	mCamera;
		size_t			mMaxDust;
		int				mCaptureTime;
		float			mMaxDist,
						mPlaneDist,
						mCutoffDist,
						mMaxRadius,
						mTargetZ;

		vector<Dust>	mParticles;
		gl::GlslProgRef mShaderRender;
		gl::VboRef		mDustData;
		gl::BatchRef	mDustDraw;

		gl::Texture2dRef	mTexDust;

		CiLibRS::RSDeviceRef	mRS;
		
		float				mDepthCutoff;
		vector<vec3>		mDepthPoints;
		gl::VboRef			mPointData;
		gl::BatchRef		mPointDraw;
		gl::Texture2dRef	mTexPoints;
		gl::GlslProgRef		mShaderPoints;
		vec2				mDustPlaneMin;
		vec2				mDustPlaneMax;
	};
}
