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


namespace Hcv
{
	namespace Ns_Binarization
	{

		class AngleDirMgrColl_Context : FRM_Object
		{
		public:

			TempImageAccessor_REF(PixelStandevInfo) m_standevInfoImg;
			TempImageAccessor_REF(ConflictInfo_Ex) m_conflictInfoImg;
			F32ImageAccessor1C_Ref m_wideConflictDiff_Img;
			F32VectorValImageAcc_4C_Ref m_avgPStandev_InrWide_Img;

			F32ImageAccessor3C_Ref m_org_Img;

			//TempImageAccessor_REF(int) m_standevInfoImg;
			//PixelStandevInfo psi;

		};

		typedef Hcpl::ObjRef< AngleDirMgrColl_Context > AngleDirMgrColl_Context_Ref;
	};

}