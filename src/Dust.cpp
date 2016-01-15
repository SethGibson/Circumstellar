#include "Dust.h"
#include "CircumstellarApp.h"

namespace CS_Dust
{
	Dust::Dust(float pRadius, float pAngle, float pDist, float pVel, float pSize=0) : Live(true)
	{
		Vel.x = randFloat(0.005f, 0.01f);
		if (pVel == -1.0f)
			Vel.y = randFloat(0.01f, 0.05f);
		else
			Vel.y = pVel;
		if (pSize < 0)
			Size = randFloat(0.01f, 0.035f);
		else
			Size = pSize;
		if (pAngle < 0)
			Angle.x = randFloat(0.0f, 2.f*M_PI);
		else
			Angle.x = pAngle;

		Angle.y = pRadius;
		Angle.z = pRadius;

		Pos.x = math<float>::cos(Angle.x)*Angle.z;
		Pos.y = math<float>::sin(Angle.x)*Angle.z;
		Pos.z = pDist;

	}

	void Dust::Step(float pDist)
	{
		Pos.z -= Vel.x;
		if (Pos.z > 0.5f)
		{
			Angle.x += Vel.y;
			Angle.z = Angle.y * (Pos.z / pDist);

			Pos.x = math<float>::cos(Angle.x)*Angle.z;
			Pos.y = math<float>::sin(Angle.x)*Angle.z;
		}
		else
			Live = false;
	}

	DustCloud::DustCloud(string pTFShader, string pVertShader, string pFragShader,	string pTexture, 
		size_t pMax, float pDist, float pRadius, 
		Circumstellar *pParent) : mMaxDust(pMax), mMaxDist(pDist), mMaxRadius(pRadius), mTargetZ(2.5f)
	{
		mDustTex = gl::Texture2d::create(loadImage(loadAsset(pTexture)));
		mShaderTF = gl::GlslProg::create(pParent->loadAsset(pTFShader));
		mShaderRender = gl::GlslProg::create(pParent->loadAsset(pVertShader), pParent->loadAsset(pFragShader));
		mShaderRender->uniform("u_Sampler", 0);
		mShaderRender->uniform("u_Max", pDist);

		mParticles.push_back(Dust(pRadius, -1, pDist, -1.0f));
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
				mParticles.push_back(Dust(mMaxRadius, -1, mMaxDist, -1.0f));
		}
		for (auto p = begin(mParticles); p != end(mParticles);)
		{
			if (!p->Live)
				p = mParticles.erase(p);
			else
			{
				p->Step(mMaxDist);
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
			{
				gl::drawBillboard(d.Pos, vec2(d.Size), 0.0f, right, up);
			}
		}
	}


	vec2 DustCloud::r2p(vec2 pR)
	{
		//return vec2(math<float>::hypot(pR.x, pR.y), math<float>::atan2(pR.x, pR.y));

		float rad = math<float>::sqrt((math<float>::pow(pR.x,2))-(math<float>::pow(pR.y, 2)));
		float ang = math<float>::atan(pR.x / pR.y);

		return vec2(rad, ang);
	}

	vec3 DustCloud::MouseSpawn(const vec2 &pMousePos, const vec2 &pWindowSize, const float &pAspect, const CameraPersp & pCam)
	{
		float u = (pWindowSize.x-pMousePos.x) / pWindowSize.x;
		//float v = (pWindowSize.y - pMousePos.y) / pWindowSize.y;
		float v = (pWindowSize.y - pMousePos.y) / pWindowSize.y;
		auto ray = pCam.generateRay(u, v, pAspect);

		auto newPos = ray.calcPosition(mMaxDist / pCam.getFarClip());
		
		//auto dust_0 = r2p(vec2(newPos));

		//make a huge particle for debug purposes
		mDebugClickPos = vec3(-newPos.x,newPos.y,newPos.z);
		auto dust_0 = r2p(vec2(mDebugClickPos));
		CI_LOG_I("Radius Angle Spawn: " << dust_0.x << ", " << dust_0.y);

		mParticles.push_back(Dust(mMaxRadius, dust_0.y, 2.0f, 0.0f, 0.5f));

		return mDebugClickPos;
	}
} //CS_Dust