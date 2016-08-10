#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\ScanTryMgr2.h>

#define ARR_SIZ 3

namespace Hcv
{
	using namespace Hcpl;
	//using namespace Hcpl::Math;


	void ScanTryMgr2::PrepareEnv( F32ImageRef a_src )
	{
		m_sigDrawVect.SetCapacity( 300 );

		//const int nRadius = GlobalStuff::AprSize1D / 2;
		const int nRadius = 3;

		const int nDiameter = nRadius * 2 + 1;


		//m_src = GlobalStuff::GetLinePathImg();
		m_src = a_src;

		CvSize srcSiz = m_src->GetSize();

		const int nSrcSiz1D = srcSiz.width * srcSiz.height;


		{
			m_maxDifArr.SetSize( nSrcSiz1D );

			m_slopeOfMaxDifArr.SetSize( nSrcSiz1D );

			for( int i=0; i < m_maxDifArr.GetSize(); i++ )
			{
				m_maxDifArr[ i ] = 0;
				m_slopeOfMaxDifArr[ i ] = 100;
			}
		}

		{
			m_pixInfoArr.SetSize( nSrcSiz1D );

			for( int i=0; i < m_pixInfoArr.GetSize(); i++ )
			{
				EdgeScan::PixInfo & rPI = m_pixInfoArr[ i ];

				rPI.nIndex = i;
			}
		}


		m_edgeInfoArr.SetCapacity( ( srcSiz.width * srcSiz.height ) / 2 );

		m_nMaxScanSize = srcSiz.width + srcSiz.height;

		m_scanData = new SimpleFilt1D2DataMgr< float >( m_nMaxScanSize );
		m_locData = new SimpleFilt1D2DataMgr< float >( m_nMaxScanSize );
		m_angDifData = new SimpleFilt1D2DataMgr< float >( m_nMaxScanSize );

		m_ioa_ptrDataMgr = new Ptr_IntFilt1D2DataMgr( NULL, m_nMaxScanSize );

		//m_ioaData = new SimpleFilt1D2DataMgr< int >( m_nMaxScanSize );
		m_ioaData = ( IIntFilt1D2DataMgrRef ) m_ioa_ptrDataMgr;

		m_finDif_ptrDataMgr = new Ptr_SingFilt1D2DataMgr( NULL, m_nMaxScanSize );
		
		m_finDifData = ( ISingFilt1D2DataMgrRef ) m_finDif_ptrDataMgr;

		m_isMaxDirArr.SetCapacity( m_nMaxScanSize );


		m_imgGradMgr = new ImgGradMgr( m_src, nDiameter, 8 );		

		S32ImageRef dirImg = m_imgGradMgr->GetGradDirImg();
		m_dirBuf = dirImg->GetPixAt( 0, 0 );


		m_sdmColl = m_imgGradMgr->GetScanDirMgrColl();

		{
			m_scanDirExtArr.SetSize( m_sdmColl->GetNofMgrs() );
			

			for( int i=0; i < m_scanDirExtArr.GetSize(); i++ )
			{
				m_scanDirExtArr[ i ] = new EdgeScan::ScanDirMgrExt( 
					m_sdmColl->GetMgrAt( i ) );
			}
		}

		m_difScale = 10;
		{
			float maxDif = (float) sqrt( (float) Sqr( 255 ) * 3 );

			m_edgeProcAction_Ques.InitSize( ( maxDif + 10 ) * m_difScale );

			m_edgePA_Arr.SetCapacity( ( srcSiz.width * srcSiz.height ) / 2 );
		}





		m_colorScan.AssignVal( 255, 0, 0 );
		m_colorLoc.AssignVal( 0, 180, 0 );
	}


	void ScanTryMgr2::FillLineData( ImgGradMgr::ScanLineItrRef a_sli )
	{
		a_sli->GotoFirst();


		int nDataCnt = 0;

		float * scanBuf = m_scanData->GetData();
		float * locBuf = m_locData->GetData();
		float * angDifBuf = m_angDifData->GetData();

		FixedVector< float > & gradDataArr = m_imgGradMgr->GetGradDataArr();

		const int nofDirs = m_sdmColl->GetNofMgrs();

		const int nDir = a_sli->GetDirIndex();
		

		{
			ScanDirMgr & rSm = m_sdmColl->GetMgrAt( nDir );

			m_ioa_ptrDataMgr->SetDataParams( 
				rSm.Get_ioaHeadAt( a_sli->GetLineNum() ),
				rSm.Get_ioaSizeAt( a_sli->GetLineNum() ) 
			);
		}

		{
			//ScanDirMgr & rSm = m_sdmColl->GetMgrAt( nDir );
			EdgeScan::ScanDirMgrExtRef smExt = m_scanDirExtArr[ nDir ];

			m_finDif_ptrDataMgr->SetDataParams( 
				smExt->Get_difHeadAt( a_sli->GetLineNum() ),
				smExt->Get_difSizeAt( a_sli->GetLineNum() ) 
			);
		}


		do
		{
			const int nCPI = a_sli->GetCurPI();

			//ioaBuf[ nDataCnt ] = nCPI;

			F32ColorVal color1;

			const int nCurDir = m_dirBuf[ nCPI ];

			{	
				const int nValIdx = nDir + nCPI * nofDirs;
				float val = gradDataArr[ nValIdx ];

				scanBuf[ nDataCnt ] = val;
			}


			{	
				const int nValIdx = nCurDir + nCPI * nofDirs;							
				float val = gradDataArr[ nValIdx ];

				locBuf[ nDataCnt ] = val;
			}


			float angDif = m_sdmColl->GetDirAngDif( nDir, nCurDir );

			angDifBuf[ nDataCnt ] = angDif;

			nDataCnt++;
		}
		while( a_sli->MoveNext() );

		{
			for( int i=0; i < nDataCnt; i++ )
			{
				if( 0 == locBuf[ i ] )
					angDifBuf[ i ] = 0;
			}
		}


		m_scanData->SetSize( nDataCnt );
		m_locData->SetSize( nDataCnt );
		m_angDifData->SetSize( nDataCnt );			

		m_ioaData->SetSize( nDataCnt );

		m_finDifData->SetSize( nDataCnt );
	}


//uuuuuuuuuuuuuuuuuu



