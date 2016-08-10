#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\ImageWatershedMgr.h>

namespace Hcv
{
	using namespace Hcpl;
	//using namespace Hcpl::Math;



	ImageWatershedMgr::ImageWatershedMgr(F32ImageRef a_gradImg, int a_nValScale)
	{
		Hcpl_ASSERT(1 == a_gradImg->GetNofChannels());

		float * gradBuf = a_gradImg->GetPixAt(0, 0);

		CvSize imgSiz = a_gradImg->GetSize();

		Init(gradBuf, imgSiz, a_nValScale);
	}


	ImageWatershedMgr::ImageWatershedMgr(float * a_gradBuf, CvSize a_imgSiz, int a_nValScale)
	{
		Init(a_gradBuf, a_imgSiz, a_nValScale);
	}


	void ImageWatershedMgr::Init(float * a_gradBuf, CvSize a_imgSiz, int a_nValScale)
	{
		m_imgSiz = a_imgSiz;
		
		m_nValScale = a_nValScale;

		m_rangeTester_X = new RangeTester( a_imgSiz.width, 10 );
		m_rangeTester_Y = new RangeTester( a_imgSiz.height, 10 );


		m_elm_Ques.InitSize( 5000 * a_nValScale );

		int nImgSize1D = a_imgSiz.width * a_imgSiz.height;

		m_dataArr.SetSize( nImgSize1D );

		//int x = 0;
		//int y = 0;

		//for( int i = 0; i < nImgSize1D; i++ )


		IndexCalc2D idx_2D( a_imgSiz.width, a_imgSiz.height );
			
		for( int y = 0; y < a_imgSiz.height; y++ )
		{
			for( int x = 0; x < a_imgSiz.width; x++ )
			{
				int i = idx_2D.Calc( x, y );

				ImageWatershed_Elm * pIwe = & m_dataArr[ i ];

				pIwe->Index = i;

				pIwe->X = x;
				pIwe->Y = y;

				pIwe->pMaster = pIwe;

				pIwe->IsFlooded = false;

				pIwe->ScaledVal = (int)(a_gradBuf[ i ] * m_nValScale);

				m_elm_Ques.PushPtr( pIwe->ScaledVal, pIwe );
			}
		}



	}


	void ImageWatershedMgr::FloodToVal( float a_val )
	{
		static int dxArr[] = { 1, 1, 0, -1,		-1, -1, 0, 1 };
		static int dyArr[] = { 0, 1, 1, 1,		0, -1, -1, -1 };

		bool * test_X_Buf = m_rangeTester_X->GetTestBuf();
		bool * test_Y_Buf = m_rangeTester_Y->GetTestBuf();

		IndexCalc2D idx2D( m_imgSiz.width, m_imgSiz.height );

		
		int nScaledVal = (int)(a_val * m_nValScale);


		while( m_elm_Ques.Get_CurMinIndex() <= nScaledVal )
		{
			ImageWatershed_Elm * pIwe = m_elm_Ques.PopPtrMin();

			if( NULL == pIwe )
				break;



			if( pIwe->IsFlooded )
				continue;


			pIwe->IsFlooded = true;

			for( int i=0; i < 8; i++ )
			{
				int x2 = pIwe->X + dxArr[i];

				if( ! test_X_Buf[ x2 ] )
					continue;


				int y2 = pIwe->Y + dyArr[i];

				if( ! test_Y_Buf[ y2 ] )
					continue;

				//int nIdx_2 = ;
				
				ImageWatershed_Elm * pIwe_2 = 
					& m_dataArr[ idx2D.Calc( x2, y2 ) ];

				if( pIwe->ScaledVal > pIwe_2->ScaledVal )
				{
					MergeElms( pIwe, pIwe_2 );
				}
				else if( pIwe->ScaledVal < pIwe_2->ScaledVal )
				{
					//	continue;
				}
				else	//	pIwe->ScaledVal == pIwe_2->ScaledVal
				{
					if( i < 4 )
					{
						MergeElms( pIwe, pIwe_2 );
					}
					//else
					//{
					//	continue;
					//}
				}

			}

			//fsdfsdf

		}

	}




	void ImageWatershedMgr::UpdateMasterElm( ImageWatershed_Elm * a_pIwe )
	{
		static FixedVector<ImageWatershed_Elm *> elmVect(10000);
		elmVect.SetSize(10000);
		static int nMaxNofElms = elmVect.GetSize();

		ImageWatershed_Elm * pMasterElm;
		do
		{
			pMasterElm = a_pIwe ;
			int nofElms = 0;

//			while(pMasterElm != pMasterElm->pMaster && nofElms < nMaxNofElms)
			while(pMasterElm != pMasterElm->pMaster)
			{
				elmVect[nofElms++] = pMasterElm;
				pMasterElm = pMasterElm->pMaster;
			}

			for(int i=0; i < nofElms; i++)
				elmVect[i]->pMaster = pMasterElm;

		}while( pMasterElm != pMasterElm->pMaster );

	}
	


	void ImageWatershedMgr::MergeElms( 
		ImageWatershed_Elm * a_pIwe_1, ImageWatershed_Elm * a_pIwe_2 )
	{
		UpdateMasterElm( a_pIwe_1 );
		UpdateMasterElm( a_pIwe_2 );


		if( a_pIwe_1->pMaster == a_pIwe_2->pMaster )
		{
			return;
		}
		else if( a_pIwe_1->pMaster < a_pIwe_2->pMaster )
		{
			a_pIwe_2->pMaster->pMaster =
				a_pIwe_1->pMaster;
		}
		else	//	a_pIwe_1->pMaster > a_pIwe_2->pMaster
		{
			a_pIwe_1->pMaster->pMaster =
				a_pIwe_2->pMaster;
		}

	}


	bool ImageWatershedMgr::AreTogether( int a_nIdx_Iwe_1, int a_nIdx_Iwe_2 )
	{
		ImageWatershed_Elm * pIwe_1 = & m_dataArr[ a_nIdx_Iwe_1 ];
		ImageWatershed_Elm * pIwe_2 = & m_dataArr[ a_nIdx_Iwe_2 ];

		return AreTogether( pIwe_1, pIwe_2 );
	}


	bool ImageWatershedMgr::AreTogether( 
		ImageWatershed_Elm * a_pIwe_1, ImageWatershed_Elm * a_pIwe_2 )
	{
		UpdateMasterElm( a_pIwe_1 );
		UpdateMasterElm( a_pIwe_2 );

		return ( a_pIwe_1->pMaster == a_pIwe_2->pMaster );
	}

}