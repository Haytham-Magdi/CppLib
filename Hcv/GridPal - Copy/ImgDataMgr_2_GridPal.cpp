#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\GridPal\ImgDataMgr_2_GridPal.h>

namespace Hcv
{
	using namespace Hcpl;
	//using namespace Hcpl::Math;


	IImgDataMgr_2 * ImgDataMgr_2_GridPal_Factory::CreateImgDataMgr( 
			F32ImageRef a_srcImg, int a_nAprSiz,
			IImgCoreSrcRef a_dataSrc, ImgRotationMgrRef a_rot )
	{
		IImgDataMgr_2 * pIdm = new ImgDataMgr_2_GridPal(
			a_srcImg, a_nAprSiz, a_dataSrc, a_rot );

		return pIdm;		
	}






	ImgDataSrc_GridPal::ImgDataSrc_GridPal( F32ImageRef a_srcImg )
	{
		m_srcImg = a_srcImg;
		//m_srcImg = GenFastAvgImg( m_srcImg, 5 );

		m_nDifScale = 100;

		int nImgSiz1D = m_srcImg->GetSize1D();

		Proceed();
	}

	void ImgDataSrc_GridPal::Proceed()
	{
		int nAprSiz = 7;

		m_meanImg = GenFastAvgImg( m_srcImg, nAprSiz );

		F32ColorVal * srcBuf = (F32ColorVal *)m_srcImg->GetPixAt( 0, 0 );
			
		int nImgSiz1D = m_srcImg->GetSize1D();
		{

			m_sqrMagMeanImg = F32Image::Create( m_srcImg->GetSize(), 1 );
			float * sqrMagBuf = m_sqrMagMeanImg->GetPixAt( 0, 0 );

			for( int i=0; i < nImgSiz1D; i++ )
			{
				F32ColorVal & rColor = srcBuf[ i ];

				sqrMagBuf[ i ] = rColor.CalcMagSqr();
			}

			m_sqrMagMeanImg = GenFastAvgImg( m_sqrMagMeanImg, nAprSiz );
		}

		F32ColorVal * meanBuf = (F32ColorVal *)m_meanImg->GetPixAt( 0, 0 );
		{
			m_stanDevImg = m_sqrMagMeanImg->Clone();


			float * stanDevBuf = m_stanDevImg->GetPixAt( 0, 0 );

			for( int i=0; i < nImgSiz1D; i++ )
			{
				if( 17812 == i )
					i = i;

				F32ColorVal & rColor = meanBuf[ i ];

				float & rVal = stanDevBuf[ i ];

				 rVal -= rColor.CalcMagSqr();

				 if( rVal < 0 )
					 rVal = 0;

				 rVal = sqrt( (float)rVal );

				 i = i;
			}
		}

		{

			m_clusterArr.SetCapacity( nImgSiz1D );
		
			m_pixEntArr.SetSize( nImgSiz1D );
			for( int i=0; i < nImgSiz1D; i++ )
			{
				m_pixEntArr[ i ].nIndex = i;
			}

			//m_pixShareArrColl_V.SetCapacity( 
			//	m_pixEntArr.GetSize() * 10, m_pixEntArr.GetSize() );

			//m_pixShareArrColl_Tot.SetCapacity( 
			//	m_pixEntArr.GetSize() * 100, m_pixEntArr.GetSize() );

			m_pixEntQues.InitSize( GetMaxStanDev_Scaled() + 2 );

			float * stanDevBuf = m_stanDevImg->GetPixAt( 0, 0 );

			for( int i=0; i < nImgSiz1D; i++ )
			{
				int nOrder = (int) (stanDevBuf[ i ] * m_nDifScale);

				Hcpl_ASSERT( nOrder >= 0 );

				m_pixEntQues.PushPtr( nOrder, & m_pixEntArr[ i ] );

				//m_pixEntArr[ i ].nIndex = i;
			}

			FixedDeque< PixEnt * > pixPtrQue( nImgSiz1D );

			while( true )
			{
				PixEnt * pPE = m_pixEntQues.PopPtrMin();

				if( NULL == pPE )
				{
					break;
				}


				int nDev = m_pixEntQues.GetLastMinIndex() / m_nDifScale;
				if( nDev >= 10 )
				{
					pixPtrQue.PushBack( pPE );
					continue;
				}


				//F32ColorVal & rPixColor = meanBuf[ pPE->nIndex ];
				F32ColorVal & rPixColor = srcBuf[ pPE->nIndex ];
								
				//float sqrMag = rPixColor.CalcMagSqr();


				//if( 83286 == pPE->nIndex )
				//if( 324 == pPE->nIndex )
				if( 16 == pPE->nIndex )
					pPE = pPE;

				bool bFound = false;
				bool bPostponed = false;
				for( int i=0; i < m_clusterArr.GetSize(); i++ )
				{
					Cluster & rClust = m_clusterArr[ i ];

					//F32ColorVal clustMean = rClust.GetMeanColor();
					F32ColorVal clustMean = rClust.OrgColor;

					float dif = F32ColorVal::Sub(
						rPixColor, clustMean ).CalcMag();

					//float distFar = 15;
					float distFar = 10;

					if( dif > distFar )
						continue;

					//float distMed = 4;
					//float distMed = 2;
					float distMed = 0.4 * distFar;

					if( dif >= distMed )
					//if( false )
					{
						if( ! bPostponed )
						{
							pixPtrQue.PushBack( pPE );
							bPostponed = true;
						}
						continue;
					}

					//if( 3 == i && rPixColor.val1 < 15 )
					//	i = i;

					rClust.AddColor( rPixColor );
					pPE->nClustIndex = i;
					bFound = true;

					break;
				}

				if( ! bFound && ! bPostponed )
				{
					m_clusterArr.IncSize();

					Cluster & rClust = m_clusterArr.GetBack();
					rClust.OrgColor = rPixColor;
					rClust.AddColor( rPixColor );
					pPE->nClustIndex = m_clusterArr.GetSize() - 1;
				}
				
			}


			MinIndexFinder< float > minIdxFnd;

			while( pixPtrQue.GetSize() > 0 )
			{
				PixEnt * pPE = pixPtrQue.GetFront();
				pixPtrQue.PopFront();

				Hcpl_ASSERT( NULL != pPE );

				//if( 15694 == pPE->nIndex )
				if( 16 == pPE->nIndex )
					pPE = pPE;

				if( pPE->nClustIndex >= 0 )
					continue;

				//F32ColorVal & rPixColor = meanBuf[ pPE->nIndex ];
				F32ColorVal & rPixColor = srcBuf[ pPE->nIndex ];
								

				minIdxFnd.Reset();

				for( int i=0; i < m_clusterArr.GetSize(); i++ )
				{
					Cluster & rClust = m_clusterArr[ i ];

					//F32ColorVal clustMean = rClust.GetMeanColor();
					F32ColorVal clustMean = rClust.OrgColor;

					float dif = F32ColorVal::Sub(
						rPixColor, clustMean ).CalcMag();

					minIdxFnd.AddValue( dif, i );
				}

				{
					Cluster & rClust = m_clusterArr[ minIdxFnd.FindMinIdx() ];
					rClust.AddColor( rPixColor );
					pPE->nClustIndex = minIdxFnd.FindMinIdx();
					pPE->bFlag_1 = true;
				}

			}



		}


		float stanDevAvg = 0;
		for( int i=0; i < m_clusterArr.GetSize(); i++ )
		{
			Cluster & rClust = m_clusterArr[ i ];

			F32ColorVal & rMean = rClust.GetMeanColor();

			float ratio = rClust.NofPixes * 100.0 / nImgSiz1D;

			rClust.PrepareStanDev();

			stanDevAvg += rClust.StanDev * rClust.NofPixes;

			i = i;
		}
		stanDevAvg /= nImgSiz1D;


		MinFinder< float > minFnd;
		for( int i=0; i < m_clusterArr.GetSize(); i++ )
		{
			for( int j=0; j < m_clusterArr.GetSize(); j++ )
			{
				if( i == j )
					continue;

				Cluster & rClustI = m_clusterArr[ i ];
				Cluster & rClustJ = m_clusterArr[ j ];

				float dif = F32ColorVal::Sub(
					rClustI.GetMeanColor(), rClustJ.GetMeanColor() ).CalcMag();
					//rClustI.OrgColor, rClustJ.OrgColor ).CalcMag();
				

				minFnd.AddValue( dif );
			}
		}

		{
			F32ColorVal * srcBuf = (F32ColorVal *)m_srcImg->GetPixAt( 0, 0 );
			
			F32ImageRef resImg = this->GenResultImg();

			F32ColorVal * resBuf = (F32ColorVal *)resImg->GetPixAt( 0, 0 );

			MaxFinder< float > maxFnd;

			float difSum = 0;

			for( int i=0; i < nImgSiz1D; i++ )
			{
				F32ColorVal & rSrcColor = srcBuf[ i ];				
				F32ColorVal & rResColor = resBuf[ i ];				

				float dif = F32ColorVal::Sub( rSrcColor, rResColor ).CalcMag();

				difSum += dif;

				maxFnd.AddValue( dif );
			}

			difSum /= nImgSiz1D;

			nImgSiz1D = nImgSiz1D;
		}

		//GlobalStuff::SetLinePathImg( this->GenResultImg() );
		//GlobalStuff::ShowLinePathImg();
		
		//ShowImage( this->GenResultImg(), "ResultImg" );
		
		m_clustHisto.Init( m_clusterArr );

	}