	void ScanTryMgr2::Show_Edges_Roots( )
	{
		float * angDifBuf = m_angDifData->GetData();

		
		CvSize srcSiz = m_src->GetSize();


		//a_sli->GotoFirst();

		F32ImageRef dspImg = m_src->Clone();

		F32ColorVal * dspImgBuf = (F32ColorVal *) dspImg->GetPixAt( 0, 0 );


		//CvSize srcSiz = m_src->GetSize();

		for( int i=0; i < m_edgeInfoArr.GetSize(); i++ )
		{
			EdgeScan::EdgeInfo & rEI = m_edgeInfoArr[ i ];

			if( rEI.IsCanceled )
				continue;

			F32Point pntR1 = F32Point::CreateFromIndex(
				//rEI.R1_IOA, srcSiz );
				//rEI.ioa_HeadPtr[ rEI.Root1_IOL ], srcSiz );
				rEI.ioa_HeadPtr[ rEI.Med1_IOL ], srcSiz );

			DrawCircle( dspImg, F32Point( pntR1.x, pntR1.y ), 
				u8ColorVal( 255, 0, 0 ), 1 );

			F32Point pntR2 = F32Point::CreateFromIndex(
				//rEI.R2_IOA, srcSiz );
				//rEI.ioa_HeadPtr[ rEI.Root2_IOL ], srcSiz );
				rEI.ioa_HeadPtr[ rEI.Med2_IOL ], srcSiz );

			DrawCircle( dspImg, F32Point( pntR2.x, pntR2.y ), 
				u8ColorVal( 255, 0, 0 ), 1 );
		}

		for( int i=0; i < m_edgeInfoArr.GetSize(); i++ )
		{
			EdgeScan::EdgeInfo & rEI = m_edgeInfoArr[ i ];

			if( rEI.IsCanceled )
				continue;

			F32Point pntR1 = F32Point::CreateFromIndex(
				//rEI.R1_IOA, srcSiz );
				//rEI.ioa_HeadPtr[ rEI.Root1_IOL ], srcSiz );
				rEI.ioa_HeadPtr[ rEI.InrR1_IOL ], srcSiz );

			DrawCircle( dspImg, F32Point( pntR1.x, pntR1.y ), 
				u8ColorVal( 0, 0, 255 ), 2 );

			F32Point pntR2 = F32Point::CreateFromIndex(
				//rEI.R2_IOA, srcSiz );
				//rEI.ioa_HeadPtr[ rEI.Root2_IOL ], srcSiz );
				rEI.ioa_HeadPtr[ rEI.InrR2_IOL ], srcSiz );

			DrawCircle( dspImg, F32Point( pntR2.x, pntR2.y ), 
				u8ColorVal( 0, 0, 255 ), 2 );
		}

		for( int i=0; i < m_edgeInfoArr.GetSize(); i++ )
		{
			EdgeScan::EdgeInfo & rEI = m_edgeInfoArr[ i ];

			if( rEI.IsCanceled )
				continue;

			F32Point pntEdge = F32Point::CreateFromIndex(
				rEI.ioa_HeadPtr[ rEI.EdgeIOL ], srcSiz );

			DrawCircle( dspImg, F32Point( pntEdge.x, pntEdge.y ), 
				u8ColorVal( 0, 255, 0 ), 2 );
		}

		ShowImage( dspImg, "Edges_Roots" );
	}




	void ScanTryMgr2::DspLineOnImg( int a_x, int a_y,
		ImgGradMgr::ScanLineItrRef a_sli )
	{
		float * angDifBuf = m_angDifData->GetData();

		
		CvSize srcSiz = m_src->GetSize();

		a_sli->GotoFirst();

		F32ImageRef dspImg = m_src->Clone();

		F32ColorVal * dspImgBuf = (F32ColorVal *) dspImg->GetPixAt( 0, 0 );

		int i = 0;

		do
		{
			const int nCPI = a_sli->GetCurPI();

			const float angDif = angDifBuf[ i ];

			F32ColorVal color1;

			//if( angDif < 15 )
				color1 = m_colorScan;
			//else
				//color1 = m_colorLoc;

			F32Point pnt1 = F32Point::CreateFromIndex( nCPI, srcSiz );
			DrawCircle( dspImg, pnt1, color1.ToU8(), 2 );

			i++;
		}
		while( a_sli->MoveNext() );


		DrawCircle( dspImg, F32Point( a_x, a_y ), 
			u8ColorVal( 0, 0, 255 ), 2 );
	

		GlobalStuff::ShowLinePathImg( dspImg );
	}




	ScanTryMgr2::ScanTryMgr2( int a_x, int a_y )
	{
		const int nLastLA_Index = GlobalStuff::m_nLA_Index;

		//a_x = 117;
		//a_y = 222;
		
		//a_x = 142;
		//a_y = 441;
		
		//a_x = 145;
		//a_y = 183;
		
		//a_x = 755;
		//a_y = 278;
		
		//a_x = 778;
		//a_y = 257;
		
		//a_x = 781;
		//a_y = 277;
		
		//a_x = 770;
		//a_y = 188;
		
		//a_x = 581;
		//a_y = 345;
		




		PrepareEnv( GlobalStuff::GetLinePathImg() );

		CvSize srcSiz = m_src->GetSize();

		ScanTryDbg::ScanTryDbgCore * pScanDbgCore = &ScanTryDbg::Core;


		//int nIndex_XY;
		{
			IndexCalc2D ic( srcSiz.width, srcSiz.height );

			pScanDbgCore->nIOA_XY = ic.Calc( a_x, a_y );
		}

		{
			pScanDbgCore->Reset();

			pScanDbgCore->CreateMarkImg( srcSiz );

			pScanDbgCore->markCentPnt = F32Point( a_x, a_y );

			//pScanDbgCore->MarkPoint( a_x, a_y, 25 );
			//pScanDbgCore->MarkPoint( a_x, a_y, 7 );

			pScanDbgCore->MarkPoint( a_x, a_y, 5 );

			//pScanDbgCore->MarkPoint( a_x, a_y, 3 );

			//ShowImage( pScanDbgCore->GetMarkImg(), "MarkImg" );
		}



		//PrepareFiltSys();
		PrepareFiltSys_2();


		Find_Edges_Roots();

		//PostProcessEI_Arr();
		PostProcessEI_Arr_2();

		Show_Edges_Roots();

		//ShowImage( GenMaxSlopeImg(), "MaxSlopeImg" );
		//ShowImage( GenMinSlopeImg(), "MinSlopeImg" );

/*
		if( NULL != pScanDbgCore->SliMark )
		{
			F32Point pnt1 = F32Point::CreateFromIndex( 
				pScanDbgCore->nIOA, srcSiz );
			
			//ScanSli( pnt1.x, pnt1.y, 
			ScanPntDir( pnt1.x, pnt1.y, 				
				pScanDbgCore->SliMark );
		}
*/

		ScanPntDir( a_x, a_y );


		//ShowImage( GenFinDifImg( 6 ), "FinDifImg" );
		//ShowImage( GenSlopeImg( 0 ), "SlopeImg" );
		

	}




	ScanTryMgr2::ScanTryMgr2( F32ImageRef a_src )
	{

		PrepareEnv( a_src );

		CvSize srcSiz = m_src->GetSize();


		ScanTryDbg::Core.Reset();

		ScanTryDbg::Core.CreateMarkImg( srcSiz );

/*
		//int nIndex_XY;
		{
			IndexCalc2D ic( srcSiz.width, srcSiz.height );

			ScanTryDbg::Core.nIOA_XY = ic.Calc( a_x, a_y );
		}

		{
			ScanTryDbg::Core.Reset();

			ScanTryDbg::Core.CreateMarkImg( srcSiz );

			ScanTryDbg::Core.markCentPnt = F32Point( a_x, a_y );

			//ScanTryDbg::Core.MarkPoint( a_x, a_y, 25 );
			//ScanTryDbg::Core.MarkPoint( a_x, a_y, 7 );

			ScanTryDbg::Core.MarkPoint( a_x, a_y, 5 );

			//ScanTryDbg::Core.MarkPoint( a_x, a_y, 3 );

			//ShowImage( ScanTryDbg::Core.GetMarkImg(), "MarkImg" );
		}
*/


		//PrepareFiltSys();
		PrepareFiltSys_2();


		Find_Edges_Roots();

		//PostProcessEI_Arr();
		PostProcessEI_Arr_2();

/*
		Show_Edges_Roots();

		//ShowImage( GenMaxSlopeImg(), "MaxSlopeImg" );
		//ShowImage( GenMinSlopeImg(), "MinSlopeImg" );


		if( NULL != ScanTryDbg::Core.SliMark )
		{
			F32Point pnt1 = F32Point::CreateFromIndex( 
				ScanTryDbg::Core.nIOA, srcSiz );
			
			//ScanSli( pnt1.x, pnt1.y, 
			ScanPntDir( pnt1.x, pnt1.y, 				
				ScanTryDbg::Core.SliMark );
		}


		//ScanPntDir( a_x, a_y );


		//ShowImage( GenFinDifImg( 6 ), "FinDifImg" );
		//ShowImage( GenSlopeImg( 0 ), "SlopeImg" );
		
*/
	}


