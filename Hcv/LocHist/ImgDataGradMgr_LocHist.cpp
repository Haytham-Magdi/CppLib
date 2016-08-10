#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\LocHist\ImgDataGradMgr_LocHist.h>

#include <Lib\Hcv\LocHist\ImgDataMgr_2_LocHist.h>



namespace Hcv
{
	using namespace Hcpl;
	//using namespace Hcpl::Math;




	IImgDataGradMgr * ImgDataGradMgr_LocHist_Factory::CreateImgDataGradMgr( 
			IImgDataMgr_2Ref a_imgData, ImageNbrMgrRef a_nbrMgr )
	{
		IImgDataGradMgr * pIdm = new ImgDataGradMgr_LocHist(
			a_imgData, a_nbrMgr );

		return pIdm;		
	}


	ImgDataGradMgr_LocHist::ImgDataGradMgr_LocHist( 
			IImgDataMgr_2Ref a_imgData, ImageNbrMgrRef a_nbrMgr )
	{
		m_bSrcExDataDone = false;

		m_imgDataMgr = a_imgData;

		m_nbrMgr = a_nbrMgr;
				
		m_nAprSiz = m_nbrMgr->GetAprSiz();

		PrepareData();
	}


	F32ImageRef ImgDataGradMgr_LocHist::GetGradImg_ByNormIdx( int a_nNormIdx )
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

	F32ImageRef ImgDataGradMgr_LocHist::GetConfImg_ByNormIdx( int a_nNormIdx )
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


	
	void ImgDataGradMgr_LocHist::PrepareData()
	{

		{
			IImgDataMgr_2 * pDM = (IImgDataMgr_2 *) m_imgDataMgr;

			m_pDataMgr_LH = dynamic_cast< ImgDataMgr_2_LocHist * >( pDM );

			if( NULL == m_pDataMgr_LH )
				ThrowHcvException();

			//m_pClustHisto = m_pDataMgr_LH->m_pClustHisto;
			
			m_pClustHisto = m_pDataMgr_LH->m_pClustHisto;

			m_srcMeanImg = m_pDataMgr_LH->m_meanImg;
			m_srcMean_Buf = (F32ColorVal *)m_srcMeanImg->GetPixAt( 0, 0 );

			m_srcStanDevImg = m_pDataMgr_LH->m_stanDevImg;
			m_srcStanDev_Buf = m_srcStanDevImg->GetPixAt( 0, 0 );

			m_srcAvgSqrMagImg = m_pDataMgr_LH->m_magSqrImg;
			m_srcAvgMagSqr_Buf = m_srcAvgSqrMagImg->GetPixAt( 0, 0 );


			m_pPixShareArrColl_Tot = & m_pDataMgr_LH->m_pixShareArrColl_Tot;

			if( NULL == m_pPixShareArrColl_Tot )
				ThrowHcvException();
		}


		m_gradImg_H = GenGradImg( m_nbrMgr->GetNbr_H_Arr() );
		
		m_gradImg_V = GenGradImg( m_nbrMgr->GetNbr_V_Arr() );



		PrepareSharpStuff();


		//m_pDataMgr_LH->PrepareExData();

		m_conf_Sharp_Buf = m_sharp_GM->GetConflictImg_H()->GetPixAt( 0, 0 );
		m_confImg_H = GenConfImg( m_nbrMgr->GetNbr_H_Arr() );
		
		m_conf_Sharp_Buf = m_sharp_GM->GetConflictImg_V()->GetPixAt( 0, 0 );
		m_confImg_V = GenConfImg( m_nbrMgr->GetNbr_V_Arr() );


		//m_pDataMgr_LH->CleanExData();

	}


	F32ImageRef ImgDataGradMgr_LocHist::GenGradImg( 
		FixedVector< ImageNbrMgr::PixIndexNbr > & a_rNbrArr )
	{

		F32ImageRef gradImg = F32Image::Create(
			m_imgDataMgr->GetSrcImg()->GetSize(), 1 );

		//FixedVector< ImgDataElm_LocHist > & rDataArr = m_pDataMgr_LH->GetDataArr();

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

			//float dif = ImgDataElm_LocHist::CalcDif(
			//	rDataArr[ rNbr.nIdxBef ], rDataArr[ rNbr.nIdxAft ] );

			float dif = CalcDif( rNbr.nIdxBef, rNbr.nIdxAft );

			gradImg_Buf[ i ] = dif;
		}
				
