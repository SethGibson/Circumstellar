#include "BlackHole.h"
#include "cinder/app/App.h"
#include "cinder/ObjLoader.h"
#include "CircumstellarApp.h"

using namespace ci;

const float kTimeScale = 0.001f;
const float kColorScale = 0.01f;

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

	void BlackHole::SetColors(const vector<Color> &pColors)
	{
		mD1 = make_pair(vec3(pColors[0]), vec3(pColors[1]));
		mD2 = make_pair(vec3(pColors[2]), vec3(pColors[3]));
		mD3 = make_pair(vec3(pColors[4]), vec3(pColors[5]));
	}

	void BlackHole::Draw()
	{
		auto l1Factor = math<float>::sin(mElapsed*mV.x*kColorScale);
		l1Factor = (l1Factor + 1.0f)*0.5f;
		auto l2Factor = math<float>::cos(mElapsed*mV.y*kColorScale);
		l2Factor = (l1Factor + 1.0f)*0.5f;
		auto l3Factor = math<float>::sin(mElapsed*mV.z*kColorScale);
		l3Factor = (l1Factor + 1.0f)*0.5f;
		vec3 color1 = lerp<vec3>(mD1.first, mD1.second, l1Factor);
		vec3 color2 = lerp<vec3>(mD2.first, mD2.second, l2Factor);
		vec3 color3 = lerp<vec3>(mD3.first, mD3.second, l3Factor);

		gl::color(Color::white());
		{
			gl::ScopedGlslProg shader(mShader);
			gl::ScopedTextureBind tex(mTexMain, 0);
			mShader->uniform("TexOffset", mElapsed*kTimeScale);
			mShader->uniform("u_Intens",mK);
			mShader->uniform("u_Scales", mS);
			mShader->uniform("u_Speeds", mV);
			mShader->uniform("u_Color1", color1);
			mShader->uniform("u_Color2", color2);
			mShader->uniform("u_Color3", color3);
			
			gl::draw(mMesh);
		}
	}
}