	void ScanTryMgr2::ScanSli( int a_x, int a_y,
			ImgGradMgr::ScanLineItrRef a_sli )
	{
		//PrepareAperSizes();

		FillLineData( a_sli );

		m_filtSys->Proceed();


		ShowSignals();

		DspLineOnImg( a_x, a_y, a_sli );
	}


	void ScanTryMgr2::ScanPntDir( int a_x, int a_y,
			ImgGradMgr::ScanLineItrRef a_sli )
	{
		ImgGradMgr & igm1 = *m_imgGradMgr;




		CvSize srcSiz = m_src->GetSize();

		IndexCalc2D idx2D( srcSiz.width, srcSiz.height );

		const int nPI = idx2D.Calc( a_x, a_y );

		int nDir = GlobalStuff::AprSize1D;

		if( nDir < 0 || nDir > 7 )
			nDir = m_dirBuf[ nPI ];

		

		m_nScanDir = nDir;



		F32ImageRef difMagImg = igm1.GetGradMagImg();
		

		ScanDirMgr & rScanMgr = m_sdmColl->GetMgrAt( nDir );

		FixedVector< ScanDirMgr::PixLineInfo > & pliArr =
			rScanMgr.GetPixLineInfoArr();

		ScanDirMgr::PixLineInfo & rPli = pliArr[ nPI ];

		//if( NULL != a_sli )
			//rPli.nIOL = a_sli->GetCurIOL();

		//const int nBPI = rScanMgr.GetBgnIndexArr()[ rPli.nLineNum ];

		//const int nStart = rScanMgr.GetShiftStartArr()[ rPli.nLineNum ];

		FixedVector< int > & rShiftArr  = rScanMgr.GetShiftIndexArr();


		//ImgGradMgr::ScanLineItrRef sli = igm1.GenScanLineItr( nPI, nDir );
		//ImgGradMgr::ScanLineItrRef sli = igm1.GenScanLineItr( nBPI, nDir );

		ImgGradMgr::ScanLineItrRef sliXY;

		if( NULL != a_sli )
			sliXY = a_sli;
		else
			sliXY = igm1.GenScanLineItr_FromLine( rPli.nLineNum, nDir );

		if( NULL != a_sli )
		{
			m_sigPosDelta = DeltaSignal::CreateAt( a_sli->GetCurIOL() );

			AddSigDraw( m_sigPosDelta, true, u8ColorVal( 0, 0, 255 ) );
		}
		else
		{
			const int nSI = rScanMgr.GetShiftStartArr()[ rPli.nLineNum ];

			m_sigPosDelta = DeltaSignal::CreateAt( rPli.nIOL - nSI );

			AddSigDraw( m_sigPosDelta, true, u8ColorVal( 0, 0, 255 ) );
		}


		ScanSli( a_x, a_y, sliXY );


	}




	void ScanTryMgr2::Find_Edges_Roots()
	{
		ImgGradMgr & igm1 = *m_imgGradMgr;

		ImgGradMgr::ScanLineItrRef sli;

		for( int i=0; i < m_sdmColl->GetNofMgrs(); i++ )
		{
			m_nScanDir = i;

			//PrepareAperSizes();

			ScanDirMgr & rScanMgr_2 = m_sdmColl->GetMgrAt( i );

			EdgeScan::ScanDirMgrExtRef smExt = m_scanDirExtArr[ i ];

			smExt->nEI_Bgn = m_edgeInfoArr.GetSize();

			const int nofLines = rScanMgr_2.GetBgnIndexArr().GetSize();

			for( int j=0; j < nofLines; j++ )
			{
				sli = igm1.GenScanLineItr_FromLine( j, i );

				ScanTryDbg::Core.SliCur = sli;

				FillLineData( sli );

				const int nofEI_BefLine = m_edgeInfoArr.GetSize();

				m_filtSys->Proceed();

				PostProcScanLine( sli, nofEI_BefLine );
			}

			smExt->nEI_Lim = m_edgeInfoArr.GetSize();
		}
	}