	F32ImageRef ImgDataSrc_GridPal::GenResultImg()
	{
		FixedVector< F32ColorVal > colorArr;


		colorArr.SetSize( m_clusterArr.GetSize() );

		for( int i=0; i < colorArr.GetSize(); i++ )
		{
			F32ColorVal & rColor = colorArr[ i ];

			rColor.val0 = rand() % 256;
			rColor.val1 = rand() % 256;
			rColor.val2 = rand() % 256;
		}

		F32ImageRef retImg = F32Image::Create( m_srcImg->GetSize(), 3 );

		F32ColorVal * srcBuf = (F32ColorVal *)m_srcImg->GetPixAt( 0, 0 );

		F32ColorVal * retBuf = (F32ColorVal *)retImg->GetPixAt( 0, 0 );

		for( int i=0; i < m_pixEntArr.GetSize(); i++ )
		{
			PixEnt & rPE = m_pixEntArr[ i ];

			Hcpl_ASSERT( rPE.nClustIndex >= 0 );

			F32ColorVal & rSrcColor = srcBuf[ i ];

			F32ColorVal & rMeanColor = 
				//m_clusterArr[ rPE.nClustIndex ].GetMeanColor();
				m_clusterArr[ rPE.nClustIndex ].OrgColor;

			if( fabsf( rSrcColor.val1 - rMeanColor.val1 ) > 100 )
				i = i;

			//retBuf[ i ] = colorArr[ rPE.nClustIndex ];
			retBuf[ i ] = rMeanColor;

			//if( rPE.bFlag_1 )
			//	retBuf[ i ].AssignVal( 0, 180, 0 );
		}

		return retImg;
	}


