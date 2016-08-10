#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Cpp\Math\mathLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>

#include <Lib\Hcv\IImgDataGradMgr.h>
#include <Lib\Hcv\GridPal\ImgDataMgr_2_GridPal.h>




namespace Hcv
{
	//using namespace Hcpl::Math;


	class ImgDataGradMgr_GridPal_Factory : public IImgDataGradMgr_Factory
	{
	public:

		virtual IImgDataGradMgr * CreateImgDataGradMgr( 
			 IImgDataMgr_2Ref a_imgData, ImageNbrMgrRef a_nbrMgr );
	};

	typedef Hcpl::ObjRef< ImgDataGradMgr_GridPal_Factory > ImgDataGradMgr_GridPal_FactoryRef;



	class ImgDataGradMgr_GridPal : public IImgDataGradMgr
	{
	public:

		ImgDataGradMgr_GridPal( 
			IImgDataMgr_2Ref a_imgData, ImageNbrMgrRef a_nbrMgr );

		virtual IImgDataMgr_2Ref GetSrcImgDataMgr()
		{
			return m_imgDataMgr;
		}

		virtual F32ImageRef GetGradImg_H()
		{
			return m_gradImg_H;
		}

		virtual F32ImageRef GetGradImg_V()
		{
			return m_gradImg_V;
		}

		virtual ImageNbrMgrRef GetNbrMgr()
		{
			return m_nbrMgr;
		}

		virtual int GetAprSiz()
		{
			return m_nAprSiz;
		}

		virtual F32ImageRef GetGradImg_ByNormIdx( int a_nNormIdx );
		
		virtual float GetMaxDif()
		{
			return S_GetMaxDif();
		}

		static float S_GetMaxDif()
		{
			return 550;
		}


	protected:

		void PrepareData();

		float CalcDif( int a_nIdx_1, int a_nIdx_2 ); 

		F32ImageRef GenGradImg( 
			FixedVector< ImageNbrMgr::PixIndexNbr > & a_rNbrArr );


	protected:

		IImgDataMgr_2Ref m_imgDataMgr;

		ImgDataMgr_2_GridPal * m_pDataMgr_LH;
		Hcpl::CommonArrCollBuff< ImgDataSrc_GridPal::ClustShare > * m_pPixShareArrColl_Tot;

		F32ImageRef m_gradImg_H;
		F32ImageRef m_gradImg_V;

		ImageNbrMgrRef m_nbrMgr;
		
		int m_nAprSiz;
	};

	typedef Hcpl::ObjRef< ImgDataGradMgr_GridPal > ImgDataGradMgr_GridPalRef;



}