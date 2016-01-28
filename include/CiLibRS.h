#pragma once
#include "cinder/app/App.h"
#include "librealsense/rs.hpp"

using namespace std;
using namespace ci;

namespace CiLibRS
{
class RSDevice;
typedef shared_ptr<RSDevice> RSDeviceRef;

struct RSStream
{
	rs::stream	StreamType;
	rs::format	StreamFormat;
	ivec3		StreamRes;	// x=width, y=height,z=fps

	RSStream();
	RSStream(rs::stream pType, rs::format pFormat, ivec3 pRes);
	RSStream(rs::stream pType, rs::format pFormat, int pWidth, int pHeight, int pFPS);
};

class RSDevice {
  public:
	  static RSDeviceRef create(RSStream pColor, RSStream pDepth);
	  
	  void start();
	  void stop();
	  void update();
	  const ivec2&			getColorDims() { return mColorDims; }
	  const ivec2&			getDepthDims() { return mDepthDims; }
	  const Surface8uRef	getColorFrame() { return mColorFrame; }
	  const Channel16uRef	getDepthFrame() { return mDepthFrame; }

  protected:
	  RSDevice();
	  RSDevice(RSStream pColor, RSStream pDepth);
  private:
	  rs::device *mDevice;

	  bool	mHasDepth,
			mHasColor;

	  ivec2	mColorDims,
			mDepthDims;

	  Surface8uRef		mColorFrame;
	  Channel16uRef		mDepthFrame;

	  SurfaceChannelOrder	mChanOrder;
	  int					mBPP_RGB;	//RGB Bytes Per Pixel
	  int					mBPP_D;		//Depth Bytes Per Pixel
};

} //CiLibRS