#include "Dust.h"
#include "CircumstellarApp.h"

namespace CS_Dust
{
	Dust::Dust(float pLinear, float pAngular, float pAngle, float pRadius, float pDepth, float pSize=0) : Live(true)
	{
		if (pLinear > 0 && pAngular>0)
			Speed = vec2(pLinear, pAngular);
		else
		{
			Speed.x = pLinear <= 0 ? randFloat(0.001f, 0.01f) : pLinear;
			Speed.y = pAngular <= 0 ? randFloat(0.01f, 0.05f) : pAngular;
		}

		AngleRadii.x = pAngle <= 0 ? randFloat(0.0f, 2.f*M_PI) : pAngle;
		AngleRadii.y = pRadius<=0 ? randFloat(0.25f,0.75f) : pRadius;
		Distance = vec2(pDepth);
		Size = pSize <= 0 ? randFloat(0.01f, 0.035f) : pSize;
	}

	DustCloud::DustCloud(string pVertShader, string pFragShader,	string pTexture, 
		size_t pMax, float pDist, float pRadius, 
		Circumstellar *pParent) : mMaxDust(pMax), mMaxDist(pDist), mMaxRadius(pRadius), mTargetZ(2.5f)
	{
		mDustTex = gl::Texture2d::create(loadImage(loadAsset(pTexture)));
		mShaderRender = gl::GlslProg::create(pParent->loadAsset(pVertShader), pParent->loadAsset(pFragShader));
		mShaderRender->uniform("u_Sampler", 0);
		mShaderRender->uniform("u_Max", pDist);

		mParticles.push_back(Dust(-1,-1,-1,-1,mMaxDist));
	}

	DustCloudRef DustCloud::create(string pVertShader, string pFragShader, string pTexture, size_t pMax, float pDist, float pRadius, Circumstellar *pParent)
	{
		return DustCloudRef(new DustCloud(pVertShader, pFragShader, pTexture, pMax, pDist, pRadius, pParent));
	}

	void DustCloud::setupTF(string pTFShader)
	{
		gl::GlslProg::Format tf;
		tf.feedbackFormat(GL_INTERLEAVED_ATTRIBS)
			
	}

	void DustCloud::Update()
	{
		if (mParticles.size() < mMaxDust)
		{
			for (int i = 0; i < 3;++i)
				mParticles.push_back(Dust(-1, -1, -1, -1, mMaxDist));
		}

		//	Transform Feedback goes here
		{

		}
	}

	void DustCloud::Draw(const CameraPersp &pCam)
	{
		vec3 right, up;
		pCam.getBillboardVectors(&right, &up);
		gl::color(Color::white());
		{
			gl::ScopedGlslProg shader(mShaderRender);
			gl::ScopedTextureBind tex(mDustTex, 0);
			
			for (const auto &d : mParticles)
				gl::drawBillboard(d.DrawPos, vec2(d.Size), 0.0f, right, up);
		}
	}

	void DustCloud::MouseSpawn(const vec2 &pMousePos, const vec2 &pWindowSize, const CameraPersp & pCam)
	{
		auto ray = pCam.generateRay(pMousePos, getWindowSize());

		float dist;
		float spawnDist = mMaxDist - 0.5f;
		vec3 rayPos;
		if (ray.calcPlaneIntersection(vec3(0, 0, spawnDist), vec3(0, 0, -1), &dist))
		{
			rayPos = ray.calcPosition(dist);
			auto angle = math<float>::atan2(rayPos.x, rayPos.y);
			if (angle < 0)
				angle += (2.0f*M_PI);

			auto rad = length(vec2(rayPos));

			mParticles.push_back(Dust(-1, -1, angle, rad, spawnDist, randFloat(0.05f,0.25f)));
		}
	}
} 