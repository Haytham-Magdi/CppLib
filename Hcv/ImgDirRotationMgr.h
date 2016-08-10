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



namespace Hcv
{

	class ImgRotationMgr  : FRM_Object
	{
	public:

		ImgRotationMgr( F32ImageRef a_srcImg, float a_angDig );


	protected:

		void Prepare();
		
		void PrepareResImg();

		int AddRound( int a_num );

		int AddRoundByMin( int a_num );
		
		//void PrepareProvArr();
		void PrepareImageItrMgr();
		


	protected:

		F32ImageRef m_srcImg;
		float m_angDig;
		float m_angRad;

		int m_nScale;
		int m_nRound;
		int m_nRoundByMin;
		int m_nCos;
		int m_nSin;

		CvSize m_resSiz;
		S32ImageRef m_resToSrcMapImg;
		F32ImageRef m_resImg;
		S32ImageRef m_srcToResMapImg;

		FixedVector< LineLimit > m_lineLimit_H_Arr;
		FixedVector< LineLimit > m_lineLimit_V_Arr;

		//FixedVector< ImageLineItrProvider > m_lineItrProv_Arr;

		ImageItrMgr m_imageItrMgr;

		
	};


	typedef Hcpl::ObjRef< ImgRotationMgr > ImgRotationMgrRef;

}