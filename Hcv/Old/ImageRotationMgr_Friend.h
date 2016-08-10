#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Cpp\Math\mathLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>

#include <Lib\Hcv\ImageLineItr.h>
#include <Lib\Hcv\ImageLineItrProvider.h>
#include <Lib\Hcv\ImgRotationMgr.h>


namespace Hcv
{
	//using namespace Hcpl::Math;


	class ImageRotationMgr_Friend : public ImgRotationMgr
	{
	public:
		
		ImageRotationMgr_Friend( F32ImageRef a_srcImg, float a_angDig );
		

	protected:


	};


	typedef Hcpl::ObjRef< ImageRotationMgr_Friend > ImageRotationMgr_FriendRef;

}