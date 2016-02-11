#pragma once
#include "cinder/gl/gl.h"

using namespace std;
using namespace ci;

class Circumstellar;

typedef std::pair<vec3, vec3> color_rng;

namespace CS_Dust
{
	class BlackHole;
	typedef shared_ptr<BlackHole> BlackHoleRef;

	class BlackHole
	{
	public:
		static BlackHoleRef create();
		void Draw();

		void SetK(const vec4 &pK) { mK = pK; }
		void SetV(const vec3 &pV) { mV = pV; }
		void SetS(const vec3 &pS) { mS = pS; }
		void SetColors(const vector<Color> &pColors);
		void SetElapsed(const float &pElapsed) { mElapsed = pElapsed; }
	protected:
		BlackHole();

	private:
		gl::GlslProgRef			mShader;
		gl::Texture2dRef		mTexMain;
		gl::VboMeshRef			mMesh;

		float	mElapsed;
		vec4	mK;
		vec3	mV, mS;

		color_rng	mD1, mD2, mD3;
	};
}
