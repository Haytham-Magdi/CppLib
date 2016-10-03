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

#define M_PI 3.14159265358979323846


namespace Hcv
{

	class ImgSizeRotation  : FRM_Object
	{
	public:

		ImgSizeRotation( F32ImageRef a_srcImg, float a_angDig );

		F32ImageRef GetSrcImg() { return m_srcImg; }

		float GetAngleByDigree() { return m_angDig; }

		float GetAngleByRad() { return m_angRad; }

		CvSize GetResImgSiz() { return m_resSiz; }

		S32ImageRef Get_ResToSrcMapImage() { return m_resToSrcMapImg; }

		//S32ImageRef Get_ResToSrcMap_X_Scaled_Image()
		//{
		//	return m_resToSrcMapImg_X_Scaled;
		//}

		//S32ImageRef Get_ResToSrcMap_Y_Scaled_Image()
		//{
		//	return m_resToSrcMapImg_Y_Scaled;
		//}

		F32ImageRef GetResImg() { return m_resImg; }

		S32ImageRef Get_SrcToResMapImage() { return m_srcToResMapImg; }

		ImageItrMgrRef GetImageItrMgr() { return m_imageItrMgr; }

		CvPoint & GetBgnPnt() { return m_bgnPnt; }

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

		CvPoint m_bgnPnt;

		CvSize m_resSiz;
		S32ImageRef m_resToSrcMapImg;
		//S32ImageRef m_resToSrcMapImg_X_Scaled;
		//S32ImageRef m_resToSrcMapImg_Y_Scaled;
		F32ImageRef m_resImg;
		S32ImageRef m_srcToResMapImg;

		//FixedVector< CvPoint > m_srcPntOfRes_Arr;

		FixedVector< LineLimit > m_lineLimit_H_Arr;
		FixedVector< LineLimit > m_lineLimit_V_Arr;

		//FixedVector< ImageLineItrProvider > m_lineItrProv_Arr;

		ImageItrMgrRef m_imageItrMgr;
	
	};


	typedef Hcpl::ObjRef< ImgSizeRotation > ImgSizeRotation_Ref;

}