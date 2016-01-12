#include "Dust.h"
#include "CircumstellarApp.h"

namespace CS_Dust
{
	Dust::Dust(float pRadius, float pDist) : Live(true)
	{
		Vel.x = randFloat(0.005f, 0.01f);
		Vel.y = randFloat(0.01f, 0.05f);
		Size = randFloat(0.01f, 0.035f);
		Angle.x = randFloat(0.0f, 2.f*M_PI);
		Angle.y = pRadius;
		Angle.z = pRadius;

		Pos.x = math<float>::cos(Angle.x)*Angle.z;
		Pos.y = math<float>::sin(Angle.x)*Angle.z;
		Pos.z = pDist;
	}

	void Dust::Step(float pDist)
	{
		Pos.z -= Vel.x;
		if (Pos.z > 0.05f)
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
		Circumstellar *pParent) : mMaxDust(pMax), mMaxDist(pDist), mMaxRadius(pRadius)
	{
		mDustTex = gl::Texture2d::create(loadImage(loadAsset(pTexture)));
		mShaderTF = gl::GlslProg::create(pParent->loadAsset(pTFShader));
		mShaderRender = gl::GlslProg::create(pParent->loadAsset(pVertShader), pParent->loadAsset(pFragShader));
		mShaderRender->uniform("u_Sampler", 0);
		mShaderRender->uniform("u_Max", pDist);

		mParticles.push_back(Dust(pRadius, pDist));
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
				mParticles.push_back(Dust(mMaxRadius, mMaxDist));
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
		{
			gl::ScopedGlslProg shader(mShaderRender);
			gl::ScopedTextureBind tex(mDustTex, 0);
			
			for (const auto &d : mParticles)
			{
				gl::drawBillboard(d.Pos, vec2(d.Size), -d.Angle.x, right, up);
			}
		}
	}
} //CS_Dust