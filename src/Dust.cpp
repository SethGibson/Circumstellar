#include "Dust.h"
#include "CircumstellarApp.h"

namespace CS_Dust
{
	Dust::Dust(float pLinear, float pAngular, float pAngle, float pRadius, float pDepth, float pSize=0, int pStartOffset=0) : Live(true)
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
		Size = pSize <= 0 ? randFloat(0.001f, 0.0035f) : pSize;
		Start = pStartOffset;
		Alpha = 1.0f;

		float x = math<float>::sin(AngleRadii.x)*AngleRadii.y;
		float y = math<float>::cos(AngleRadii.x)*AngleRadii.y;
		DrawPos = vec3(x, y, pDepth);

	}

	DustCloud::DustCloud(string pVertShader, string pFragShader,	string pTexture, 
		size_t pMax, float pDist, float pRadius, 
		Circumstellar *pParent) : mMaxDust(pMax), mMaxDist(pDist), mMaxRadius(pRadius), mTargetZ(2.5f)
	{
		mDustTex = gl::Texture2d::create(loadImage(loadAsset(pTexture)));
		mShaderRender = gl::GlslProg::create(pParent->loadAsset(pVertShader), pParent->loadAsset(pFragShader));
		mShaderRender->uniform("u_Sampler", 0);
		mShaderRender->uniform("u_Max", pDist);

		setupDust();
	}

	DustCloudRef DustCloud::create(string pVertShader, string pFragShader, string pTexture, size_t pMax, float pDist, float pRadius, Circumstellar *pParent)
	{
		return DustCloudRef(new DustCloud(pVertShader, pFragShader, pTexture, pMax, pDist, pRadius, pParent));
	}

	void DustCloud::setupDust()
	{
		//add some dust first
		auto count = randInt(1, mMaxDust / 4);
		for (int i = 0; i < count;++i)
			mParticles.push_back(Dust(-1, -1, -1, -1, mMaxDist));

		geom::BufferLayout attribs;
		attribs.append(geom::CUSTOM_0, 3, sizeof(Dust), offsetof(Dust, DrawPos), 1);
		attribs.append(geom::CUSTOM_1, 1, sizeof(Dust), offsetof(Dust, Size), 1);
		attribs.append(geom::CUSTOM_2, 1, sizeof(Dust), offsetof(Dust, Alpha), 1);

		mDustData = gl::Vbo::create(GL_ARRAY_BUFFER, mParticles, GL_DYNAMIC_DRAW);
		auto mesh = gl::VboMesh::create(geom::Plane().axes(vec3(1, 0, 0), vec3(0, 1, 0)).size(vec2(8)));
		mesh->appendVbo(attribs, mDustData);
		mDustDraw = gl::Batch::create(mesh, mShaderRender, { {geom::CUSTOM_0, "i_Position"},{geom::CUSTOM_1, "i_Size"},{ geom::CUSTOM_2, "i_Alpha" } });
	}

	void DustCloud::Update()
	{
		if (mParticles.size() < mMaxDust)
		{
			for (int i = 0; i < 3;++i)
				mParticles.push_back(Dust(-1, -1, -1, -1, mMaxDist,0,0));
		}

		for (auto d = begin(mParticles); d != end(mParticles);)
		{
			if (!d->Live)
				d = mParticles.erase(d);
			else
			{
				if (d->Start > 0)
					d->Start -= 1;
				else
				{
					if (d->Distance.y > 2.0f)
					{
						d->Distance.y -= d->Speed.x;

						d->AngleRadii.x -= d->Speed.y;
						d->AngleRadii.z = lerp<float>(0.0f, d->AngleRadii.y, d->Distance.y / d->Distance.x);

						float x = math<float>::sin(d->AngleRadii.x)*d->AngleRadii.z;
						float y = math<float>::cos(d->AngleRadii.x)*d->AngleRadii.z;
						d->DrawPos = vec3(x, y, d->Distance.y);

						d->Alpha = lmap<float>(d->Distance.y, 2.0, d->Distance.x, 0.0f, 1.0f);
					}
					else
						d->Live = false;
				}
				++d;
			}
		}

		mDustData->bufferData(mParticles.size()*sizeof(Dust), mParticles.data(), GL_DYNAMIC_DRAW);
	}

	void DustCloud::Draw(const CameraPersp &pCam)
	{
		gl::color(Color::white());
		{
			gl::ScopedTextureBind tex(mDustTex, 0);
			mDustDraw->drawInstanced(mParticles.size());
		}
	}

	void DustCloud::MouseSpawn(const vec2 &pMousePos, const vec2 &pWindowSize, const CameraPersp & pCam)
	{
		auto ray = pCam.generateRay(pMousePos, getWindowSize());

		float dist;
		float spawnDist = mMaxDist - 0.1f;
		vec3 rayPos;
		if (ray.calcPlaneIntersection(vec3(0, 0, spawnDist), vec3(0, 0, -1), &dist))
		{
			rayPos = ray.calcPosition(dist);
			auto angle = math<float>::atan2(rayPos.x, rayPos.y);
			if (angle < 0)
				angle += (2.0f*M_PI);

			auto rad = length(vec2(rayPos));

			mParticles.push_back(Dust(-1, -1, angle, rad, spawnDist, randFloat(0.001f,0.0035f), randInt(60,180)));
		}
	}
} 