	void ImgDataSrc_GridPal::ClustHisto::Init( FixedVector< Cluster > & a_rClusterArr )
	{
		m_pClusterArr = & a_rClusterArr;

		FixedVector< Cluster > & rClusterArr = * m_pClusterArr;
		
		m_clustShareArr.SetSize( rClusterArr.GetSize() );

		for( int i=0; i < m_clustShareArr.GetSize(); i++ )		
		{
			ClustShareEx & rShare = m_clustShareArr[ i ];

			rShare.nClustIdx = i;
		}

		m_pFstShare = NULL;
		m_nListSize = 0;

		m_nMaxListSiz = 0;
	}






/////----------------------------------------------------------------



	ImgDataMgr_2_GridPal::ImgDataMgr_2_GridPal( 
		F32ImageRef a_srcImg, int a_nAprSiz, IImgCoreSrcRef a_dataSrc,
		ImgRotationMgrRef a_rot )
	{
		m_srcImg = a_srcImg;
		m_nAprSiz = a_nAprSiz;
		m_dataSrc = a_dataSrc;

		m_rot = a_rot;

		
		//ImgDataMgr_2_GridPal * dataMgr_CM;
		m_pDataSrc_LH = NULL;
		{
			IImgCoreSrc * pDS = (IImgCoreSrc *) m_dataSrc;

			m_pDataSrc_LH = dynamic_cast< ImgDataSrc_GridPal * >( pDS );

			if( NULL == m_pDataSrc_LH )
				ThrowHcvException();
		}


		PrepareData();
	}

	
	void ImgDataMgr_2_GridPal::PrepareData()
	{
		//ThrowHcvException();

		m_pClustHisto = & m_pDataSrc_LH->m_clustHisto;

		PreparePixEntArr();

		PrepareArrColl_V();
		PrepareArrColl_Tot();

		
		//ShowImage( GenResImg(), "GenResImg" );

	}