	void ScanTryMgr2::PostProcScanLine( ImgGradMgr::ScanLineItrRef a_sli,
			int a_nofEIs_BefLine )
	{
		const int nLineSiz = m_ioa_ptrDataMgr->GetSize();

		const int nDir = a_sli->GetDirIndex();

		EdgeScan::ScanDirMgrExtRef smExt = m_scanDirExtArr[ nDir ];

		float * finDifGlob_Buf = & smExt->m_difGlobArr[ 0 ];
		 


		int * ioa_Buf = m_ioa_ptrDataMgr->GetData();

		float * finDifBuf = m_finDifData->GetData();


		for( int i=0; i < nLineSiz; i++ )
		{
			const float difVal = finDifBuf[ i ];

			const int nIOA = ioa_Buf[ i ];

			finDifGlob_Buf[ nIOA ] = difVal;

			if( difVal > m_maxDifArr[ nIOA ] )
			{
				m_maxDifArr[ nIOA ] = difVal;

				m_isMaxDirArr[ i ] = true;
			}
			else
			{
				m_isMaxDirArr[ i ] = false;
			}

		}

		float * slopeGlob_Buf = & smExt->m_slopeGlobArr[ 0 ];

		if( m_edgeInfoArr.GetSize() == a_nofEIs_BefLine )
		{
			for( int i=0; i < nLineSiz; i++ )
			{
				const int nIOA = ioa_Buf[ i ];

				//slopeGlob_Buf[ nIOA ] = 0;
				slopeGlob_Buf[ nIOA ] = 100;
			}

			return;
		}

		{
			EdgeScan::EdgeInfo & rEI_Bgn = m_edgeInfoArr[ a_nofEIs_BefLine ];

			int nBgnIOL = rEI_Bgn.Root1_IOL;

			for( int i=0; i < nBgnIOL; i++ )
			{
				const int nIOA = ioa_Buf[ i ];

				//slopeGlob_Buf[ nIOA ] = 0;
				slopeGlob_Buf[ nIOA ] = 100;
			}
		}

		{
			EdgeScan::EdgeInfo & rEI_End = m_edgeInfoArr.GetBack();

			int nEndIOL = rEI_End.Root2_IOL;

			for( int i = nEndIOL + 1; i < nLineSiz; i++ )
			{
				const int nIOA = ioa_Buf[ i ];

				slopeGlob_Buf[ nIOA ] = 0;
			}
		}

		for( int i = a_nofEIs_BefLine; i < m_edgeInfoArr.GetSize(); i++ )
		{
			EdgeScan::EdgeInfo & rEI = m_edgeInfoArr[ i ];

			float * difBuf = rEI.dif_HeadPtr;

			const float difVal = difBuf[ rEI.EdgeIOL ] ;

			const float difValBef = difBuf[ rEI.Root1_IOL ];

			const float difValAft = difBuf[ rEI.Root2_IOL ];


			{
				const float difNetBef = difVal - difValBef;

				bool bNotFound_Med = true;
				bool bNotFound_Inr = true;

				rEI.Med1_IOL = rEI.Root1_IOL;

				rEI.InrR1_IOL = rEI.Root1_IOL;

				for( int i = rEI.EdgeIOL; i >= rEI.Root1_IOL; i-- )
				{
					const float valNetI = difBuf[ i ] - difValBef;

					float valSlope = valNetI / difNetBef;

					valSlope *= 100;

					int nIOA_I = ioa_Buf[ i ];

					slopeGlob_Buf[ nIOA_I ] = valSlope;

					if( m_isMaxDirArr[ i ] )
						m_slopeOfMaxDifArr[ nIOA_I ] = valSlope;

					if( bNotFound_Med )
					{
						if( valSlope < 35 )
						{
							rEI.Med1_IOL = i;

							//slopeGlob_Buf[ nIOA_I ] = 255;

							bNotFound_Med = false;
						}
					}

					if( bNotFound_Inr )
					{
						if( valSlope < 15 )
						{
							rEI.InrR1_IOL = i;

							//slopeGlob_Buf[ nIOA_I ] = 255;

							bNotFound_Inr = false;
						}
					}

				}
			}


			{
				const float difNetAft = difVal - difValAft;

				bool bNotFound_Med = true;
				bool bNotFound_Inr = true;

				rEI.Med2_IOL = rEI.Root1_IOL;

				rEI.InrR2_IOL = rEI.Root1_IOL;

				for( int i = rEI.EdgeIOL + 1; i <= rEI.Root2_IOL; i++ )
				{
					const float valNetI = difBuf[ i ] - difValAft;

					float valSlope = valNetI / difNetAft;

					valSlope *= 100;

					int nIOA_I = ioa_Buf[ i ];

					slopeGlob_Buf[ nIOA_I ] = valSlope;

					if( m_isMaxDirArr[ i ] )
						m_slopeOfMaxDifArr[ nIOA_I ] = valSlope;

					if( bNotFound_Med )
					{
						if( valSlope < 35 )
						{
							rEI.Med2_IOL = i;

							//slopeGlob_Buf[ nIOA_I ] = 255;

							bNotFound_Med = false;
						}
					}
				
					if( bNotFound_Inr )
					{
						if( valSlope < 15 )
						{
							rEI.InrR2_IOL = i;

							//slopeGlob_Buf[ nIOA_I ] = 255;

							bNotFound_Inr = false;
						}
					}

				}
			}


		}

	}

	EdgeScanMgr::DataDraw * ScanTryMgr2::AddSigDraw( ISingFilt1D2Ref a_filt, 
		bool a_bDraw, U8ColorVal a_color, int a_shiftX, int a_shiftY, float a_scale )
	{
		m_filtSys->GetFiltArr().PushBack( (IFilt1D2Ref) a_filt );

		return AddSigDraw( a_filt->GetOutputReader(), 
			a_bDraw, a_color, a_shiftX, a_shiftY, a_scale );
	}


	void ScanTryMgr2::ShowSignals()
	{
		Signal1DViewer sv1;

		for( int i=0; i < m_sigDrawVect.GetSize(); i++ )
		{
			DataDraw & rSD = m_sigDrawVect[ i ];

			if( ! rSD.ShouldDraw )
				continue;

			int nDataSiz = rSD.DataRdr->GetSize();

			//sv1.AddSignal( GenSignal( rSD.DataRdr, rSD.ShiftX, rSD.ShiftY ), rSD.Color );
			sv1.AddSignal( GenSignal( rSD.DataRdr ), rSD.Color,
				rSD.Scale, rSD.ShiftX, - rSD.ShiftY );
		}


		ShowImage( sv1.GenDisplayImage(), "G D L S" );
	}





	void ScanTryMgr2::PrepareFiltSys()
	{
		int nShiftY_1 = 100;
		int nShiftY_2 = 180;
		

		m_filtSys = new Filt1D2SysEmpty( m_nMaxScanSize );
		

		{
			//AddSigDraw( ToRdr( m_angDifData ), true, u8ColorVal( 255, 0, 255 ), 0, nShiftY_1 - 20 );
			AddSigDraw( ToRdr( m_angDifData ), false, u8ColorVal( 255, 0, 255 ) );

			//AddSigDraw( Signal1DBuilder::CreateConstSignal( 
				//0, GetLastDraw().DataRdr->GetSize() ),
				//false, u8ColorVal( 255, 0, 255 ) );

			AddSigDraw( new AvgFilt1D2( 5, GetLastDraw().DataRdr ),
				//true, GetLastDraw().Color, 0, nShiftY_2 );
				true, GetLastDraw().Color, 0, 0 );
				//false, GetLastDraw().Color );
		}
		ISingFilt1D2DataReaderRef angDifRdr = GetLastDraw().DataRdr;

/*
		{
			AddSigDraw( Signal1DBuilder::CreateConstSignal(
				0, GetLastDraw().DataRdr->GetSize() ), 
				true, u8ColorVal( 0, 0, 0 ), 0, nShiftY_2 );
		}
*/

///////////////////////


		{
			//AddSigDraw( ToRdr( m_locData ), true, m_colorLoc.ToU8() );
			AddSigDraw( ToRdr( m_locData ), false, m_colorLoc.ToU8() );

			AddSigDraw( new AvgFilt1D2( 5, GetLastDraw().DataRdr ),
				true, GetLastDraw().Color, 0, GetLastDraw().ShiftY );
		}
		ISingFilt1D2DataReaderRef locRdr = GetLastDraw().DataRdr;


		{
			//AddSigDraw( ToRdr( m_scanData ), false, m_colorScan.ToU8() );
			AddSigDraw( ToRdr( m_scanData ), true, m_colorScan.ToU8() );
		}


		{
			{
				//AddSigDraw( new MaxFilt1D2( 3, GetLastDraw().DataRdr ),
					//false, GetLastDraw().Color, 0, GetLastDraw().ShiftY );
			}


			{
				//const int nAprSiz = 3;
				const int nAprSiz = 5;
				//const int nAprSiz = 7;

				const int nAvgCnt = 1;

				for( int i=0; i < nAvgCnt; i++ )
				{
					ISingFilt1D2DataMgrRef outData = NULL;

					if( nAvgCnt - 1 == i )
						outData = m_finDifData;					

					AddSigDraw( new AvgFilt1D2( nAprSiz, GetLastDraw().DataRdr, outData ),
						false, GetLastDraw().Color, 0, GetLastDraw().ShiftY );
				}

				GetLastDraw().ShouldDraw = true;
			}
			ISingFilt1D2DataReaderRef scanRdr = GetLastDraw().DataRdr;


			{
				//AddSigDraw( new DifFilt1D2( 7, GetLastDraw().DataRdr ), 
				AddSigDraw( new DifFilt1D2( 5, GetLastDraw().DataRdr ), 
					//true, GetLastDraw().Color, 0, nShiftY_1, 3 );
					//true, u8ColorVal( 0, 100, 155 ), 0, nShiftY_2, 1 );				
					false, GetLastDraw().Color, 0, nShiftY_1, 3 );

				//AddSigDraw( new AvgFilt1D2( 5, GetLastDraw().DataRdr ), 
					//true, u8ColorVal( 255, 255, 255 ), 0, GetLastDraw().ShiftY, 4 );
			}
			ISingFilt1D2DataReaderRef dif2LocRdr = GetLastDraw().DataRdr;


			DataDraw * pDif3Draw = NULL;
			{
				//pDif3Draw = AddSigDraw( new DifFilt1D2( 7, GetLastDraw().DataRdr ), 
				pDif3Draw = AddSigDraw( new DifFilt1D2( 5, GetLastDraw().DataRdr ), 
					//true, u8ColorVal( 0, 255, 255 ), 0, nShiftY_1, 3 );
					false, u8ColorVal( 0, 255, 255 ), 0, nShiftY_1, 3 );

				//AddSigDraw( new AvgFilt1D2( 5, GetLastDraw().DataRdr ), 
					//true, GetLastDraw().Color, 0, GetLastDraw().ShiftY, GetLastDraw().Scale );
			}
			ISingFilt1D2DataReaderRef dif3LocRdr = GetLastDraw().DataRdr;


			IIntFilt1D2DataReaderRef ZCPosRdr = NULL;
			{
				ZeroCrossingFilt1D2 * filt1 = new ZeroCrossingFilt1D2(
					5, dif2LocRdr );

				ZCPosRdr = filt1->GetZCPosDataRdr();

				AddSigDraw( filt1, 
					//true, m_colorLoc.ToU8(), 0, nShiftY_2 );
					true, u8ColorVal( 255, 255, 255 ), 0, nShiftY_2 );
					//false, m_colorLoc.ToU8(), 0, nShiftY_2 );
			}


			{
				AddSigDraw( new PeakMarkerFilt1D2( 5, scanRdr, locRdr, dif2LocRdr, 
					angDifRdr, GetLastDraw().DataRdr, ZCPosRdr, ToRdr( m_ioaData ), 
					m_edgeInfoArr, m_pixInfoArr ), 
					//true, pDif3Draw->Color, 0, nShiftY_2 + 10 );
					true, pDif3Draw->Color, 0, 0 );
					//false, pDif3Draw->Color, 0, nShiftY_2 + 10 );
			}

		}


///////////////////////

		//AddSigDraw( DeltaSignal::CreateAt( 12 ), 
			//true, u8ColorVal( 0, 0, 255 ) );
	}



