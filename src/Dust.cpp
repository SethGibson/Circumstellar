#include "Dust.h"
#include "CircumstellarApp.h"

namespace CS_Dust
{
	Dust::Dust(float pLinear, float pAngular, float pAngle, float pRadius, float pDepth, float pSize=0) : Live(true)
	{
		if (pLinear > 0 && pAngular>0)
			Speed = DustVel(pLinear, pAngular);
		else
		{
			Speed.Linear = pLinear <= 0 ? randFloat(0.001f, 0.01f) : pLinear;
			Speed.Angular = pAngular <= 0 ? randFloat(0.025f, 0.075f) : pAngular;
		}

		Polars.Angle = pAngle <= 0 ? randFloat(0.0f, 2.f*M_PI) : pAngle;
		Polars.Radius_0 = pRadius<=0 ? randFloat(0.25f,0.75f) : pRadius;
		Distance = DustDepth(pDepth);
		Size = pSize <= 0 ? randFloat(0.01f, 0.035f) : pSize;
	}

	void Dust::Step()
	{
		if (Distance.Current > 0.5f)
		{
			Distance.Current -= Speed.Linear;
			Polars.Angle -= Speed.Angular;
			Polars.Radius_1 = lerp<float>(0.0f, Polars.Radius_0, Distance.Current / Distance.Initial);

			auto x = math<float>::sin(Polars.Angle)*Polars.Radius_1;
			auto y = math<float>::cos(Polars.Angle)*Polars.Radius_1;
			DrawPos = vec3(x, y, Distance.Current);
		}

		else
			Live = false;
	}

	DustCloud::DustCloud(string pTFShader, string pVertShader, string pFragShader,	string pTexture, 
		size_t pMax, float pDist, float pRadius, 
		Circumstellar *pParent) : mMaxDust(pMax), mMaxDist(pDist-1.0f), mMaxRadius(pRadius), mTargetZ(2.5f)
	{
		mDustTex = gl::Texture2d::create(loadImage(loadAsset(pTexture)));
		mShaderTF = gl::GlslProg::create(pParent->loadAsset(pTFShader));
		mShaderRender = gl::GlslProg::create(pParent->loadAsset(pVertShader), pParent->loadAsset(pFragShader));
		mShaderRender->uniform("u_Sampler", 0);
		mShaderRender->uniform("u_Max", pDist);

		mParticles.push_back(Dust(-1,-1,-1,-1,mMaxDist));
		mDebugClickPos = vec3();
	}

	DustCloudRef DustCloud::create(string pTFShader, string pVertShader, string pFragShader, string pTexture, size_t pMax, float pDist, float pRadius, Circumstellar *pParent)
	{
		return DustCloudRef(new DustCloud(pTFShader, pVertShader, pFragShader, pTexture, pMax, pDist, pRadius, pParent));
	}

	void DustCloud::Update()
	{
		if (mParticles.size() < mMaxDust)
		{
			for (int i = 0; i < 3;++i)
				mParticles.push_back(Dust(-1, -1, -1, -1, mMaxDist));
		}
		for (auto p = begin(mParticles); p != end(mParticles);)
		{
			if (!p->Live)
				p = mParticles.erase(p);
			else
			{
				p->Step();
				p++;
			}
		}
	}

	void DustCloud::Draw(const CameraPersp &pCam)
	{
		vec3 right, up;
		pCam.getBillboardVectors(&right, &up);
		gl::color(Color::white());
		gl::drawSphere(mDebugClickPos, 0.005f);
		{
			gl::ScopedGlslProg shader(mShaderRender);
			gl::ScopedTextureBind tex(mDustTex, 0);
			
			for (const auto &d : mParticles)
				gl::drawBillboard(d.DrawPos, vec2(d.Size), 0.0f, right, up);
		}
	}

	vec3 DustCloud::MouseSpawn(const vec2 &pMousePos, const vec2 &pWindowSize, const CameraPersp & pCam)
	{
		auto ray = pCam.generateRay(pMousePos, getWindowSize());

		float dist;
		vec3 rayPos;
		if (ray.calcPlaneIntersection(vec3(0, 0, mMaxDist), vec3(0, 0, -1), &dist))
		{
			rayPos = ray.calcPosition(dist);
			auto angle = math<float>::atan2(rayPos.x, rayPos.y);
			if (angle < 0)
				angle += (2.0f*M_PI);

			auto rad = length(vec2(rayPos));

			mParticles.push_back(Dust(-1, -1, angle, rad, mMaxDist, 0.5f));
			
			mDebugClickPos = rayPos;
			return rayPos;
		}

		return vec3();
	}
} 