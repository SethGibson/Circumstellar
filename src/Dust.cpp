#include "Dust.h"
#include "CircumstellarApp.h"

namespace CS_Dust
{
	Dust::Dust(float pLinear, float pAngular, float pAngle, float pRadius, float pDepth, float pSize=0, int pStartOffset=0, ColorA pTint=ColorA(1,1,1,1)) : Live(true)
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
		Size = pSize <= 0 ? randFloat(0.005f, 0.015f) : pSize;
		Start = pStartOffset;

		float x = math<float>::sin(AngleRadii.x)*AngleRadii.y;
		float y = math<float>::cos(AngleRadii.x)*AngleRadii.y;
		DrawPos = vec3(x, y, pDepth);
		Tint = pTint;
		Tint.a = randFloat(0.25f, 1.f);
		Alpha = Tint.a;
	}

	DustCloud::DustCloud(size_t pMax, float pDist, float pRadius, const CameraPersp  &pCam) :
		mMaxDust(pMax), mMaxDist(pDist), mCutoffDist(1.75f), mMaxRadius(pRadius), mTargetZ(2.5f), mCam(pCam)
	{
		mDustTex = gl::Texture2d::create(loadImage(loadAsset("textures/TX_Sprite.png")));
		
		mShaderRender = gl::GlslProg::create(loadAsset("shaders/dust_cloud.vert"), loadAsset("shaders/dust_cloud.frag"));
		mShaderRender->uniform("u_SamplerRGB", 0);
		mShaderRender->uniform("u_Max", pDist);

		setupDust();
	}

	DustCloudRef DustCloud::create(size_t pMax, float pDist, float pRadius, const CameraPersp  &pCam)
	{
		return DustCloudRef(new DustCloud(pMax, pDist, pRadius, pCam));
	}

	void DustCloud::setupDust()
	{
		//add some dust first
		auto count = randInt(1, mMaxDust / 4);
		for (int i = 0; i < count;++i)
			mParticles.push_back(Dust(-1, -1, -1, -1, mMaxDist));

		geom::BufferLayout attribs;
		attribs.append(geom::CUSTOM_0, 3, sizeof(Dust), offsetof(Dust, DrawPos), 1);
		attribs.append(geom::CUSTOM_1, 4, sizeof(Dust), offsetof(Dust, Tint), 1);
		attribs.append(geom::CUSTOM_2, 1, sizeof(Dust), offsetof(Dust, Size), 1);

		mDustData = gl::Vbo::create(GL_ARRAY_BUFFER, mParticles, GL_DYNAMIC_DRAW);
		auto mesh = gl::VboMesh::create(geom::Plane().axes(vec3(1, 0, 0), vec3(0, 1, 0)).size(vec2(1)));
		mesh->appendVbo(attribs, mDustData);
		mDustDraw = gl::Batch::create(mesh, mShaderRender, { {geom::CUSTOM_0, "i_Position"},{ geom::CUSTOM_1, "i_Tint" },{geom::CUSTOM_2, "i_Size"} });
	}

	void DustCloud::Update()
	{
		if (mParticles.size() < mMaxDust)
		{
			for (int i = 0; i < 3;++i)
				mParticles.push_back(Dust(-1, -1, -1, -1, mMaxDist));
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
					if (d->Distance.y > mCutoffDist)
					{
						d->Distance.y -= d->Speed.x;

						d->AngleRadii.x -= d->Speed.y;
						d->AngleRadii.z = lerp<float>(0.0f, d->AngleRadii.y, d->Distance.y / d->Distance.x);

						float x = math<float>::sin(d->AngleRadii.x)*d->AngleRadii.z;
						float y = math<float>::cos(d->AngleRadii.x)*d->AngleRadii.z;
						d->DrawPos = vec3(x, y, d->Distance.y);

						d->Tint.a = lmap<float>(d->Distance.y, mCutoffDist, d->Distance.x, 0.0f, d->Alpha);
					}
					else
						d->Live = false;
				}
				++d;
			}
		}

		mDustData->bufferData(mParticles.size()*sizeof(Dust), mParticles.data(), GL_DYNAMIC_DRAW);
	}

	void DustCloud::Draw()
	{
		gl::color(Color::white());
		{
			gl::ScopedTextureBind tex(mDustTex, 0);
			mDustDraw->drawInstanced(mParticles.size());
		}
	}

	void DustCloud::MouseSpawn(const vec2 &pMousePos)
	{
		auto ray = mCam.generateRay(pMousePos, getWindowSize());

		float dist;
		float spawnDist = mMaxDist - 0.1f;
		if (ray.calcPlaneIntersection(vec3(0, 0, spawnDist), vec3(0, 0, -1), &dist))
		{
			auto rayPos = ray.calcPosition(dist);
			auto angle = math<float>::atan2(rayPos.x, rayPos.y);
			if (angle < 0)
				angle += (2.0f*M_PI);

			auto rad = length(vec2(rayPos));
			
			float r = randFloat(0.0f, 1.0f);
			float g = 0.0f, b = 0.0f;
			if(r>0.25f)
				g = randFloat(0.25f, 1.0f);
			else {
				r = 0.0f;
				b = randFloat(0.5f, 1.0f);
				g = randFloat(0.0f, 0.25f);
			}
			mParticles.push_back(Dust(-1, -1, angle, rad, spawnDist, randFloat(0.0065f,0.02f), 120
				, ColorA(r,g,b, randFloat(0.5f,0.95f))));
		}
	}

	void DustCloud::CapturePoints(const Surface8uRef & pSurface)
	{
		auto iter = pSurface->getIter();
		auto pxScale = vec2(getWindowSize()) / vec2(320.0f, 240.0f);

		float framePad = 0;
		while (iter.line())
		{
			while (iter.pixel())
			{
				if (iter.r() > 0 || iter.g() > 0 || iter.b() > 0)
				{
					if (iter.x() % 2 == 0 && iter.y() % 2 == 0) {
						auto x = getWindowWidth() -(iter.x()*pxScale.x);
						auto y = iter.y()*pxScale.y;
						auto ray = mCam.generateRay(vec2(x, y), getWindowSize());

						float dist;
						float spawnDist = mMaxDist - 0.1f;
						if (ray.calcPlaneIntersection(vec3(0, 0, spawnDist), vec3(0, 0, -1), &dist))
						{
							auto rayPos = ray.calcPosition(dist);
							auto angle = math<float>::atan2(rayPos.x, rayPos.y);
							if (angle < 0)
								angle += (2.0f*M_PI);

							auto rad = length(vec2(rayPos));

							mParticles.push_back(Dust(-1, -1, angle, rad, spawnDist, randFloat(0.0065f, 0.015f), 60+framePad
								, ColorA(iter.r() / 255.f, iter.g() / 255.f, iter.b() / 255.f, randFloat(0.5f,0.80f))));
							framePad += 0.025f;
							
						}
					}
				}
			}
		}
	}
} 