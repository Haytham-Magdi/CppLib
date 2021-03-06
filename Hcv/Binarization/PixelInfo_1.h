#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Cpp\Math\mathLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>

//#include <Lib\Hcv\ImageLineItrProvider.h>
#include <Lib\Hcv\ImageItrMgr.h>
#include <Lib\Hcv\TempImageAccessor.h>
#include <Lib\Hcv\PixelStandevInfo.h>
#include <Lib\Hcv\ConflictInfo.h>

#include <Lib\Hcv\Binarization\ImgAngleDirMgr.h>
#include <Lib\Hcv\Binarization\RotationMgrColl.h>
#include <Lib\Hcv\ConflictInfo.h>


namespace Hcv
{
	namespace Ns_Binarization
	{

		class PixelInfo_1 : public MultiListQueMember< PixelInfo_1 >
		{
		public:

			int X;
			int Y;

			ConflictInfo_Ex * pConflictInfo;

			float Val_WideOutStandev;

			bool IsPushed;

		};

		//typedef Hcpl::ObjRef< PixelInfo_1 > PixelInfo_1_Ref;
	};

}