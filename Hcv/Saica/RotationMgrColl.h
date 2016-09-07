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

#include <Lib\Hcv\ImgRotationMgr.h>
#include <Lib\Hcv\Saica\ImgAngleDirMgr.h>


namespace Hcv
{
	namespace Ns_Saica
	{

		class RotationMgrColl : FRM_Object
		{
		public:

			RotationMgrColl(F32ImageRef a_srcImg, int a_nofRots);

			ImgRotationMgr_2_Ref GetRotAt(int a_nRotIdx)
			{
				return m_rotMgrArr[a_nRotIdx];
			}

			int GetNofRots()
			{
				return m_rotMgrArr.GetSize();
			}

			F32ImageRef GetSrcImg()
			{
				return m_srcImg;
			}

		protected:

			//void Prepare();

		protected:

			F32ImageRef m_srcImg;

			int m_nofRots;
			FixedVector< ImgRotationMgr_2_Ref > m_rotMgrArr;
		};

		typedef Hcpl::ObjRef< RotationMgrColl > RotationMgrCollRef;
	};

}