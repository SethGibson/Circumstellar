#include "Dust.h"
#include "CircumstellarApp.h"

namespace CS_Dust
{

	DustCloud::DustCloud(string pTFShader, string pVertShader, string pFragShader, Circumstellar *pParent)
	{
		mShaderTF = gl::GlslProg::create(pParent->loadAsset(pTFShader));
		mShaderRender = gl::GlslProg::create(pParent->loadAsset(pVertShader), pParent->loadAsset(pVertShader));
	}

	DustCloudRef DustCloud::create(string pTFShader, string pVertShader, string pFragShader, Circumstellar *pParent)
	{
		return DustCloudRef(new DustCloud(pTFShader, pVertShader, pFragShader, pParent));
	}

	void DustCloud::Update()
	{

	}

	void DustCloud::Draw()
	{
		gl::color(Color::white());
		{
			gl::ScopedGlslProg shader(mShaderRender);
			
		}
	}
} //CS_Dust