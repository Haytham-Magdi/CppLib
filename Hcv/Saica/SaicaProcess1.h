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

		class SaicaProcess1 : FRM_Object
		{
		public:

			SaicaProcess1(F32ImageRef a_srcImg);

			F32ImageRef GetSrcImg()
			{
				return m_srcImg;
			}

		protected:

			void Prepare();

		protected:

			F32ImageRef m_srcImg;
		};
	};


	typedef Hcpl::ObjRef< Ns_Saica::SaicaProcess1 > SaicaProcess1Ref;

}