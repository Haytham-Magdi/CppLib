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
#include <Lib\Hcv\LocHist\ImgDataMgr_2_LocHist.h>




namespace Hcv
{
	//using namespace Hcpl::Math;


	class ImgDataGradMgr_LocHist_Factory : public IImgDataGradMgr_Factory
	{
	public:

		virtual IImgDataGradMgr * CreateImgDataGradMgr( 
			 IImgDataMgr_2Ref a_imgData, ImageNbrMgrRef a_nbrMgr );
	};

	typedef Hcpl::ObjRef< ImgDataGradMgr_LocHist_Factory > ImgDataGradMgr_LocHist_FactoryRef;



	class ImgDataGradMgr_LocHist : public IImgDataGradMgr
	{
	public:

		ImgDataGradMgr_LocHist( 
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

		virtual F32ImageRef GetConflictImg_H()
		{
			ThrowHcvException();

			return NULL;
		}

		virtual F32ImageRef GetConflictImg_V()
		{
			ThrowHcvException();

			return NULL;
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
		
		virtual F32ImageRef GetConfImg_ByNormIdx( int a_nNormIdx );

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

		void PrepareSharpStuff();

		float CalcDif( int a_nIdx_1, int a_nIdx_2 ); 

		F32ImageRef GenGradImg( 
			FixedVector< ImageNbrMgr::PixIndexNbr > & a_rNbrArr );

		float CalcConf( int a_nIdx_E, int a_nIdx_1, int a_nIdx_2 ); 

		F32ImageRef GenConfImg( 
			FixedVector< ImageNbrMgr::PixIndexNbr > & a_rNbrArr );


	protected:

		IImgDataMgr_2Ref m_imgDataMgr;

		F32ImageRef m_srcMeanImg;
		F32ColorVal * m_srcMean_Buf;

		F32ImageRef m_srcStanDevImg;
		float * m_srcStanDev_Buf;

		F32ImageRef m_srcAvgSqrMagImg;
		float * m_srcAvgMagSqr_Buf;

		ImgDataMgr_2_LocHist * m_pDataMgr_LH;
		Hcpl::CommonArrCollBuff< ImgDataSrc_LocHist::ClustShare > * m_pPixShareArrColl_Tot;

		ImgDataSrc_LocHist::ClustHisto * m_pClustHisto;

		F32ImageRef m_gradImg_H;
		F32ImageRef m_gradImg_V;

		float * m_conf_Sharp_Buf;

		F32ImageRef m_confImg_H;
		F32ImageRef m_confImg_V;

		ImageNbrMgrRef m_nbrMgr;
		
		int m_nAprSiz;

		bool m_bSrcExDataDone;

		IImgDataMgr_2Ref m_sharp_IDM;
		IImgDataGradMgrRef m_sharp_GM;
	};

	typedef Hcpl::ObjRef< ImgDataGradMgr_LocHist > ImgDataGradMgr_LocHistRef;



}