	void ScanTryMgr2::PrepareFiltSys_2()
	{
		int nShiftY_1 = 100;
		int nShiftY_2 = 180;
		

		m_filtSys = new Filt1D2SysEmpty( m_nMaxScanSize );
		

		{
			AddSigDraw( ToRdr( m_angDifData ), true, u8ColorVal( 255, 0, 255 ), 0, nShiftY_1 );
			//AddSigDraw( ToRdr( m_angDifData ), false, u8ColorVal( 255, 0, 255 ) );

			//AddSigDraw( Signal1DBuilder::CreateConstSignal( 
				//0, GetLastDraw().DataRdr->GetSize() ),
				//false, u8ColorVal( 255, 0, 255 ) );

/*
			AddSigDraw( new AvgFilt1D2( 5, GetLastDraw().DataRdr ),
				true, GetLastDraw().Color, 0, nShiftY_1 );
				//true, GetLastDraw().Color, 0, 0 );
				//false, GetLastDraw().Color );
*/
		}
		ISingFilt1D2DataReaderRef angDifRdr = GetLastDraw().DataRdr;

/*
		{
			AddSigDraw( Signal1DBuilder::CreateConstSignal(
				0, GetLastDraw().DataRdr->GetSize() ), 
				true, u8ColorVal( 0, 0, 0 ), 0, nShiftY_2 );
		}
*/

///////////////////////


		{
			//AddSigDraw( ToRdr( m_locData ), true, m_colorLoc.ToU8() );
			AddSigDraw( ToRdr( m_locData ), false, m_colorLoc.ToU8() );

			AddSigDraw( new AvgFilt1D2( 5, GetLastDraw().DataRdr ),
				true, GetLastDraw().Color, 0, GetLastDraw().ShiftY );
		}
		ISingFilt1D2DataReaderRef locRdr = GetLastDraw().DataRdr;


		{
			AddSigDraw( ToRdr( m_scanData ), false, m_colorScan.ToU8() );
			//AddSigDraw( ToRdr( m_scanData ), true, m_colorScan.ToU8() );
		}


		{
			{
				//AddSigDraw( new MaxFilt1D2( 3, GetLastDraw().DataRdr ),
					//false, GetLastDraw().Color, 0, GetLastDraw().ShiftY );
			}


			{
				//const int nAprSiz = 3;
				const int nAprSiz = 5;
				//const int nAprSiz = 7;

				const int nAvgCnt = 1;

				for( int i=0; i < nAvgCnt; i++ )
				{
					ISingFilt1D2DataMgrRef outData = NULL;

					if( nAvgCnt - 1 == i )
						outData = m_finDifData;					

					AddSigDraw( new AvgFilt1D2( nAprSiz, GetLastDraw().DataRdr, outData ),
						false, GetLastDraw().Color, 0, GetLastDraw().ShiftY );
				}

				GetLastDraw().ShouldDraw = true;
			}
			ISingFilt1D2DataReaderRef scanRdr = GetLastDraw().DataRdr;


			{
				//AddSigDraw( new DifFilt1D2( 7, GetLastDraw().DataRdr ), 
				AddSigDraw( new DifFilt1D2( 5, GetLastDraw().DataRdr ), 
					//true, GetLastDraw().Color, 0, nShiftY_1, 3 );
					//true, u8ColorVal( 0, 100, 155 ), 0, nShiftY_1, 3 );				
					false, GetLastDraw().Color, 0, nShiftY_1, 3 );

				//AddSigDraw( new AvgFilt1D2( 5, GetLastDraw().DataRdr ), 
					//true, u8ColorVal( 255, 255, 255 ), 0, GetLastDraw().ShiftY, 4 );
			}
			ISingFilt1D2DataReaderRef dif2LocRdr = GetLastDraw().DataRdr;


			DataDraw * pDif3Draw = NULL;
			{
				//pDif3Draw = AddSigDraw( new DifFilt1D2( 7, GetLastDraw().DataRdr ), 
				pDif3Draw = AddSigDraw( new DifFilt1D2( 5, GetLastDraw().DataRdr ), 
					//true, u8ColorVal( 0, 255, 255 ), 0, nShiftY_1, 3 );
					false, u8ColorVal( 0, 255, 255 ), 0, nShiftY_1, 3 );

				//AddSigDraw( new AvgFilt1D2( 5, GetLastDraw().DataRdr ), 
					//true, GetLastDraw().Color, 0, GetLastDraw().ShiftY, GetLastDraw().Scale );
			}
			ISingFilt1D2DataReaderRef dif3LocRdr = GetLastDraw().DataRdr;


			IIntFilt1D2DataReaderRef ZCPosRdr = NULL;
			{
				ZeroCrossing_2_Filt1D2 * filt1 = new ZeroCrossing_2_Filt1D2(
				//ZeroCrossingFilt1D2 * filt1 = new ZeroCrossingFilt1D2(
					5, dif2LocRdr );

				ZCPosRdr = filt1->GetZCPosDataRdr();

				AddSigDraw( filt1, 
					//true, m_colorLoc.ToU8(), 0, nShiftY_2 );
					//true, u8ColorVal( 255, 255, 255 ), 0, nShiftY_2 );
					false, u8ColorVal( 255, 255, 255 ), 0, nShiftY_1 );
					//false, m_colorLoc.ToU8(), 0, nShiftY_2 );
			}


			ISingFilt1D2DataReaderRef pk_out_2 = NULL;
			{
				PeakMarkerFilt1D2 * filt1 = new PeakMarkerFilt1D2( 5, scanRdr, locRdr, 
					dif2LocRdr, angDifRdr, GetLastDraw().DataRdr, ZCPosRdr, ToRdr( m_ioaData ), 
					m_edgeInfoArr, m_pixInfoArr );

				pk_out_2 = filt1->GetOutput_2_Reader();

				AddSigDraw( filt1, 
					//true, pDif3Draw->Color, 0, nShiftY_2 + 10 );
					true, pDif3Draw->Color, 0, nShiftY_1 );
					//false, pDif3Draw->Color, 0, 0 );

				AddSigDraw( pk_out_2, 
						true, u8ColorVal( 255, 255, 255 ), 0, 0 );
					//false, pDif3Draw->Color, 0, 0 );
			}

		}


///////////////////////

		//AddSigDraw( DeltaSignal::CreateAt( 12 ), 
			//true, u8ColorVal( 0, 0, 255 ) );
	}


