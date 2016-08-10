#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\GridPal\ImgDataGradMgr_GridPal.h>

#include <Lib\Hcv\GridPal\ImgDataMgr_2_GridPal.h>



namespace Hcv
{
	using namespace Hcpl;
	//using namespace Hcpl::Math;




	IImgDataGradMgr * ImgDataGradMgr_GridPal_Factory::CreateImgDataGradMgr( 
			IImgDataMgr_2Ref a_imgData, ImageNbrMgrRef a_nbrMgr )
	{
		IImgDataGradMgr * pIdm = new ImgDataGradMgr_GridPal(
			a_imgData, a_nbrMgr );

		return pIdm;		
	}


	ImgDataGradMgr_GridPal::ImgDataGradMgr_GridPal( 
			IImgDataMgr_2Ref a_imgData, ImageNbrMgrRef a_nbrMgr )
	{
		m_imgDataMgr = a_imgData;

		m_nbrMgr = a_nbrMgr;
				
		m_nAprSiz = m_nbrMgr->GetAprSiz();

		PrepareData();
	}


	F32ImageRef ImgDataGradMgr_GridPal::GetGradImg_ByNormIdx( int a_nNormIdx )
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


	
	void ImgDataGradMgr_GridPal::PrepareData()
	{
		m_gradImg_H = GenGradImg( m_nbrMgr->GetNbr_H_Arr() );
		
		m_gradImg_V = GenGradImg( m_nbrMgr->GetNbr_V_Arr() );
	}


	F32ImageRef ImgDataGradMgr_GridPal::GenGradImg( 
		FixedVector< ImageNbrMgr::PixIndexNbr > & a_rNbrArr )
	{
		ThrowHcvException();

		return NULL;
/*
		{
			CvSize imgSiz = m_imgDataMgr->GetSrcImg()->GetSize();

			int nImgSiz1D = imgSiz.width * imgSiz.height;

			Hcpl_ASSERT( a_rNbrArr.GetSize() == nImgSiz1D );
		}

		//ImgDataMgr_2_GridPal * m_pDataMgr_LH;
		{
			IImgDataMgr_2 * pDM = (IImgDataMgr_2 *) m_imgDataMgr;

			m_pDataMgr_LH = dynamic_cast< ImgDataMgr_2_GridPal * >( pDM );

			if( NULL == m_pDataMgr_LH )
				ThrowHcvException();

			m_pPixShareArrColl_Tot = & m_pDataMgr_LH->m_pixShareArrColl_Tot;

			if( NULL == m_pPixShareArrColl_Tot )
				ThrowHcvException();
		}

		F32ImageRef gradImg = F32Image::Create(
			m_imgDataMgr->GetSrcImg()->GetSize(), 1 );



		//FixedVector< ImgDataElm_GridPal > & rDataArr = m_pDataMgr_LH->GetDataArr();

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

			//float dif = ImgDataElm_GridPal::CalcDif(
			//	rDataArr[ rNbr.nIdxBef ], rDataArr[ rNbr.nIdxAft ] );

			float dif = CalcDif( rNbr.nIdxBef, rNbr.nIdxAft );

			gradImg_Buf[ i ] = dif;
		}
				
		//ThrowHcvException();
		return gradImg;
		*/
	}


	float ImgDataGradMgr_GridPal::CalcDif( int a_nIdx_1, int a_nIdx_2 )
	{
		ThrowHcvException();
		return -1;

/*
		ImgDataSrc_GridPal::ClustShare * buf_1 = 
			m_pPixShareArrColl_Tot->GetHeadAt( a_nIdx_1 );

		int nLen_1 = m_pPixShareArrColl_Tot->GetSizeAt( a_nIdx_1 );

		ImgDataSrc_GridPal::ClustShare * buf_2 = 
			m_pPixShareArrColl_Tot->GetHeadAt( a_nIdx_2 );

		int nLen_2 = m_pPixShareArrColl_Tot->GetSizeAt( a_nIdx_2 );


		//ImgDataSrc_GridPal::

		FixedVector< ImgDataSrc_GridPal::Cluster > & rClusterArr =
			*m_pDataMgr_LH->m_pClustHisto->m_pClusterArr;

		int nofPixs_Tot =  Sqr( m_pDataMgr_LH->m_nAprSiz );

		//bool bDone = false;
		//	if( ! bDone )
		//	{

		//	}

		float dif_Tot = 0;


		//F32ColorVal meanColor_1;
		//meanColor_1.AssignVal( 0, 0, 0 );

		//for( int i=0; i < nLen_1; i++ )
		//{
		//	ImgDataSrc_GridPal::ClustShare & rS_1 = buf_1[ i ];
		//	float ratio_1 = (float)rS_1.nofPixes / nofPixs_Tot;
		//					
		//	F32ColorVal & rC1 = rClusterArr[ rS_1.nClustIdx ].OrgColor;
		//	meanColor_1.IncBy( rC1.MultBy( ratio_1 ) );
		//}

		//F32ColorVal meanColor_2;
		//meanColor_2.AssignVal( 0, 0, 0 );

		//for( int i=0; i < nLen_2; i++ )
		//{
		//	ImgDataSrc_GridPal::ClustShare & rS_2 = buf_2[ i ];
		//	float ratio_2 = (float)rS_2.nofPixes / nofPixs_Tot;
		//					
		//	F32ColorVal & rC2 = rClusterArr[ rS_2.nClustIdx ].OrgColor;
		//	meanColor_2.IncBy( rC2.MultBy( ratio_2 ) );
		//}

		//dif_Tot = F32ColorVal::Sub( meanColor_1, meanColor_2 ).CalcMag();

		for( int i=0; i < nLen_1; i++ )
		{
			ImgDataSrc_GridPal::ClustShare & rS_1 = buf_1[ i ];
			float ratio_1 = (float)rS_1.nofPixes / nofPixs_Tot;
							
			F32ColorVal & rC1 = rClusterArr[ rS_1.nClustIdx ].OrgColor;
			
							
			for( int j=0; j < nLen_1; j++ )
			{
				ImgDataSrc_GridPal::ClustShare & rS_2 = buf_2[ j ];			
				float ratio_2 = (float)rS_2.nofPixes / nofPixs_Tot;

				F32ColorVal & rC2 = rClusterArr[ rS_2.nClustIdx ].OrgColor;

				float colorDif = F32ColorVal::Sub( rC1, rC2 ).CalcMag();

				dif_Tot += colorDif * ratio_1 * ratio_2;
			}
		}

		return dif_Tot;
		*/
	}



}