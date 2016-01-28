#include "CiLibRS.h"

namespace CiLibRS {

RSStream::RSStream()
{
}

RSStream::RSStream(rs::stream pType, rs::format pFormat, ivec3 pRes) :
	StreamType(pType), StreamFormat(pFormat), StreamRes(pRes)
{

}

RSStream::RSStream(rs::stream pType, rs::format pFormat, int pWidth, int pHeight, int pFPS)
{
	RSStream(pType, pFormat, ivec3(pWidth, pHeight, pFPS));
}

RSDeviceRef RSDevice::create(RSStream pColor, RSStream pDepth)
{
	return RSDeviceRef(new RSDevice(pColor, pDepth));
}

RSDevice::RSDevice()
{
}

RSDevice::RSDevice(RSStream pColor, RSStream pDepth = RSStream())
{
	rs::context ctx;
	if (ctx.get_device_count() == 0) {
		throw std::runtime_error("No device detected. Is it plugged in?");
	}

	mDevice = ctx.get_device(0);

	mDevice->enable_stream(pColor.StreamType, pColor.StreamRes.x, pColor.StreamRes.y, pColor.StreamFormat, pColor.StreamRes.z);
	mColorDims = ivec2(pColor.StreamRes.x, pColor.StreamRes.y);

	switch (pColor.StreamFormat) {
	case rs::format::bgr8:
		mChanOrder = SurfaceChannelOrder::BGR;
		mBPP_RGB = 3;
		break;
	case rs::format::rgb8:
		mChanOrder = SurfaceChannelOrder::RGB;
		mBPP_RGB = 3;
		break;
	case rs::format::bgra8:
		mChanOrder = SurfaceChannelOrder::BGRA;
		mBPP_RGB = 4;
	case rs::format::rgba8:
		mChanOrder = SurfaceChannelOrder::RGBA;
		mBPP_RGB = 4;
		break;
	}
	mColorFrame = Surface8u::create(mColorDims.x, mColorDims.y, mBPP_RGB == 4 ? true : false, mChanOrder);
	mHasColor = true;

	if (pDepth.StreamType == rs::stream::depth) {
		mDevice->enable_stream(pDepth.StreamType, pDepth.StreamRes.x, pDepth.StreamRes.y, pDepth.StreamFormat, pDepth.StreamRes.z);
		mDepthDims = ivec2(pDepth.StreamRes.x, pDepth.StreamRes.y);
		switch (pDepth.StreamFormat) {
		case rs::format::y16:
		case rs::format::z16:
			mBPP_D = 2;
			break;
		case rs::format::y8:
			mBPP_D = 1;
			break;
		}
		mDepthFrame = Channel16u::create(mDepthDims.x, mDepthDims.y);
		mHasDepth = true;
	}
	else {
		mHasDepth = false;
	}
}

void RSDevice::start()
{
	if (mDevice != nullptr) {
		if (!mDevice->is_streaming()) {
			mDevice->start();
		}
	}
}

void RSDevice::stop()
{
	if (mDevice != nullptr) {
		mDevice->stop();
	}
}

void RSDevice::update()
{
	if (!mDevice->is_streaming()) {
		mDevice->start();
	}

	mDevice->wait_for_frames();

	if (mHasColor) {
		mColorFrame = Surface8u::create((uint8_t *)mDevice->get_frame_data(rs::stream::color), mColorDims.x, mColorDims.y, mColorDims.x*mBPP_RGB, mChanOrder);
	}

	if (mHasDepth) {
		mDepthFrame = Channel16u::create(mDepthDims.x, mDepthDims.y, mDepthDims.x*mBPP_D, 1, (uint16_t *)mDevice->get_frame_data(rs::stream::depth));
	}
}

} //CiLibRS {}