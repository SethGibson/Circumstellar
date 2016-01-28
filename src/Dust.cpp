#include "Dust.h"
#include "CircumstellarApp.h"

namespace CS_Dust
{
	Dust::Dust(float pLinear, float pAngular, float pAngle, float pRadius, float pDepth, float pSize=0, int pStartOffset=0, Color pTint=Color::white()) : Live(true)
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
		Alpha = 1.0f;

		float x = math<float>::sin(AngleRadii.x)*AngleRadii.y;
		float y = math<float>::cos(AngleRadii.x)*AngleRadii.y;
		DrawPos = vec3(x, y, pDepth);
		Tint = pTint;

	}

	DustCloud::DustCloud(size_t pMax, float pDist, float pRadius, const CameraPersp & pCam) :
		mMaxDust(pMax), mMaxDist(pDist), mCutoffDist(1.75f), mMaxRadius(pRadius), mTargetZ(2.5f), mCamera(pCam)
	{
		mTexDust = gl::Texture2d::create(loadImage(loadAsset("textures/TX_Sprite.png")));
		
		mShaderRender = gl::GlslProg::create(loadAsset("shaders/dust_cloud.vert"), loadAsset("shaders/dust_cloud.frag"));
		mShaderRender->uniform("u_Sampler", 0);
		mShaderRender->uniform("u_Max", pDist);

		setupDust();
		setupLibRS();
	}

	DustCloudRef DustCloud::create(size_t pMax, float pDist, float pRadius, const CameraPersp & pCam)
	{
		return DustCloudRef(new DustCloud(pMax, pDist, pRadius, pCam));
	}

	void DustCloud::setupDust()
	{
		mPlaneDist = mMaxDist - 0.1f;
		//add some dust first
		auto count = randInt(1, mMaxDust / 4);
		for (int i = 0; i < count;++i)
			mParticles.push_back(Dust(-1, -1, -1, -1, mMaxDist));

		geom::BufferLayout attribs;
		attribs.append(geom::CUSTOM_0, 3, sizeof(Dust), offsetof(Dust, DrawPos), 1);
		attribs.append(geom::CUSTOM_1, 3, sizeof(Dust), offsetof(Dust, Tint), 1);
		attribs.append(geom::CUSTOM_2, 1, sizeof(Dust), offsetof(Dust, Size), 1);
		attribs.append(geom::CUSTOM_3, 1, sizeof(Dust), offsetof(Dust, Alpha), 1);

		mDustData = gl::Vbo::create(GL_ARRAY_BUFFER, mParticles, GL_DYNAMIC_DRAW);
		auto mesh = gl::VboMesh::create(geom::Plane().axes(vec3(1, 0, 0), vec3(0, 1, 0)).size(vec2(1)));
		mesh->appendVbo(attribs, mDustData);
		mDustDraw = gl::Batch::create(mesh, mShaderRender, { {geom::CUSTOM_0, "i_Position"},{ geom::CUSTOM_1, "i_Tint" },{geom::CUSTOM_2, "i_Size"},{ geom::CUSTOM_3, "i_Alpha" } });

		// setup planes
		auto ray = mCamera.generateRay(vec2(0), getWindowSize());
		float dist;
		if (ray.calcPlaneIntersection(vec3(0, 0, mPlaneDist), vec3(0, 0, -1), &dist))
		{
			auto rayPos = ray.calcPosition(dist);
			mDustPlaneMin.x = rayPos.x;
			mDustPlaneMin.y = rayPos.y;
		}
		
		ray = mCamera.generateRay(vec2(getWindowWidth(),getWindowHeight()), getWindowSize());
		if (ray.calcPlaneIntersection(vec3(0, 0, mPlaneDist), vec3(0, 0, -1), &dist))
		{
			auto rayPos = ray.calcPosition(dist);
			mDustPlaneMax.x = rayPos.x;
			mDustPlaneMax.y = rayPos.y;
		}
	}

	void DustCloud::setupLibRS()
	{
		CiLibRS::RSStream color(rs::stream::color,rs::format::bgr8, ivec3(640,480,30));
		CiLibRS::RSStream depth(rs::stream::depth, rs::format::z16, ivec3(480, 360, 30));

		mRS = CiLibRS::RSDevice::create(color, depth);
		mRS->start();

		auto dd = mRS->getDepthDims();
		for (int y = 0; y < dd.y; y += 4)
		{
			for (int x = 0; x < dd.x; x += 4)
			{
				mDepthPoints.push_back(vec3(x, y, mPlaneDist));
			}
		}
		mShaderPoints = gl::GlslProg::create(loadAsset("shaders/depth_dust.vert"), loadAsset("shaders/depth_dust.frag"));
		mPointData = gl::Vbo::create(GL_ARRAY_BUFFER, mDepthPoints, GL_DYNAMIC_DRAW);
		
		geom::BufferLayout attribs;
		attribs.append(geom::POSITION, 3, 0, 0);

		auto mesh = gl::VboMesh::create(mDepthPoints.size(), GL_POINTS, { { attribs, mDustData } });
		mPointDraw = gl::Batch::create(mesh, mShaderPoints);
	}

	void DustCloud::getDrawPoints()
	{
		auto depthChan = mRS->getDepthFrame();
		auto iter = depthChan->getIter();

		auto dd = mRS->getDepthDims();
		mDepthPoints.clear();
		while (iter.line())
		{
			while (iter.pixel())
			{
				auto d = iter.v();
				if (d < mDepthCutoff)
				{
					auto x = lmap<float>(iter.x(), 0, dd.x, mDustPlaneMin.x, mDustPlaneMax.x);
					auto y = lmap<float>(iter.x(), 0, dd.y, mDustPlaneMin.y, mDustPlaneMax.y);
					mDepthPoints.push_back(vec3(x, y, mPlaneDist));
				}
			}
		}

		mDustData->bufferData(mDepthPoints.size()*sizeof(vec3), mDepthPoints.data(), GL_DYNAMIC_DRAW);
	}
	
	void DustCloud::capturePoints()
	{
		auto max = mDepthPoints.size() / 2;
		for (int i = 0; i < max; ++i)
		{
			auto id = randInt(0, mDepthPoints.size() - 1);
			auto pos = mDepthPoints[id];

			auto angle = math<float>::atan2(pos.x, pos.y);
			if (angle < 0)
				angle += (2.0f*M_PI);

			auto rad = length(vec2(pos));

			int start = (int)lmap<float>(pos.y, mDustPlaneMin.y, mDustPlaneMax.y, 60.0f, 240.0f);
			mParticles.push_back(Dust(-1, -1, angle, rad, mPlaneDist, randFloat(0.0065f, 0.02f), start));
		}
	}

	void DustCloud::Update()
	{
		mRS->update();
		if (mCaptureTime == 0)
		{
			capturePoints();
			mCaptureTime = 900;
		}
		else
			mCaptureTime -= 1;

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

						d->Alpha = lmap<float>(d->Distance.y, mCutoffDist, d->Distance.x, 0.0f, 1.0f);
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
			gl::ScopedTextureBind tex(mTexDust, 0);
			mDustDraw->drawInstanced(mParticles.size());
		}
	}

	void DustCloud::MouseSpawn(const vec2 &pMousePos, const vec2 &pWindowSize)
	{
		auto ray = mCamera.generateRay(pMousePos, getWindowSize());

		float dist;
		if (ray.calcPlaneIntersection(vec3(0, 0, mPlaneDist), vec3(0, 0, -1), &dist))
		{
			auto rayPos = ray.calcPosition(dist);
			auto angle = math<float>::atan2(rayPos.x, rayPos.y);
			if (angle < 0)
				angle += (2.0f*M_PI);

			auto rad = length(vec2(rayPos));
			
			auto r = 1.0f;
			auto g = lmap<float>(pMousePos.x, 0.0f, 1280.0f, 0.0f, 1.0f);
			auto b = lmap<float>(pMousePos.y, 0.0f, 720.0f, 0.0f, 1.0f);

			int start = (int)lmap<float>(pMousePos.y, 0.0f, 720.0f, 60.0f, 240.0f);
			mParticles.push_back(Dust(-1, -1, angle, rad, mPlaneDist, randFloat(0.0065f,0.02f), start, Color(r,g,b)));
		}
	}
} 