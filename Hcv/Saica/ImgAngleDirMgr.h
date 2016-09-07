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

#include <Lib\Hcv\ImgRotationMgr_2.h>
#include <Lib\Hcv\Saica\ImgAngleDirMgr.h>


namespace Hcv
{
	namespace Ns_Saica
	{

		class ImgAngleDirMgr : FRM_Object
		{
		public:

			ImgAngleDirMgr(ImgRotationMgr_2_Ref a_imgRotMgr, F32ImageRef a_rotMagSqrImg, char a_hv, int a_nAprLen_1, int a_nAprLen_2);

			F32ImageRef GetSrcImg()
			{
				return m_srcImg;
			}

		protected:

			//void Prepare();

		protected:


			F32ImageRef m_srcImg;
			F32ImageRef m_rotImg;
			F32ImageRef m_rotMagSqrImg;

			//const char m_hv;
			char m_hv;

			int m_nAprLen_1; 
			int m_nAprLen_2;

			ImgRotationMgr_2_Ref m_imgRotMgr;

		};
	
		typedef Hcpl::ObjRef< ImgAngleDirMgr > ImgAngleDirMgrRef;
	};



}