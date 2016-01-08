#include "BlackHole.h"
#include "cinder/app/App.h"
#include "cinder/ObjLoader.h"
#include "CircumstellarApp.h"

using namespace ci;

static float S_TIME_SCALE = 0.001f;
namespace CS_Dust
{
	
	BlackHoleRef BlackHole::create(string pObjFile, string pVertShader, string pFragShader, string pTexture, Circumstellar *pParent)
	{
		return BlackHoleRef(new BlackHole(pObjFile, pVertShader, pFragShader, pTexture, pParent));
	}

	BlackHole::BlackHole(string pObjFile, string pVertShader, string pFragShader, string pTexture, Circumstellar *pParent)
	{
		mMesh = gl::VboMesh::create(ObjLoader(pParent->loadAsset("meshes/MSH_CS.obj")));
		mTexMain = gl::Texture2d::create(loadImage(pParent->loadAsset("textures/TX_BlackHole.tif")));
		mTexMain->setWrap(GL_REPEAT, GL_REPEAT);

		mShader = gl::GlslProg::create(pParent->loadAsset("shaders/black_hole.vert"), pParent->loadAsset("shaders/black_hole.frag"));
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