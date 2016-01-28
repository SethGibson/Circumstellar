#include "BlackHole.h"
#include "cinder/app/App.h"
#include "cinder/ObjLoader.h"
#include "CircumstellarApp.h"

using namespace ci;

static float S_TIME_SCALE = 0.001f;
namespace CS_Dust
{
	
	BlackHoleRef BlackHole::create()
	{
		return BlackHoleRef(new BlackHole());
	}

	BlackHole::BlackHole()
	{
		mMesh = gl::VboMesh::create(ObjLoader(loadAsset("meshes/MSH_CS.obj")));
		mTexMain = gl::Texture2d::create(loadImage(loadAsset("textures/TX_BlackHole.tif")));
		mTexMain->setWrap(GL_REPEAT, GL_REPEAT);

		mShader = gl::GlslProg::create(loadAsset("shaders/black_hole.vert"), loadAsset("shaders/black_hole.frag"));
		mShader->uniform("u_samplerRGBA", 0);
	}

	void BlackHole::Update()
	{
	}

	void BlackHole::Draw(float pElapsed)
	{
		gl::color(Color::white());
		{
			gl::ScopedGlslProg shader(mShader);
			gl::ScopedTextureBind tex(mTexMain, 0);
			mShader->uniform("TexOffset", pElapsed*S_TIME_SCALE);
			gl::draw(mMesh);
		}
	}
}