	void ScanTryMgr2::PrepareAperSizes()
	{
		FixedVector< IFilt1D2Ref > & rFiltArr = m_filtSys->GetFiltArr();

		ScanDirMgr & rScanMgr = m_sdmColl->GetMgrAt( m_nScanDir );

		for( int i=0; i < rFiltArr.GetSize(); i++ )
		{
			IFilt1D2Ref filt = rFiltArr[ i ];

			int nAprSizOrg = filt->GetAprSizeOrg();

			int nAprSizAct = rScanMgr.CalcAprSizAct( nAprSizOrg );

			filt->SetAprSize( nAprSizAct );

			//SetOutSize Bef Aft Filt Proceed
		}

	}


	void ScanTryMgr2::PostProcessEI_Arr_2()
	{
		const int nofDirs = m_scanDirExtArr.GetSize();

		//float * markBuf = pScanDbgCore->GetMarkImg()->GetPixAt( 0, 0 );
		float * markBuf = ScanTryDbg::Core.GetMarkImg()->GetPixAt( 0, 0 );

		MinIndexFinder< float > minFnd;

		float * maxDif_Buf = & m_maxDifArr[ 0 ];

		float * slopeOfMax_Buf = & m_slopeOfMaxDifArr[ 0 ];


		F32ColorVal * colorBuf = (F32ColorVal *) m_src->GetPixAt( 0, 0 );

		CvSize srcSiz = m_src->GetSize();

		for( int nDir = 0; nDir < nofDirs; nDir++ )
		{
			EdgeScan::ScanDirMgrExtRef smExt = m_scanDirExtArr[ nDir ];

			float * difBuf = & smExt->m_difGlobArr[ 0 ];

			float * slopeBuf = & smExt->m_slopeGlobArr[ 0 ];


			const int nDirNorm = 
				( nDir + ( nofDirs / 2 ) ) % nofDirs;

			EdgeScan::ScanDirMgrExtRef smExtNorm = m_scanDirExtArr[ nDirNorm ];

			float * difBuf_Norm = & smExtNorm->m_difGlobArr[ 0 ];

			float * slopeBuf_Norm = & smExtNorm->m_slopeGlobArr[ 0 ];


			for( int i = smExt->nEI_Bgn; i < smExt->nEI_Lim; i++ )
			{
				EdgeScan::EdgeInfo & rEI = m_edgeInfoArr[ i ];

				//if( 11741 == rEI.Index )
				if( 1285 == rEI.Index )
					i = i;

				int * ioaBuf = rEI.ioa_HeadPtr;

				const float difVal_Edge = difBuf[ ioaBuf[ rEI.EdgeIOL ] ];

				{
					const int nIOA = ioaBuf[ rEI.Med1_IOL ];

					float difRatio = difBuf_Norm[ nIOA ] / difBuf[ nIOA ];
					//float difRatio = maxDif_Buf[ nIOA ] / difBuf[ nIOA ];


					if( difRatio < 1 )
						difRatio = difRatio;

					if( difRatio > 0.7 )
					//if( difRatio > 1.4 )
					{
						//if( slopeBuf_Norm[ nIOA ] > 30 )
						if( slopeBuf_Norm[ nIOA ] > 10 )
						//if( slopeOfMax_Buf[ nIOA ] > 10 )							
						{
							rEI.IsCanceled = true;
							continue;
						}
					}

				}

				{
					const int nIOA = ioaBuf[ rEI.Med2_IOL ];

					float difRatio = difBuf_Norm[ nIOA ] / difBuf[ nIOA ];
					//float difRatio = maxDif_Buf[ nIOA ] / difBuf[ nIOA ];


					if( difRatio < 1 )
						difRatio = difRatio;

					if( difRatio > 0.7 )
					//if( difRatio > 1.4 )
					{
						//if( slopeBuf_Norm[ nIOA ] > 30 )
						if( slopeBuf_Norm[ nIOA ] > 10 )
						//if( slopeOfMax_Buf[ nIOA ] > 10 )							
						{
							rEI.IsCanceled = true;
							continue;
						}
					}

				}

////////////////////////////////////////////////////////////////////////////////
/*
				{
					const int nIOA = ioaBuf[ rEI.InrR1_IOL ];

					float difRatio = difBuf_Norm[ nIOA ] / difBuf[ nIOA ];
					//float difRatio = maxDif_Buf[ nIOA ] / difBuf[ nIOA ];


					if( difRatio < 1 )
						difRatio = difRatio;

					if( difRatio > 0.7 )
					//if( difRatio > 1.4 )
					{
						//if( slopeBuf_Norm[ nIOA ] > 30 )
						if( slopeBuf_Norm[ nIOA ] > 10 )
						//if( slopeOfMax_Buf[ nIOA ] > 10 )							
						{
							rEI.IsCanceled = true;
							continue;
						}
					}

				}

				{
					const int nIOA = ioaBuf[ rEI.InrR2_IOL ];

					float difRatio = difBuf_Norm[ nIOA ] / difBuf[ nIOA ];
					//float difRatio = maxDif_Buf[ nIOA ] / difBuf[ nIOA ];


					if( difRatio < 1 )
						difRatio = difRatio;

					if( difRatio > 0.7 )
					//if( difRatio > 1.4 )
					{
						//if( slopeBuf_Norm[ nIOA ] > 30 )
						if( slopeBuf_Norm[ nIOA ] > 10 )
						//if( slopeOfMax_Buf[ nIOA ] > 10 )							
						{
							rEI.IsCanceled = true;
							continue;
						}
					}

				}
*/
////////////////////////////////////////////////////////////////////////////////




				{
					minFnd.Reset();

					//rEI.InrR1_IOL = rEI.Root1_IOL;

					//for( int i = rEI.Med1_IOL; i >= rEI.Root1_IOL; i-- )
					//for( int i = rEI.InrR1_IOL; i >= rEI.Root1_IOL; i-- )
					for( int i = rEI.Med1_IOL; i >= rEI.InrR1_IOL; i-- )
					{
						int nIOA_I = ioaBuf[ i ];

						const float val = m_maxDifArr[ nIOA_I ];

						minFnd.AddValue( val, i );

					}

					rEI.InrR1_IOL = minFnd.FindMinIdx();
				}

				{
					minFnd.Reset();

					//rEI.InrR2_IOL = rEI.Root2_IOL;

					//for( int i = rEI.Med2_IOL; i <= rEI.Root2_IOL; i++ )
					//for( int i = rEI.InrR2_IOL; i >= rEI.Root1_IOL; i-- )
					for( int i = rEI.Med2_IOL; i <= rEI.InrR2_IOL; i++ )
					{
						int nIOA_I = ioaBuf[ i ];

						const float val = m_maxDifArr[ nIOA_I ];

						minFnd.AddValue( val, i );

					}

					rEI.InrR2_IOL = minFnd.FindMinIdx();
				}


				{
					int nIOA_1 = ioaBuf[ rEI.InrR1_IOL ];

					F32ColorVal * pColor1 = & colorBuf[ nIOA_1 ];

					int nIOA_2 = ioaBuf[ rEI.InrR2_IOL ];

					F32ColorVal * pColor2 = & colorBuf[ nIOA_2 ];

					float colorDif = F32ColorVal::Sub( *pColor1, *pColor2 ).CalcMag();

					if( colorDif / difVal_Edge < 0.8 )
					{
						rEI.IsCanceled = true;
						continue;
					}
					

				}


////////////////////////////////////////////////////////////////////////////////

				{
					int nIOA_InrR1 = ioaBuf[ rEI.InrR1_IOL ];

					if( markBuf[ nIOA_InrR1 ] > 1 )
					{
						F32Point pnt1 = F32Point::CreateFromIndex( 
							nIOA_InrR1, srcSiz );

						F32Point & rCentPnt = ScanTryDbg::Core.markCentPnt;

						float distToCent = F32Point::Sub( rCentPnt, pnt1 ).CalcMag();

						//if( distToCent < ScanTryDbg::Core.markDistToCent )
						if( false )
						{
							ScanTryDbg::Core.markDistToCent = distToCent;

							ScanTryDbg::Core.markClosePnt = pnt1;

							ScanTryDbg::Core.nEI_Index = rEI.Index;

							//ScanTryDbg::Core.nIOL = rEI.InrR1_IOL;

							ScanTryDbg::Core.SliMark = 
								m_imgGradMgr->GenScanLineItr_FromLine( 
									rEI.LineNum, rEI.Dir, rEI.InrR1_IOL );						
						}
					}
				}

				{
					int nIOA_Edge = ioaBuf[ rEI.EdgeIOL ];

					if( markBuf[ nIOA_Edge ] > 1 )
					{
						F32Point pnt1 = F32Point::CreateFromIndex( 
							nIOA_Edge, srcSiz );

						F32Point & rCentPnt = ScanTryDbg::Core.markCentPnt;

						float distToCent = F32Point::Sub( rCentPnt, pnt1 ).CalcMag();

						if( distToCent < ScanTryDbg::Core.markDistToCent )
						{
							ScanTryDbg::Core.markDistToCent = distToCent;

							ScanTryDbg::Core.markClosePnt = pnt1;

							ScanTryDbg::Core.nEI_Index = rEI.Index;

							//ScanTryDbg::Core.nIOL = rEI.EdgeIOL;

							ScanTryDbg::Core.SliMark = 
								m_imgGradMgr->GenScanLineItr_FromLine( 
								rEI.LineNum, rEI.Dir, rEI.EdgeIOL );
						}
					}
				}

				{
					int nIOA_InrR2 = ioaBuf[ rEI.InrR2_IOL ];

					if( markBuf[ nIOA_InrR2 ] > 1 )
					{
						F32Point pnt1 = F32Point::CreateFromIndex( 
							nIOA_InrR2, srcSiz );

						F32Point & rCentPnt = ScanTryDbg::Core.markCentPnt;

						float distToCent = F32Point::Sub( rCentPnt, pnt1 ).CalcMag();

						//if( distToCent < ScanTryDbg::Core.markDistToCent )
						if( false )
						{
							ScanTryDbg::Core.markDistToCent = distToCent;

							ScanTryDbg::Core.markClosePnt = pnt1;

							ScanTryDbg::Core.nEI_Index = rEI.Index;

							//ScanTryDbg::Core.nIOL = rEI.InrR2_IOL;

							ScanTryDbg::Core.SliMark = 
								m_imgGradMgr->GenScanLineItr_FromLine( 
									rEI.LineNum, rEI.Dir, rEI.InrR2_IOL );
						}
					}
				}

			}

		}

	}