		//ThrowHcvException();
		return gradImg;
	}


	float ImgDataGradMgr_LocHist::CalcDif( int a_nIdx_1, int a_nIdx_2 )
	{
		//ThrowHcvException();
		//return -1;

		F32ColorVal & rC1 = m_srcMean_Buf[ a_nIdx_1 ];
		F32ColorVal & rC2 = m_srcMean_Buf[ a_nIdx_2 ];

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


	void ImgDataGradMgr_LocHist::PrepareSharpStuff()
	{
		int nAprSiz_Sharp = 7;

		ImgRotationMgrRef rm1 = m_pDataMgr_LH->m_rot;
			
		{
			IImgDataMgr_2 * pIdm = new ImgDataMgr_2_Simple(
				rm1->GetResImg(), nAprSiz_Sharp, NULL, rm1 );

			m_sharp_IDM = pIdm;
		}

		{
			IImgDataGradMgr * pGm = new ImgDataGradMgr_Simple(
				m_sharp_IDM, 
				new ImageNbrMgr( rm1->GetImageItrMgr(), nAprSiz_Sharp ));

			m_sharp_GM = pGm;
		}

	}


	F32ImageRef ImgDataGradMgr_LocHist::GenConfImg( 
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

		//ImgDataMgr_2_LocHist * m_pDataMgr_LH;

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

			//float dif = ImgDataElm_LocHist::CalcDif(
			//	rDataArr[ rNbr.nIdxBef ], rDataArr[ rNbr.nIdxAft ] );

			float dif = CalcConf( i, rNbr.nIdxBef, rNbr.nIdxAft );

			confImg_Buf[ i ] = dif;
		}
				
		//ThrowHcvException();
		return confImg;
//*/
	}


	float ImgDataGradMgr_LocHist::CalcConf( int a_nIdx_E, int a_nIdx_1, int a_nIdx_2 )
	{
		//ThrowHcvException();
		//return -1;

		F32ColorVal & rC1 = m_srcMean_Buf[ a_nIdx_1 ];
		F32ColorVal & rC2 = m_srcMean_Buf[ a_nIdx_2 ];

		//float dif = F32ColorVal::Sub( rC1, rC2 ).CalcMag();

		//if( dif < 20 )
			//return 0;

		float ams_1 = m_srcAvgMagSqr_Buf[ a_nIdx_1 ];
		float ams_2 = m_srcAvgMagSqr_Buf[ a_nIdx_2 ];

		float ams_Tot = ( ams_1 + ams_2 ) / 2;

		F32ColorVal meanColor_Tot = F32ColorVal::Add( rC1, rC2 );
		meanColor_Tot.DividSelfBy( 2 );

		float dev_Tot = ams_Tot - meanColor_Tot.CalcMagSqr();
		if( dev_Tot < 0 )
			dev_Tot = 0;

		dev_Tot = sqrt( (float) dev_Tot );

		if( dev_Tot < 20 )
			return 0;

		float dev_1 = m_srcStanDev_Buf[ a_nIdx_1 ];
		float dev_2 = m_srcStanDev_Buf[ a_nIdx_2 ];

		//float ratio = 0.3;
		float ratio = 0.4;

		float val;

		val = dev_Tot * ratio;

		if( dev_1 > val )
			goto ComplexTest;

		if( dev_2 > val )
			goto ComplexTest;

		return 100;


ComplexTest:

		return 0;


		if( m_conf_Sharp_Buf[ a_nIdx_E ] < 10 )
			return 0;


///----

		//if( false )
		//{
		//	int nTmp = a_nIdx_1;
		//	a_nIdx_1 = a_nIdx_2;
		//	a_nIdx_2 = nTmp;
		//}

		ImgDataSrc_LocHist::ClustShare * buf_1 = 
			m_pPixShareArrColl_Tot->GetHeadAt( a_nIdx_1 );

		int nLen_1 = m_pPixShareArrColl_Tot->GetSizeAt( a_nIdx_1 );

		ImgDataSrc_LocHist::ClustShare * buf_2 = 
			m_pPixShareArrColl_Tot->GetHeadAt( a_nIdx_2 );

		int nLen_2 = m_pPixShareArrColl_Tot->GetSizeAt( a_nIdx_2 );


		FixedVector< ImgDataSrc_LocHist::Cluster > & rClusterArr =
			*m_pDataMgr_LH->m_pClustHisto->m_pClusterArr;

		int nofPixs_Tot =  Sqr( m_pDataMgr_LH->m_nAprSiz );


		float dif_Tot = 0;


		float confRatio_T = 0;

		//float ratioThr = 0.6;
		//float ratioThr = 0.7;
		float ratioThr = 0.75;
		//float ratioThr = 0.9;

		for( int i=0; i < nLen_1; i++ )
		{
			ImgDataSrc_LocHist::ClustShare & rS_1 = buf_1[ i ];
			float ratio_1 = (float)rS_1.nofPixes / nofPixs_Tot;
			
			Hcpl_ASSERT( ratio_1 < 1.3 );

			F32ColorVal & rC1 = rClusterArr[ rS_1.nClustIdx ].OrgColor;

			float confRatio_1 = 0;
			
							
			for( int j=0; j < nLen_2; j++ )
			{
				ImgDataSrc_LocHist::ClustShare & rS_2 = buf_2[ j ];			
				float ratio_2 = (float)rS_2.nofPixes / nofPixs_Tot;

				Hcpl_ASSERT( ratio_2 < 1.3 );

				F32ColorVal & rC2 = rClusterArr[ rS_2.nClustIdx ].OrgColor;

				float colorDif = F32ColorVal::Sub( rC1, rC2 ).CalcMag();

				if( colorDif > 18 )
					confRatio_1 += ratio_2;

				//dif_Tot += colorDif * ratio_1 * ratio_2;
			}

			if( confRatio_1 > ratioThr )
				confRatio_T += ratio_1;
		}

		if( confRatio_T > ratioThr )
			return 100;

		return 0;



///----


	}

}