	F32ImageRef ImgDataMgr_2_GridPal::GenResImg()
	{
		FixedVector< F32ColorVal > colorArr;

		
		FixedVector< ImgDataSrc_GridPal::Cluster > & rClusterArr =
			*m_pClustHisto->m_pClusterArr;

		colorArr.SetSize( rClusterArr.GetSize() );

		for( int i=0; i < colorArr.GetSize(); i++ )
		{
			F32ColorVal & rColor = colorArr[ i ];

			rColor.val0 = rand() % 256;
			rColor.val1 = rand() % 256;
			rColor.val2 = rand() % 256;
		}

		//F32ImageRef retImg = F32Image::Create( m_srcImg->GetSize(), 3 );
		F32ImageRef retImg = F32Image::Create( m_rot->GetResImgSiz(), 3 );

		F32ColorVal * retBuf = (F32ColorVal *)retImg->GetPixAt( 0, 0 );

		for( int i=0; i < m_pixEntArr.GetSize(); i++ )
		{
			ImgDataSrc_GridPal::PixEnt & rPE = m_pixEntArr[ i ];

			Hcpl_ASSERT( rPE.nClustIndex >= 0 );

			F32ColorVal & rMeanColor = 
				//m_clusterArr[ rPE.nClustIndex ].GetMeanColor();
				rClusterArr[ rPE.nClustIndex ].OrgColor;

			//retBuf[ i ] = colorArr[ rPE.nClustIndex ];
			retBuf[ i ] = rMeanColor;

			//if( rPE.bFlag_1 )
			//	retBuf[ i ].AssignVal( 0, 180, 0 );
		}

		return retImg;
	}


	void ImgDataMgr_2_GridPal::PrepareArrColl_V()
	{
		CvSize imgSiz = m_srcImg->GetSize();

		IndexCalc2D idxCalc( imgSiz.width, imgSiz.height );

		
		int nAprSiz = m_nAprSiz;
		int nMarg = nAprSiz / 2;


		for( int x=0; x < imgSiz.width; x++ )
		{
			m_pClustHisto->Reset();

			int nSrcY = 0;

			for( ; nSrcY < nAprSiz; nSrcY++ )
			{
				{
					int nPI = idxCalc.Calc( x, nSrcY );

					ImgDataSrc_GridPal::PixEnt & rPE = m_pixEntArr[ nPI ];

					m_pClustHisto->IncAt( rPE.nClustIndex, 1 );
				}

			}

			for( int i=0; i < nMarg + 1; i++ )
			{
				//	Write Elm
				AddToArrColl( m_pixShareArrColl_V );
			}


			int nLimY = imgSiz.height - nMarg;

			int y = nMarg + 1;


			//for( int y = nMarg; y < nLimY; y++, nSrcY++ )
			for( ; y < nLimY; y++, nSrcY++ )
			{

				{
					int nPI = idxCalc.Calc( x, nSrcY );

					ImgDataSrc_GridPal::PixEnt & rPE = m_pixEntArr[ nPI ];

					m_pClustHisto->IncAt( rPE.nClustIndex, 1 );
				}

				{
					int nPI = idxCalc.Calc( x, nSrcY - nAprSiz );

					ImgDataSrc_GridPal::PixEnt & rPE = m_pixEntArr[ nPI ];
				
					m_pClustHisto->DecAt( rPE.nClustIndex, 1 );
				}
			
				{
					//	Write Elm
					AddToArrColl( m_pixShareArrColl_V );
				}
			}

			for( ; y < imgSiz.height; y++ )
			{
				//	Write Elm
				AddToArrColl( m_pixShareArrColl_V );
			}
		}




		float nAvgPixShares = (float) m_pixShareArrColl_V.GetDataSizeTot() / 
			m_pixShareArrColl_V.GetNofHeads();

	}

	
	void ImgDataMgr_2_GridPal::PrepareArrColl_Tot()
	{
		CvSize imgSiz = m_srcImg->GetSize();

		{
			int nTmp = imgSiz.width;
			imgSiz.width = imgSiz.height;
			imgSiz.height = nTmp;
		}

		IndexCalc2D idxCalc( imgSiz.width, imgSiz.height );

		int nAprSiz = 23;
		int nMarg = nAprSiz / 2;


		for( int x=0; x < imgSiz.width; x++ )
		{
			m_pClustHisto->Reset();

			int nSrcY = 0;

			for( ; nSrcY < nAprSiz; nSrcY++ )
			{
				{
					int nPI = idxCalc.Calc( x, nSrcY );

					IncHistoFrom_V( nPI );
				}

			}

			for( int i=0; i < nMarg + 1; i++ )
			{
				//	Write Elm
				AddToArrColl( m_pixShareArrColl_Tot );
			}


			int nLimY = imgSiz.height - nMarg;

			int y = nMarg + 1;


			//for( int y = nMarg; y < nLimY; y++, nSrcY++ )
			for( ; y < nLimY; y++, nSrcY++ )
			{

				{
					int nPI = idxCalc.Calc( x, nSrcY );

					IncHistoFrom_V( nPI );
				}

				{
					int nPI = idxCalc.Calc( x, nSrcY - nAprSiz );

					DecHistoFrom_V( nPI );
				}
			
				{
					//	Write Elm
					AddToArrColl( m_pixShareArrColl_Tot );
				}
			}

			for( ; y < imgSiz.height; y++ )
			{
				//	Write Elm
				AddToArrColl( m_pixShareArrColl_Tot );
			}
		}




		float nAvgPixShares = (float) m_pixShareArrColl_Tot.GetDataSizeTot() / 
			m_pixShareArrColl_Tot.GetNofHeads();

		float goodCnt = 0; 
		for( int i=0; i < m_pixShareArrColl_Tot.GetNofHeads(); i++ )
		{
			if( m_pixShareArrColl_Tot.GetSizeAt( i ) <= 25 )
				goodCnt++;
		}

		float goodRatio = goodCnt / m_pixShareArrColl_Tot.GetNofHeads();

	}


