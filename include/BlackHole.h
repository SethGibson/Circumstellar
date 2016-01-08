#pragma once
#include "cinder/app/App.h"
#include "cinder/gl/gl.h"

using namespace std;
using namespace ci;

class Circumstellar;

namespace CS_Dust
{
	class BlackHole;
	typedef std::shared_ptr<BlackHole> BlackHoleRef;

	class BlackHole
	{
	public:
		static BlackHoleRef create(string pObjFile, string pVertShader, string pFragShader, string pTexture, Circumstellar *pParent);
		void Update();
		void Draw(float pElapsed);

	protected:
		BlackHole() {}
		BlackHole(string pObjFile, string pVertShader, string pFragShader, string pTexture, Circumstellar *pParent);

	private:
		gl::GlslProgRef			mShader;
		gl::Texture2dRef		mTexMain;
		gl::VboMeshRef			mMesh;
	};
}
