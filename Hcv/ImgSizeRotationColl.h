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

#include <Lib\Hcv\ImgSizeRotation.h>
//#include <Lib\Hcv\Saica\ImgAngleDirMgr.h>


namespace Hcv
{
	namespace Ns_Saica
	{

		class ImgSizeRotationColl : FRM_Object
		{
		public:

			ImgSizeRotationColl(CvSize a_srcSiz, int a_nofRots);

			ImgSizeRotationRef GetRotAt(int a_nRotIdx)
			{
				return m_rotMgrArr[a_nRotIdx];
			}

			int GetNofRots()
			{
				return m_rotMgrArr.GetSize();
			}

			CvSize GetSrcImgSiz()
			{
				return m_srcSiz;
			}

		protected:

			//void Prepare();

		protected:

			//F32ImageRef m_srcImg;
			CvSize m_srcSiz;

			int m_nofRots;
			FixedVector< ImgSizeRotationRef > m_rotMgrArr;
		};

		typedef Hcpl::ObjRef< ImgSizeRotationColl > ImgSizeRotationCollRef;
	};

}