	void ImgDataMgr_2_GridPal::IncHistoFrom_V( int a_nVI )
	{
		//ThrowHcvException();

		Hcpl::CommonArrCollBuff< ImgDataSrc_GridPal::ClustShare > & rPixShareArrColl = 
			m_pixShareArrColl_V;

		ImgDataSrc_GridPal::ClustShare * shareBuf = rPixShareArrColl.GetHeadAt( a_nVI );
		int nofShares = rPixShareArrColl.GetSizeAt( a_nVI );

		for( int i=0; i < nofShares; i++ )
		{
			ImgDataSrc_GridPal::ClustShare & rShare = shareBuf[ i ];

			//int nOldMaxLS = m_pClustHisto->m_nMaxListSiz;

			m_pClustHisto->IncAt( rShare.nClustIdx, rShare.nofPixes );

			// bool b

			if( 110 == m_pClustHisto->m_nMaxListSiz )
			{
				CvSize imgSiz = m_srcImg->GetSize();
				
				IndexCalc2D idxCalc( imgSiz.height, imgSiz.width );

				int x = idxCalc.Calc_Y( a_nVI );
				int y = idxCalc.Calc_X( a_nVI );
			}

		}
	}


	void ImgDataMgr_2_GridPal::DecHistoFrom_V( int a_nVI )
	{
		//ThrowHcvException();

		Hcpl::CommonArrCollBuff< ImgDataSrc_GridPal::ClustShare > & rPixShareArrColl = 
			m_pixShareArrColl_V;

		ImgDataSrc_GridPal::ClustShare * shareBuf = rPixShareArrColl.GetHeadAt( a_nVI );
		int nofShares = rPixShareArrColl.GetSizeAt( a_nVI );

		for( int i=0; i < nofShares; i++ )
		{
			ImgDataSrc_GridPal::ClustShare & rShare = shareBuf[ i ];

			m_pClustHisto->DecAt( rShare.nClustIdx, rShare.nofPixes );
		}
	}


	void ImgDataMgr_2_GridPal::PreparePixEntArr()
	{
		CvSize orgSiz = m_rot->GetSrcImg()->GetSize();
		//int nRotSiz1D = rotSiz.width * rotSiz.height;

		CvSize rotSiz = m_rot->GetResImgSiz();
		int nRotSiz1D = rotSiz.width * rotSiz.height;

		FixedVector< ImgDataSrc_GridPal::PixEnt > 
			& rOrgEntArr = m_pDataSrc_LH->m_pixEntArr;

		//int * orgToLocMap_Buf = m_rot->Get_ResToSrcMapImage();
		int * locToOrgMap_Buf = (int *) m_rot->Get_ResToSrcMapImage(
			)->GetPixAt( 0, 0 );

		m_pixEntArr.SetSize( nRotSiz1D );

		for( int i=0; i < nRotSiz1D; i++ )
		{
			ImgDataSrc_GridPal::PixEnt & rPE = m_pixEntArr[ i ];
			rPE.nIndex = i;
			
			int nOI = locToOrgMap_Buf[ i ];

			if( nOI < 0 )
			{
				nOI = nOI;

				rPE.nClustIndex = 0;
			}
			else
			{
				nOI = nOI;

				rPE.nClustIndex = rOrgEntArr[ nOI ].nClustIndex;
			}

		}


		m_pixShareArrColl_V.SetCapacity( 
			m_pixEntArr.GetSize() * 10, m_pixEntArr.GetSize() );

		m_pixShareArrColl_Tot.SetCapacity( 
			//m_pixEntArr.GetSize() * 100, m_pixEntArr.GetSize() );
			m_pixEntArr.GetSize() * 30, m_pixEntArr.GetSize() );
	}