	void ScanTryMgr2::PostProcessEI_Arr()
	{
		//float slopePeak_Thr = 65;
		float slopePeak_Thr = 60;

		//float slopeDif_Thr = 15;
		float slopeDif_Thr = 8;

		F32ColorVal * colorBuf = (F32ColorVal *) m_src->GetPixAt( 0, 0 );

		ScanTryDbg::ScanTryDbgCore * pScanDbgCore = &ScanTryDbg::Core;

		float * markBuf = pScanDbgCore->GetMarkImg()->GetPixAt( 0, 0 );

		for( int i=0; i < m_edgeInfoArr.GetSize(); i++ )
		{
			EdgeScan::EdgeInfo & rEI = m_edgeInfoArr[ i ];

			if( rEI.IsCanceled )
				continue;
	
			m_edgePA_Arr.IncSize();

			EdgeScan::EdgeProcAction & rEPA = m_edgePA_Arr.GetBack();

			rEPA.pEdgeInfo = & rEI;

			//float edgeVal = rEI.EdgeVal;
			float edgeVal;
			{
				int nIOA_R1 = rEI.ioa_HeadPtr[ rEI.Root1_IOL ];
				int nIOA_R2 = rEI.ioa_HeadPtr[ rEI.Root2_IOL ];

				F32ColorVal & rColorR1 = colorBuf[ nIOA_R1 ];
				F32ColorVal & rColorR2 = colorBuf[ nIOA_R2 ];

				edgeVal = F32ColorVal::Sub( rColorR1, rColorR2 ).CalcMag();				
			}

			int nQueIndex = (int)( edgeVal * m_difScale );

			m_edgeProcAction_Ques.PushPtr( nQueIndex, & rEPA );
		}


		FixedVector< EdgeScan::PixInfo > & rPixInfoArr = m_pixInfoArr;

		int nMyEI_Idx = -1;

		int a;

		//for( int i=0; i < m_edgeInfoArr.GetSize(); i++ )
		while( true )
		{

			EdgeScan::EdgeProcAction * pEPA = m_edgeProcAction_Ques.PopPtrMax();

			if( NULL == pEPA )
			{
				break;
			}


			//EdgeScan::EdgeInfo & rEI = m_edgeInfoArr[ i ];
			EdgeScan::EdgeInfo & rEI = *( pEPA->pEdgeInfo );

			if( 11695 == rEI.Index )
				a = 0;

			if( rEI.IsCanceled )
				continue;

			float * difBuf = rEI.dif_HeadPtr;

			int * ioaBuf = rEI.ioa_HeadPtr;


			float difVal = difBuf[ rEI.EdgeIOL ] ;

			const float difValBef = difBuf[ rEI.Root1_IOL ];

			const float difValAft = difBuf[ rEI.Root2_IOL ];


			{
				const float difNetBef = difVal - difValBef;

				bool bNotFound = true;

				for( int i = rEI.EdgeIOL - 1; i >= rEI.Root1_IOL; i-- )
				{
					const float valNetI = difBuf[ i ] - difValBef;

					float valSlope = valNetI / difNetBef;

					valSlope *= 100;

					int nIOA_I = ioaBuf[ i ];

					EdgeScan::PixInfo & rPI = rPixInfoArr[ nIOA_I ];


					if( valSlope > slopePeak_Thr &&
						rPI.minSlopeVal < slopePeak_Thr )
					{
						if( valSlope - rPI.minSlopeVal > slopeDif_Thr )
						{
							//rEI.IsCanceled = true;
							//continue;
						}

					}
					else if( rPI.maxSlopeVal > slopePeak_Thr &&
						valSlope < slopePeak_Thr )
					{
						if( rPI.maxSlopeVal - valSlope > slopeDif_Thr )
						{
							//rEI.IsCanceled = true;
							//continue;
						}

					}


					if( valSlope > rPI.maxSlopeVal )
						rPI.maxSlopeVal = valSlope;

					if( valSlope < rPI.minSlopeVal )
						rPI.minSlopeVal = valSlope;

					if( bNotFound )
					{
						if( valSlope < 15 )
						{
							//rEI.Root1_IOL = i;

							bNotFound = false;
						}
					}
				}

			}

			//if( rEI.IsCanceled )
				//continue;

			{
				const float difNetAft = difVal - difValAft;

				bool bNotFound = true;

				for( int i = rEI.EdgeIOL + 1; i <= rEI.Root2_IOL; i++ )
				{
					const float valNetI = difBuf[ i ] - difValAft;

					float valSlope = valNetI / difNetAft;

					valSlope *= 100;

					int nIOA_I = ioaBuf[ i ];

					EdgeScan::PixInfo & rPI = rPixInfoArr[ nIOA_I ];

					if( valSlope > slopePeak_Thr &&
						rPI.minSlopeVal < slopePeak_Thr )
					{
						if( valSlope - rPI.minSlopeVal > slopeDif_Thr )
						{
							//rEI.IsCanceled = true;
							//continue;
						}

					}
					else if( rPI.maxSlopeVal > slopePeak_Thr &&
						valSlope < slopePeak_Thr )
					{
						if( rPI.maxSlopeVal - valSlope > slopeDif_Thr )
						{
							//rEI.IsCanceled = true;
							//continue;
						}

					}



					if( valSlope > rPI.maxSlopeVal )
						rPI.maxSlopeVal = valSlope;

					if( valSlope < rPI.minSlopeVal )
						rPI.minSlopeVal = valSlope;

					if( bNotFound )
					{
						if( valSlope < 15 )
						{
							//rEI.Root2_IOL = i;

							bNotFound = false;
						}
					}
				}

			}

			const int nR1_IOA = ioaBuf[ rEI.Root1_IOL ];

			if( markBuf[ nR1_IOA ] > 1 )
			{
				pScanDbgCore->SliMark = 
//						pScanDbgCore->SliCur;
					m_imgGradMgr->GenScanLineItr_FromLine( rEI.LineNum, rEI.Dir );

				//pScanDbgCore->nIOA = rEI.R1_IOA;
				pScanDbgCore->nIOA = nR1_IOA;

				nMyEI_Idx = rEI.Index;
			}


			const int nR2_IOA = ioaBuf[ rEI.Root2_IOL ];

			if( markBuf[ nR2_IOA ] > 1 )
			{
				pScanDbgCore->SliMark = 
//						pScanDbgCore->SliCur;
					m_imgGradMgr->GenScanLineItr_FromLine( rEI.LineNum, rEI.Dir );

				//pScanDbgCore->nIOA = rEI.R2_IOA;
				pScanDbgCore->nIOA = nR2_IOA;

				nMyEI_Idx = rEI.Index;
			}

		}

		//int a;
		a = 0;
	}


