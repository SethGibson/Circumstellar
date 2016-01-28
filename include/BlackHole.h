#pragma once
#include "cinder/gl/gl.h"

using namespace std;
using namespace ci;

class Circumstellar;

namespace CS_Dust
{
	class BlackHole;
	typedef shared_ptr<BlackHole> BlackHoleRef;

	class BlackHole
	{
	public:
		static BlackHoleRef create();
		void Update();
		void Draw(float pElapsed);

	protected:
		BlackHole();

	private:
		gl::GlslProgRef			mShader;
		gl::Texture2dRef		mTexMain;
		gl::VboMeshRef			mMesh;
	};
}
