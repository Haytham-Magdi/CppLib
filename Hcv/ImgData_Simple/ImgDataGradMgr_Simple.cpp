#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\ImgData_Simple\ImgDataGradMgr_Simple.h>

#include <Lib\Hcv\ImgData_Simple\ImgDataMgr_2_Simple.h>



namespace Hcv
{
	using namespace Hcpl;
	//using namespace Hcpl::Math;




	IImgDataGradMgr * ImgDataGradMgr_Simple_Factory::CreateImgDataGradMgr( 
			IImgDataMgr_2Ref a_imgData, ImageNbrMgrRef a_nbrMgr )
	{
		IImgDataGradMgr * pIdm = new ImgDataGradMgr_Simple(
			a_imgData, a_nbrMgr );

		return pIdm;		
	}


	ImgDataGradMgr_Simple::ImgDataGradMgr_Simple( 
			IImgDataMgr_2Ref a_imgData, ImageNbrMgrRef a_nbrMgr )
	{
		m_bSrcExDataDone = false;

		m_imgDataMgr = a_imgData;

		m_nbrMgr = a_nbrMgr;
				
		m_nAprSiz = m_nbrMgr->GetAprSiz();

		PrepareData();
	}


	F32ImageRef ImgDataGradMgr_Simple::GetGradImg_ByNormIdx( int a_nNormIdx )
	{
		if( 0 == a_nNormIdx )
		{
			return m_gradImg_H;
		}
		else if( 1 == a_nNormIdx )
		{
			return m_gradImg_V;
		}
		else
		{
			ThrowHcvException();
			return m_gradImg_H;
		}
	}

	F32ImageRef ImgDataGradMgr_Simple::GetConfImg_ByNormIdx( int a_nNormIdx )
	{
		if( 0 == a_nNormIdx )
		{
			return m_confImg_H;
		}
		else if( 1 == a_nNormIdx )
		{
			return m_confImg_V;
		}
		else
		{
			ThrowHcvException();
			return m_confImg_H;
		}
	}


	
	void ImgDataGradMgr_Simple::PrepareData()
	{

		{
			IImgDataMgr_2 * pDM = (IImgDataMgr_2 *) m_imgDataMgr;

			m_pDataMgr_LH = dynamic_cast< ImgDataMgr_2_Simple * >( pDM );

			if( NULL == m_pDataMgr_LH )
				ThrowHcvException();

			//m_pClustHisto = m_pDataMgr_LH->m_pClustHisto;
			
			m_srcMeanImg = m_pDataMgr_LH->m_meanImg;
			m_srcMean_Buf = (F32ColorVal *)m_srcMeanImg->GetPixAt( 0, 0 );

			int a;
			{
				CvSize meanSiz = m_srcMeanImg->GetSize();
				int nSiz1D = m_srcMeanImg->GetSize1D();

				a = 0;
			}

			m_srcStanDevImg = m_pDataMgr_LH->m_stanDevImg;
			m_srcStanDev_Buf = m_srcStanDevImg->GetPixAt( 0, 0 );

			m_srcAvgSqrMagImg = m_pDataMgr_LH->m_magSqrImg;
			m_srcAvgMagSqr_Buf = m_srcAvgSqrMagImg->GetPixAt( 0, 0 );
		}


		m_gradImg_H = GenGradImg( m_nbrMgr->GetNbr_H_Arr() );
		m_gradImg_H_Buf = m_gradImg_H->GetPixAt( 0, 0 );
		
		m_gradImg_V = GenGradImg( m_nbrMgr->GetNbr_V_Arr() );
		m_gradImg_V_Buf = m_gradImg_V->GetPixAt( 0, 0 );

		//m_pDataMgr_LH->PrepareExData();

		m_gradImg_C_Buf = m_gradImg_H_Buf;
		m_confImg_H = GenConfImg( m_nbrMgr->GetNbr_H_Arr() );
		
		m_gradImg_C_Buf = m_gradImg_V_Buf;
		m_confImg_V = GenConfImg( m_nbrMgr->GetNbr_V_Arr() );


		//m_pDataMgr_LH->CleanExData();

	}