	F32ImageRef ScanTryMgr2::GenSlopeImg( int a_nDir )
	{
		CvSize srcSiz = m_src->GetSize();

		F32ImageRef ret = F32Image::Create( srcSiz, 1 );

		float * retBuf = ret->GetPixAt( 0, 0 );

		EdgeScan::ScanDirMgrExtRef smExt = m_scanDirExtArr[ a_nDir ];

		FixedVector< float > & rSlopeGlobArr = smExt->m_slopeGlobArr;

		for( int i=0; i < rSlopeGlobArr.GetSize(); i++ )
		{
			float val = rSlopeGlobArr[ i ];

			if( val < -5 )
				//val = val;
				val = 255;

			retBuf[ i ] = val;
		}

		return ret;
	}


	F32ImageRef ScanTryMgr2::GenFinDifImg( int a_nDir )
	{
		CvSize srcSiz = m_src->GetSize();

		F32ImageRef ret = F32Image::Create( srcSiz, 1 );

		float * retBuf = ret->GetPixAt( 0, 0 );

		EdgeScan::ScanDirMgrExtRef smExt = m_scanDirExtArr[ a_nDir ];

		FixedVector< float > & rDifGlobArr = smExt->m_difGlobArr;

		for( int i=0; i < rDifGlobArr.GetSize(); i++ )
		{
			float val = rDifGlobArr[ i ];

			retBuf[ i ] = val;
		}

		return ret;
	}


	F32ImageRef ScanTryMgr2::GenMaxSlopeImg()
	{
		CvSize srcSiz = m_src->GetSize();

		F32ImageRef ret = F32Image::Create( srcSiz, 1 );

		float * retBuf = ret->GetPixAt( 0, 0 );

		for( int i=0; i < m_pixInfoArr.GetSize(); i++ )
		{
			float val = m_pixInfoArr[ i ].maxSlopeVal
				* 255 / 100;

			if( val > 50 )
			//if( val > 95 )
				val = 255;
			else
				val = 0;

			retBuf[ i ] = val;
		}

		return ret;
	}


	F32ImageRef ScanTryMgr2::GenMinSlopeImg()
	{
		CvSize srcSiz = m_src->GetSize();

		F32ImageRef ret = F32Image::Create( srcSiz, 1 );

		float * retBuf = ret->GetPixAt( 0, 0 );

		for( int i=0; i < m_pixInfoArr.GetSize(); i++ )
		{
			float val = ( 100 - m_pixInfoArr[ i ].minSlopeVal )
				* 255 / 100;

			//if( val > 30 )
			//if( val > 95 )
			//	val = 255;
			//else
			//	val = 0;

			retBuf[ i ] = val;
		}

		return ret;
	}


}