	void ImgDataMgr_2_GridPal::AddToArrColl( 
			Hcpl::CommonArrCollBuff< ImgDataSrc_GridPal::ClustShare > & rPixShareArrColl )
	{
		int nNewIdx = rPixShareArrColl.GetNofHeads();

		rPixShareArrColl.AddArr( m_pClustHisto->m_nListSize );

		ImgDataSrc_GridPal::ClustShare * newBuf = rPixShareArrColl.GetHeadAt( nNewIdx );

		ImgDataSrc_GridPal::ClustShareEx * pShareEx = m_pClustHisto->m_pFstShare;

		int i = 0;
		for( ; NULL != pShareEx; i++ )
		{
			ImgDataSrc_GridPal::ClustShare & rShare = newBuf[ i ];

			rShare.nClustIdx = pShareEx->nClustIdx;
			rShare.nofPixes = pShareEx->nofPixes;

			pShareEx = pShareEx->pNext;
		}

		Hcpl_ASSERT( i == m_pClustHisto->m_nListSize );
	}

	void ImgDataSrc_GridPal::ClustHisto::Reset()
	{
		for( ClustShareEx * pShare = m_pFstShare;
			NULL != pShare; pShare = m_pFstShare )
		{
			DecAt( pShare->nClustIdx, pShare->nofPixes );
		}

	} 

	void ImgDataSrc_GridPal::ClustHisto::IncAt( int a_nIdx, int a_nIncVal )
	{
		ClustShareEx & rShare = m_clustShareArr[ a_nIdx ];

		int nOldNofPixes = rShare.nofPixes;


		rShare.nofPixes += a_nIncVal;

		if( 0 == nOldNofPixes )
		{
			AddShareToList( rShare );

			if( 110 == m_nListSize )
				m_nListSize = m_nListSize;

			if( m_nListSize > m_nMaxListSiz )
				m_nMaxListSiz = m_nListSize;

			if( m_nListSize > 40 )
				m_nListSize = m_nListSize;
		}
	}

	void ImgDataSrc_GridPal::ClustHisto::DecAt( int a_nIdx, int a_nDecVal )
	{
		ClustShareEx & rShare = m_clustShareArr[ a_nIdx ];

		rShare.nofPixes -= a_nDecVal;

		Hcpl_ASSERT( rShare.nofPixes >= 0 );

		if( 0 == rShare.nofPixes )
		{
			RemoveShareFromList( rShare );
		}
	}

	void ImgDataSrc_GridPal::ClustHisto::AddShareToList( ClustShareEx & a_rShare )
	{
		if( NULL == m_pFstShare )
		{
			m_pFstShare = & a_rShare;
			m_nListSize++;
			return;
		}

		m_pFstShare->pPrev = & a_rShare;

		a_rShare.pNext = m_pFstShare;
		
		m_pFstShare = & a_rShare;
		m_nListSize++;
	}

	void ImgDataSrc_GridPal::ClustHisto::RemoveShareFromList( ClustShareEx & a_rShare )
	{
		ClustShareEx * pShare = & a_rShare;

		ClustShareEx * pPrev = pShare->pPrev;
		ClustShareEx * pNext = pShare->pNext;

		if( NULL != pPrev )
		{
			pPrev->pNext = pNext;
		}
		//if( NULL == pPrev )
		else
		{
			m_pFstShare = pNext;
		}


		if( NULL != pNext )
		{
			pNext->pPrev = pPrev;
		}

		//if( NULL == pNext && NULL == pPrev )
		//{
		//	m_pFstShare = NULL;
		//}

		pShare->pPrev = NULL;
		pShare->pNext = NULL;

		Hcpl_ASSERT( m_nListSize > 0 );
		m_nListSize--;
	}





}