	F32ImageRef ImgDataGradMgr_Simple::GenGradImg( 
		FixedVector< ImageNbrMgr::PixIndexNbr > & a_rNbrArr )
	{

		F32ImageRef gradImg = F32Image::Create(
			m_imgDataMgr->GetSrcImg()->GetSize(), 1 );

		//FixedVector< ImgDataElm_Simple > & rDataArr = m_pDataMgr_LH->GetDataArr();

		float * gradImg_Buf = gradImg->GetPixAt( 0, 0 );


		for( int i=0; i < a_rNbrArr.GetSize(); i++ )
		{
			ImageNbrMgr::PixIndexNbr & rNbr = a_rNbrArr[ i ];
			
			if( -1 == rNbr.nIdxBef || -1 == rNbr.nIdxAft )
			{
				gradImg_Buf[ i ] = S_GetMaxDif();
				continue;
			}

			if( 11832 == i )
				i = i;

			//float dif = ImgDataElm_Simple::CalcDif(
			//	rDataArr[ rNbr.nIdxBef ], rDataArr[ rNbr.nIdxAft ] );

			float dif = CalcDif( rNbr.nIdxBef, rNbr.nIdxAft );

			gradImg_Buf[ i ] = dif;
		}
				
		//ThrowHcvException();
		return gradImg;
	}


	float ImgDataGradMgr_Simple::CalcDif( int a_nIdx_1, int a_nIdx_2 )
	{
		//ThrowHcvException();
		//return -1;

		F32ColorVal & rC1 = m_srcMean_Buf[ a_nIdx_1 ];
		F32ColorVal & rC2 = m_srcMean_Buf[ a_nIdx_2 ];

		float dif_0 = F32ColorVal::Sub( 
			rC1, rC2 ).CalcMag();

		return dif_0;

		float ams_1 = m_srcAvgMagSqr_Buf[ a_nIdx_1 ];
		float ams_2 = m_srcAvgMagSqr_Buf[ a_nIdx_2 ];

		float ams_Tot = ( ams_1 + ams_2 ) / 2;

		F32ColorVal meanColor_Tot = F32ColorVal::Add( rC1, rC2 );
		meanColor_Tot.DividSelfBy( 2 );

		float dev_Tot = ams_Tot - meanColor_Tot.CalcMagSqr();
		if( dev_Tot < 0 )
			dev_Tot = 0;

		dev_Tot = sqrt( (float) dev_Tot );

		return dev_Tot;


		float dif = F32ColorVal::Sub( rC1, rC2 ).CalcMag();

		return dif;

	}




	F32ImageRef ImgDataGradMgr_Simple::GenConfImg( 
		FixedVector< ImageNbrMgr::PixIndexNbr > & a_rNbrArr )
	{
		//ThrowHcvException();

		//return NULL;
///*
		{
			CvSize imgSiz = m_imgDataMgr->GetSrcImg()->GetSize();

			int nImgSiz1D = imgSiz.width * imgSiz.height;

			Hcpl_ASSERT( a_rNbrArr.GetSize() == nImgSiz1D );
		}

		//ImgDataMgr_2_Simple * m_pDataMgr_LH;

		F32ImageRef confImg = F32Image::Create(
			m_imgDataMgr->GetSrcImg()->GetSize(), 1 );



		float * confImg_Buf = confImg->GetPixAt( 0, 0 );


		for( int i=0; i < a_rNbrArr.GetSize(); i++ )
		{
			ImageNbrMgr::PixIndexNbr & rNbr = a_rNbrArr[ i ];
			
			if( -1 == rNbr.nIdxBef || -1 == rNbr.nIdxAft )
			{
				confImg_Buf[ i ] = S_GetMaxDif();
				continue;
			}

			if( 11832 == i )
				i = i;

			//float dif = ImgDataElm_Simple::CalcDif(
			//	rDataArr[ rNbr.nIdxBef ], rDataArr[ rNbr.nIdxAft ] );

			float dif = CalcConf( rNbr.nIdxBef, i, rNbr.nIdxAft );

			confImg_Buf[ i ] = dif;
		}
				
		//ThrowHcvException();
		return confImg;
//*/
	}


	float ImgDataGradMgr_Simple::CalcConf( int a_nIdx_1, int a_nIdx_E, int a_nIdx_2 )
	{
		//return 0;

		//ThrowHcvException();
		//return -1;


		float grad_1 = m_gradImg_C_Buf[ a_nIdx_1 ];
		float grad_E = m_gradImg_C_Buf[ a_nIdx_E ];
		float grad_2 = m_gradImg_C_Buf[ a_nIdx_2 ];

		if( grad_E < 20 )
			return 0;


		//float ratio = 0.3;
		//float ratio = 0.6;
		//float ratio = 0.8;
		float ratio = 0.95;
		//float ratio = 1.6;

		float val;

		val = grad_E * ratio;

		if( grad_1 > val )
			goto ComplexTest;

		if( grad_2 > val )
			goto ComplexTest;

		//return 100;
		//return 200;
		return 255;


ComplexTest:

		return 0;

///----


///----


	}

}


