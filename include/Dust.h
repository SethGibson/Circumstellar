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
		Dust(float pLinear, float pAngular, float pAngle, float pRadius, float pDepth, float pSize, int pStartOffset, ColorA pTint);

		vec3		AngleRadii;	//x=Angle, y=radius init z=radius actual
		vec3		DrawPos;
		vec2		Speed; //x = Linear, y = angular
		vec2		Distance;	//x=initial, y=actual
		float		Size;
		float		Alpha;
		bool		Live;
		int			Start;
		ColorA		Tint;
	};

	class DustCloud;
	typedef shared_ptr<DustCloud> DustCloudRef;

	class DustCloud
	{
	public:
		static DustCloudRef create(size_t pMax, float pDist, float pRadius, const CameraPersp &pCam);
		void Update();
		void Draw();	//Debug billboards

		// DEBUG
		void MouseSpawn(const vec2 &pMousePos);
		//

		void CapturePoints(const Surface8uRef &pSurface);

	protected:
		DustCloud() {}
		DustCloud(size_t pMax, float pDist, float pRadius, const CameraPersp &pCam);

	private:
		void setupDust();
		size_t			mMaxDust;
		float			mMaxDist,
						mCutoffDist,
						mMaxRadius,
						mTargetZ;

		vector<Dust>	mParticles;
		gl::GlslProgRef mShaderRender;
		gl::VboRef		mDustData;
		gl::BatchRef	mDustDraw;

		gl::Texture2dRef	mDustTex;

		CameraPersp mCam;
	};
}