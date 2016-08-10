#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>


#include <Lib\Hcv\ScanTryMgr_6.h>
#include <Lib\Hcv\ImgDataMgr_Simple.h>



#define ARR_SIZ 3

namespace Hcv
{
	using namespace Hcpl;
	//using namespace Hcpl::Math;


	void ScanTryMgr_6::PrepareEnv( IImgDataMgrRef a_src, int a_nDif_Diameter )
	{
		m_someIOA_Arr.SetCapacity( 80 );

		m_sigDrawVect.SetCapacity( 300 );

		const int nDiameter = a_nDif_Diameter;

		EdgeScan::PixInfo * pPI_Dbg = & m_pixInfoArr[ 100928 ];



		//m_src = GlobalStuff::GetLinePathImg();
		m_src = a_src;

		m_standDiv_Buf = m_src->GetStandDiv_Mag_Image(
			)->GetPixAt(0, 0);

		CvSize srcSiz = m_src->GetSize();

		const int nSrcSiz1D = srcSiz.width * srcSiz.height;


		{
			m_maxDifArr.SetSize( nSrcSiz1D );

			m_maxEI_Arr.SetSize( nSrcSiz1D );

			m_slopeOfMaxDifArr.SetSize( nSrcSiz1D );

			for( int i=0; i < nSrcSiz1D; i++ )
			{
				m_maxDifArr[ i ] = 0;
				m_slopeOfMaxDifArr[ i ] = 100;
				m_maxEI_Arr[ i ] = NULL;
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

		EdgeScan::EdgeInfo * pEI_Dbg = & m_edgeInfoArr[ 206 ];


		//EdgeScan::EdgeInfo * pEI_Dbg = & m_edgeInfoArr[ 62643 ];

		m_nMaxScanSize = srcSiz.width + srcSiz.height;

		m_scanData = new SimpleFilt1D2DataMgr< float >( m_nMaxScanSize );
		m_scanData_2 = new SimpleFilt1D2DataMgr< float >( m_nMaxScanSize );		
		m_stDivLocData = new SimpleFilt1D2DataMgr< float >( m_nMaxScanSize );
		
		m_locData = new SimpleFilt1D2DataMgr< float >( m_nMaxScanSize );
		m_angDifData = new SimpleFilt1D2DataMgr< float >( m_nMaxScanSize );
		m_confData = new SimpleFilt1D2DataMgr< float >( m_nMaxScanSize );
		m_mag_Data = new SimpleFilt1D2DataMgr< float >( m_nMaxScanSize );		
		m_standDivData = new SimpleFilt1D2DataMgr< float >( m_nMaxScanSize );
		m_standDiv_R_Data = new SimpleFilt1D2DataMgr< float >( m_nMaxScanSize );
		m_standDiv_R12_Data = new SimpleFilt1D2DataMgr< float >( m_nMaxScanSize );
		

		m_ioa_ptrDataMgr = new Ptr_IntFilt1D2DataMgr( NULL, m_nMaxScanSize );

		//m_ioaData = new SimpleFilt1D2DataMgr< int >( m_nMaxScanSize );
		m_ioaData = ( IIntFilt1D2DataMgrRef ) m_ioa_ptrDataMgr;

		m_finDif_ptrDataMgr = new Ptr_SingFilt1D2DataMgr( NULL, m_nMaxScanSize );
		
		m_finDifData = ( ISingFilt1D2DataMgrRef ) m_finDif_ptrDataMgr;

		//m_isMaxDirArr.SetCapacity( m_nMaxScanSize );
		m_isMaxDirArr.SetSize( m_nMaxScanSize );


		m_imgGradMgr = new ImgGradMgr_2( m_src, 
		//m_imgGradMgr = new ImgAvgingMgr( m_src, 
			nDiameter, 8 );		
			//nDiameter * 2 + 1, 8 );		

		m_igm_2 = new ImgGradMgr_2( m_src, 5, 8 );		

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


	void ScanTryMgr_6::FillLineData( Ns_ImgGradMgr::ScanLineItrRef a_sli )
	{
		a_sli->GotoFirst();


		int nDataCnt = 0;

		float * scanBuf = m_scanData->GetData();
		float * scanBuf_2 = m_scanData_2->GetData();

		float * stDivLocBuf = m_stDivLocData->GetData();		

		float * locBuf = m_locData->GetData();
		float * angDifBuf = m_angDifData->GetData();
		//float * confSig_Buf = m_confData->GetData();		
		float * mag_Buf = m_mag_Data->GetData();
		//float * standDivSig_Buf = m_standDivData->GetData();
		//float * standDivSig_R_Buf = m_standDiv_R_Data->GetData();
		//float * standDivSig_R12_Buf = m_standDiv_R12_Data->GetData();
		
		float * magImg_Buf = m_src->Get_Mag_Image()->GetPixAt( 0, 0 );

		float * stDivImg_Buf = m_src->GetStandDiv_Mag_Image()->GetPixAt( 0, 0 );

		FixedVector< float > & gradDataArr = m_imgGradMgr->GetGradDataArr();

		FixedVector< float > & gradDataArr_2 = m_igm_2->GetGradDataArr();
				

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
				float val_2 = gradDataArr_2[ nValIdx ];

				float magVal = magImg_Buf[ nCPI ];

				float stDiv_Val = stDivImg_Buf[ nCPI ];

				
				

				//	Hthm Tmp
				//if(4 != nDir)
					//val = 0;

				scanBuf[ nDataCnt ] = val;

				scanBuf_2[ nDataCnt ] = val_2;

				stDivLocBuf[ nDataCnt ] = stDiv_Val;

				//confSig_Buf[ nDataCnt ] = val;

				mag_Buf[ nDataCnt ] = magVal;
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
		m_scanData_2->SetSize( nDataCnt );

		m_stDivLocData->SetSize( nDataCnt );

		m_locData->SetSize( nDataCnt );
		m_angDifData->SetSize( nDataCnt );		
		m_confData->SetSize( nDataCnt );		
		m_mag_Data->SetSize( nDataCnt );
		m_standDivData->SetSize( nDataCnt );
		m_standDiv_R_Data->SetSize( nDataCnt );
		m_standDiv_R12_Data->SetSize( nDataCnt );
		
		
		
		m_ioaData->SetSize( nDataCnt );

		m_finDifData->SetSize( nDataCnt );
	}


//uuuuuuuuuuuuuuuuuu



	void ScanTryMgr_6::Show_Edges_Roots( )
	{
		float * angDifBuf = m_angDifData->GetData();

		
		CvSize srcSiz = m_src->GetSize();


		//a_sli->GotoFirst();

//--		F32ImageRef dspImg = m_src->Clone();

		F32ImageRef dspImg = F32Image::Create(m_src->GetSize(), 3);

		F32ColorVal * dspImgBuf = (F32ColorVal *) dspImg->GetPixAt( 0, 0 );


		//CvSize srcSiz = m_src->GetSize();

		for( int i=0; i < m_edgeInfoArr.GetSize(); i++ )
		{
			EdgeScan::EdgeInfo & rEI = m_edgeInfoArr[ i ];

			if( rEI.IsCanceled )
				continue;

			//if( ! rEI.DoesConflict() )
			//	continue;

			F32Point pntEdge = F32Point::CreateFromIndex(
				rEI.ioa_HeadPtr[ rEI.EdgeIOL ], srcSiz );

			DrawCircle( dspImg, F32Point( pntEdge.x, pntEdge.y ), 
				u8ColorVal( 255, 0, 0 ), 2 );
		}

		for( int i=0; i < m_edgeInfoArr.GetSize(); i++ )
		{
			EdgeScan::EdgeInfo & rEI = m_edgeInfoArr[ i ];

			if( rEI.IsCanceled )
				continue;

			F32Point pntR1 = F32Point::CreateFromIndex(
				rEI.ioa_HeadPtr[ rEI.InrR1_IOL ], srcSiz );

			DrawCircle( dspImg, F32Point( pntR1.x, pntR1.y ), 
				u8ColorVal( 0, 0, 255 ), 2 );

			F32Point pntR2 = F32Point::CreateFromIndex(
				rEI.ioa_HeadPtr[ rEI.InrR2_IOL ], srcSiz );

			DrawCircle( dspImg, F32Point( pntR2.x, pntR2.y ), 
				u8ColorVal( 0, 0, 255 ), 2 );
		}

		//for( int i=0; i < m_edgeInfoArr.GetSize(); i++ )
		//{
		//	EdgeScan::EdgeInfo & rEI = m_edgeInfoArr[ i ];

		//	if( rEI.IsCanceled )
		//		continue;

		//	F32Point pntMed1 = F32Point::CreateFromIndex(
		//		rEI.ioa_HeadPtr[ rEI.Med1_IOL ], srcSiz );

		//	DrawCircle( dspImg, F32Point( pntMed1.x, pntMed1.y ), 
		//		u8ColorVal( 180, 180, 180 ), 1 );

		//	F32Point pntMed2 = F32Point::CreateFromIndex(
		//		rEI.ioa_HeadPtr[ rEI.Med2_IOL ], srcSiz );

		//	DrawCircle( dspImg, F32Point( pntMed2.x, pntMed2.y ), 
		//		u8ColorVal( 180, 180, 180 ), 1 );
		//}



//		m_standDiv_Buf = m_src->GetStandDiv_Mag_Image(
//			)->GetPixAt(0, 0);


		F32Point testPnt( 531, 170 );



		for( int i=0; i < m_edgeInfoArr.GetSize(); i++ )
		{
			EdgeScan::EdgeInfo & rEI = m_edgeInfoArr[ i ];

			if( rEI.IsCanceled )
				continue;


			{
				int nIOA_R = rEI.ioa_HeadPtr[ rEI.InrR1_IOL ];

				EdgeScan::PixInfo * pPI = & m_pixInfoArr[ nIOA_R ];

				int nIOA_DeepR = pPI->nIOA_VeryDeep_R;

				int nIOA_DeepMed = pPI->nIOA_VeryDeep_Med;

				F32Point pntMed1 = F32Point::CreateFromIndex(
					nIOA_DeepMed, srcSiz );

				DrawCircle( dspImg, F32Point( pntMed1.x, pntMed1.y ), 
					u8ColorVal( 180, 180, 180 ), 2 );
			}

			{
				int nIOA_R = rEI.ioa_HeadPtr[ rEI.InrR2_IOL ];

				EdgeScan::PixInfo * pPI = & m_pixInfoArr[ nIOA_R ];

				int nIOA_DeepR = pPI->nIOA_VeryDeep_R;

				int nIOA_DeepMed = pPI->nIOA_VeryDeep_Med;

				F32Point pntMed1 = F32Point::CreateFromIndex(
					nIOA_DeepMed, srcSiz );

				DrawCircle( dspImg, F32Point( pntMed1.x, pntMed1.y ), 
					u8ColorVal( 180, 180, 180 ), 2 );
			}
		}


		for( int i=0; i < m_edgeInfoArr.GetSize(); i++ )
		{
			EdgeScan::EdgeInfo & rEI = m_edgeInfoArr[ i ];

			if( rEI.IsCanceled )
				continue;

			{
				int nIOA_R = rEI.ioa_HeadPtr[ rEI.InrR1_IOL ];

				EdgeScan::PixInfo * pPI = & m_pixInfoArr[ nIOA_R ];

				int nIOA_DeepR = pPI->nIOA_VeryDeep_R;

				F32Point pntR = F32Point::CreateFromIndex(
					nIOA_DeepR, srcSiz );


				//if( F32Point::Sub( testPnt, pntR1 ).CalcMag() < 3 ) 
				if( false ) 
				{
					i = i;

					{
						F32Point pnt1 = F32Point::CreateFromIndex(
							rEI.ioa_HeadPtr[ rEI.InrR2_IOL ], srcSiz );

						DrawCircle( dspImg, F32Point( pnt1.x, pnt1.y ), 
							u8ColorVal( 255, 255, 255 ), 4 );
					}

					{
						F32Point pnt1 = F32Point::CreateFromIndex(
							rEI.ioa_HeadPtr[ rEI.EdgeIOL ], srcSiz );

						DrawCircle( dspImg, F32Point( pnt1.x, pnt1.y ), 
							u8ColorVal( 255, 255, 255 ), 4 );
					}

				}

				DrawCircle( dspImg, F32Point( pntR.x, pntR.y ), 
					u8ColorVal( 0, 255, 0 ), 2 );
			}

			{
				int nIOA_R = rEI.ioa_HeadPtr[ rEI.InrR2_IOL ];

				EdgeScan::PixInfo * pPI = & m_pixInfoArr[ nIOA_R ];

				int nIOA_DeepR = pPI->nIOA_VeryDeep_R;

				F32Point pntR = F32Point::CreateFromIndex(
					nIOA_DeepR, srcSiz );

				//if( F32Point::Sub( testPnt, pntR2 ).CalcMag() < 3 ) 
				if( false ) 
				{
					i = i;

					{
						F32Point pnt1 = F32Point::CreateFromIndex(
							rEI.ioa_HeadPtr[ rEI.InrR2_IOL ], srcSiz );

						DrawCircle( dspImg, F32Point( pnt1.x, pnt1.y ), 
							u8ColorVal( 255, 255, 255 ), 4 );
					}

					{
						F32Point pnt1 = F32Point::CreateFromIndex(
							rEI.ioa_HeadPtr[ rEI.EdgeIOL ], srcSiz );

						DrawCircle( dspImg, F32Point( pnt1.x, pnt1.y ), 
							u8ColorVal( 255, 255, 255 ), 4 );
					}


				}

				DrawCircle( dspImg, F32Point( pntR.x, pntR.y ), 
					u8ColorVal( 0, 255, 0 ), 2 );
			}
		}


		

		for( int i=0; i < m_someIOA_Arr.GetSize(); i++ )
		{
			F32Point pnt1 = F32Point::CreateFromIndex(
				m_someIOA_Arr[ i ], srcSiz );

			//DrawCircle( dspImg, F32Point( pnt1.x, pnt1.y ), 
			//	u8ColorVal( 255, 255, 255 ), 4 );
		}


		ShowImage( dspImg, "Edges_Roots" );
	}




	void ScanTryMgr_6::DspLineOnImg( int a_x, int a_y,
		Ns_ImgGradMgr::ScanLineItrRef a_sli )
	{
		float * angDifBuf = m_angDifData->GetData();

		
		CvSize srcSiz = m_src->GetSize();

		a_sli->GotoFirst();

//--		F32ImageRef dspImg = m_src->Clone();

		//F32ImageRef dspImg = F32Image::Create(srcSiz, 3);
		F32ImageRef dspImg = m_src->GetSrcImage()->Clone();

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


	//void ScanTryMgr_6::ValidateDiameter( int a_nDiameter )
	void ScanTryMgr_6::ValidateDiameter( )
	{
		int a_nDiameter = m_nDiameter;

		Hcpl_ASSERT( 1 == a_nDiameter % 2 );

		int nCoreOdd = GetDiamCore();
		Hcpl_ASSERT( nCoreOdd % 2 > 0 );

		int nInr_1 = GetDiamInr_1();
		Hcpl_ASSERT( nInr_1 % 2 > 0 );

		int nInr_2 = GetDiamInr_2();
		Hcpl_ASSERT( nInr_2 % 2 > 0 );
	}


	ScanTryMgr_6::ScanTryMgr_6( int a_x, int a_y )
	{
		const int nLastLA_Index = GlobalStuff::m_nLA_Index;

		//a_x = 781;
		//a_y = 277;
		
		//a_x = 770;
		//a_y = 188;
		
		//a_x = 581;
		//a_y = 345;
		
		//a_x = 423;
		//a_y = 299;
		
		//a_x = 128;
		//a_y = 99;




//--		PrepareEnv( GlobalStuff::GetLinePathImg() );


		//const int nRadius = GlobalStuff::AprSize1D / 2;
		//const int nRadius = 1;
		//const int nRadius = 3;
		//const int nRadius = 10;

		//m_nDiameter = nRadius * 2 + 1;

		m_nDiameter = GlobalStuff::AprSize1D;

		//ValidateDiameter( m_nDiameter );
		ValidateDiameter();

		//IImgDataMgrRef dm1 = new ImgDataMgr_Simple(
		IImgDataMgrRef dm1 = new ImgDataMgr_Simple_2(			
			//GlobalStuff::GetLinePathImg(), m_nDiameter);
			GlobalStuff::GetLinePathImg(), GetDiamInr_1());



		PrepareEnv( dm1, dm1->GetDiameter() );

		CvSize srcSiz = m_src->GetSize();

		ScanTryDbg::ScanTryDbgCore * pScanDbgCore = & ScanTryDbg::Core;


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
		//PrepareFiltSys_2();

		//PrepareFiltSys_Thesis();

		PrepareFiltSys_Thesis_2();


		Find_Edges_Roots( a_x, a_y );

		//PostProcessEI_Arr();
		PostProcessEI_Arr_2();

		Show_Edges_Roots();

		ShowImage( GenMaxSlopeImg(), "MaxSlopeImg" );
		//ShowImage( GenMinSlopeImg(), "MinSlopeImg" );


		//if( -1 == nDir &&
		if( -1 == GlobalStuff::ScanDir &&
			NULL != pScanDbgCore->SliMark )
		{
			F32Point pnt1 = F32Point::CreateFromIndex( 
				pScanDbgCore->nIOA, srcSiz );
			
			//ScanSli( pnt1.x, pnt1.y, 
			ScanPntDir( pnt1.x, pnt1.y, 				
				pScanDbgCore->SliMark );
		}


		//ScanPntDir( a_x, a_y );


		//ShowImage( GenFinDifImg( 6 ), "FinDifImg" );
		//ShowImage( GenSlopeImg( 0 ), "SlopeImg" );
		

		EdgeScan::EdgeInfo rEI = *(GetEI_Of_Pix( a_x, a_y, 
			GlobalStuff::ScanDir ));

		//GlobalStuff::SetLinePathImg(
		//	GenTriChGrayImg( m_src->GetStandDiv_Mag_Image() ));

		//GlobalStuff::ShowLinePathImg();


		//ShowImage(m_src->GetStandDiv_Mag_Image(), "StandDiv_Mag_Image");


		if( false )
		{
			float * maxDif_Buf = & (this->GetMaxDif_Arr())[ 0 ];

			int nScale = 10;

			ImageWatershedMgrRef iwm1 = new ImageWatershedMgr(
				maxDif_Buf, m_src->GetSize(), nScale );

			iwm1->FloodToVal(
				maxDif_Buf[ pScanDbgCore->nIOA_XY ] );

			F32ImageRef iwImg = F32Image::Create( 
				m_src->GetSize(), 1 );

			float * iwImg_Buf = iwImg->GetPixAt( 0, 0 );

			const int nSize1D = 
				m_src->GetSize().width * m_src->GetSize().height;

			FixedVector< ImageWatershed_Elm > & iw_Data_Arr =
				iwm1->GetDataArr();

			for( int i=0; i < nSize1D; i++ )
			{
				ImageWatershed_Elm * pIwe = & iw_Data_Arr[ i ];

				if( pIwe->IsFlooded )
				{
					iwImg_Buf[ i ] = 255;
				}
				else
				{
					iwImg_Buf[ i ] = 0;
				}
			}


			ShowImage( iwImg, "Image Watershed Image" );
		}


	}




	ScanTryMgr_6::ScanTryMgr_6( IImgDataMgrRef a_src, int a_nDiameter )
	{
		//m_nDiameter = GlobalStuff::AprSize1D;
		m_nDiameter = a_src->GetDiameter();

		m_nDiameter = a_nDiameter;

		ValidateDiameter();

		PrepareEnv( a_src, m_nDiameter );

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
		//PrepareFiltSys_2();

		//PrepareFiltSys_Thesis();

		PrepareFiltSys_Thesis_2();

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


	void ScanTryMgr_6::ScanSli( int a_x, int a_y,
			Ns_ImgGradMgr::ScanLineItrRef a_sli )
	{
		//PrepareAperSizes();

		FillLineData( a_sli );

		m_filtSys->Proceed();


		ShowSignals();

		DspLineOnImg( a_x, a_y, a_sli );
	}


	EdgeScan::EdgeInfo * ScanTryMgr_6::GetEI_Of_Pix( int a_x, int a_y,
		int a_nDir )
	{
		CvSize srcSiz = m_src->GetSize();

		IndexCalc2D idx2D( srcSiz.width, srcSiz.height );

		int nPI = idx2D.Calc( a_x, a_y );

		return GetEI_Of_Pix( nPI, a_nDir );
	}

	EdgeScan::EdgeInfo * ScanTryMgr_6::GetEI_Of_Pix( int a_nPI, int a_nDir )
	{
		//EdgeScan::EdgeInfo * pEI_Ret;


		ScanDirMgr & rScanMgr = m_sdmColl->GetMgrAt( a_nDir );

		FixedVector< ScanDirMgr::PixLineInfo > & pliArr =
			rScanMgr.GetPixLineInfoArr();

		ScanDirMgr::PixLineInfo & rPli = pliArr[ a_nPI ];



		EdgeScan::ScanDirMgrExtRef smExtNorm = m_scanDirExtArr[ a_nDir ];


		MinIndexFinder< int > minFnd;

		for( int i = smExtNorm->nEI_Bgn; i < smExtNorm->nEI_Lim; i++)		
		{
			EdgeScan::EdgeInfo & rEI_1 = m_edgeInfoArr[ i ];

			if(rPli.nLineNum != rEI_1.LineNum )
				continue;


			int nDif = abs( rPli.nIOL - rEI_1.EdgeIOL );


			minFnd.AddValue( nDif, i );
		}

		


		return & m_edgeInfoArr[ minFnd.FindMinIdx() ];


		//return pEI_Ret;
	}


	void ScanTryMgr_6::ScanPntDir( int a_x, int a_y,
			Ns_ImgGradMgr::ScanLineItrRef a_sli )
	{
		ImgGradMgr_2 & igm1 = *m_imgGradMgr;
		//ImgAvgingMgr & igm1 = *m_imgGradMgr;




		CvSize srcSiz = m_src->GetSize();

		IndexCalc2D idx2D( srcSiz.width, srcSiz.height );

		const int nPI = idx2D.Calc( a_x, a_y );

		//int nDir = GlobalStuff::AprSize1D;
		int nDir = GlobalStuff::ScanDir;

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


		//Ns_ImgGradMgr::ScanLineItrRef sli = igm1.GenScanLineItr( nPI, nDir );
		//Ns_ImgGradMgr::ScanLineItrRef sli = igm1.GenScanLineItr( nBPI, nDir );

		Ns_ImgGradMgr::ScanLineItrRef sliXY;

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
			//AddSigDraw( m_sigPosDelta, false, u8ColorVal( 0, 0, 255 ) );
		}


		ScanSli( a_x, a_y, sliXY );


	}




	void ScanTryMgr_6::Find_Edges_Roots( int a_x, int a_y )
	{
		ImgGradMgr_2 & igm1 = *m_imgGradMgr;
		//ImgAvgingMgr & igm1 = *m_imgGradMgr;
		

		//Ns_ImgGradMgr::ScanLineItrRef sli_0;

		Ns_ImgGradMgr::ScanLineItrRef sli;


		bool bSliXY_Exists = false;
		//Ns_ImgGradMgr::ScanLineItrRef sliXY = NULL;

		//const int nDir = GlobalStuff::AprSize1D;
		const int nDir = GlobalStuff::ScanDir;

		int nLineNum = -1;
		//int nLineNum = 290;
		

		if(-1 == nDir)
		{
			a_x = -1;
			a_y = -1;
		}


		if( -1 != a_x && -1 != a_y )
		//if( false )
		{
			CvSize srcSiz = m_src->GetSize();

			IndexCalc2D idx2D( srcSiz.width, srcSiz.height );

			const int nPI = idx2D.Calc( a_x, a_y );



			ScanDirMgr & rScanMgr = m_sdmColl->GetMgrAt( nDir );

			FixedVector< ScanDirMgr::PixLineInfo > & pliArr =
				rScanMgr.GetPixLineInfoArr();


			ScanDirMgr::PixLineInfo & rPli = pliArr[ nPI ];

			nLineNum = rPli.nLineNum;

			Ns_ImgGradMgr::ScanLineItrRef sliXY;


			//sliXY = igm1.GenScanLineItr_FromLine( rPli.nLineNum, nDir );

			//sli_0 = igm1.GenScanLineItr_FromLine( rPli.nLineNum, nDir );

			//sliXY = sli_0;



			bSliXY_Exists = true;
		}


		//Ns_ImgGradMgr::ScanLineItrRef sliXY = 
			//igm1.GenScanLineItr_FromLine( nDir, nLineNum );



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


				//if( NULL != sliXY )			
				if( bSliXY_Exists )			
				{
					//if( sli->GetDirIndex() == sliXY->GetDirIndex() &&
						//sli->GetLineNum() == sliXY->GetLineNum() )

					if( -1 != nDir && sli->GetDirIndex() == nDir &&
						sli->GetLineNum() == nLineNum )
					{					
						//ScanSli( a_x, a_y, sliXY );
						//ScanSli( a_x, a_y, sli );






						CvSize srcSiz = m_src->GetSize();

						IndexCalc2D idx2D( srcSiz.width, srcSiz.height );

						const int nPI = idx2D.Calc( a_x, a_y );


						//F32ImageRef difMagImg = igm1.GetGradMagImg();
						

						ScanDirMgr & rScanMgr = m_sdmColl->GetMgrAt( nDir );

						FixedVector< ScanDirMgr::PixLineInfo > & pliArr =
							rScanMgr.GetPixLineInfoArr();

						ScanDirMgr::PixLineInfo & rPli = pliArr[ nPI ];



						const int nSI = rScanMgr.GetShiftStartArr()[ nLineNum ];

						m_sigPosDelta = DeltaSignal::CreateAt( rPli.nIOL - nSI );

						AddSigDraw( m_sigPosDelta, true, u8ColorVal( 0, 0, 255 ) );


						ShowSignals();

						DspLineOnImg( a_x, a_y, sli );
					}
				}


			}

			smExt->nEI_Lim = m_edgeInfoArr.GetSize();
		}
	}


	void ScanTryMgr_6::PostProcScanLine( Ns_ImgGradMgr::ScanLineItrRef a_sli,
			int a_nofEIs_BefLine )
	{
		const int nLineSiz = m_ioa_ptrDataMgr->GetSize();

		const int nDir = a_sli->GetDirIndex();

		EdgeScan::ScanDirMgrExtRef smExt = m_scanDirExtArr[ nDir ];

		float * finDifGlob_Buf = & smExt->m_difGlobArr[ 0 ];

		EdgeScan::EdgeInfo * * EI_GlobArr_Buf = & smExt->m_edgeInfo_GlobArr[ 0 ];

		
		 


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

		//	if no edges on this line
		if( m_edgeInfoArr.GetSize() == a_nofEIs_BefLine )
		{
			for( int i=0; i < nLineSiz; i++ )
			{
				const int nIOA = ioa_Buf[ i ];

				//slopeGlob_Buf[ nIOA ] = 0;
				slopeGlob_Buf[ nIOA ] = 100;

				EI_GlobArr_Buf[ nIOA ] = NULL;
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

				EI_GlobArr_Buf[ nIOA ] = NULL;
			}
		}

		{
			EdgeScan::EdgeInfo & rEI_End = m_edgeInfoArr.GetBack();

			int nEndIOL = rEI_End.Root2_IOL;

			for( int i = nEndIOL + 1; i < nLineSiz; i++ )
			{
				const int nIOA = ioa_Buf[ i ];

				//slopeGlob_Buf[ nIOA ] = 0;
				slopeGlob_Buf[ nIOA ] = 100;

				EI_GlobArr_Buf[ nIOA ] = NULL;
			}
		}

		//uuu



		IImgPopulationRef imgPop_R1 = m_src->CreatePopulation();
		FixedVector< int > popIdx_Arr_R1(10000);
		
		IImgPopulationRef imgPop_R2 = m_src->CreatePopulation();
		FixedVector< int > popIdx_Arr_R2(10000);
		
		IImgPopulationRef imgPop_E = m_src->CreatePopulation();
		FixedVector< int > popIdx_Arr_E(10000);


				
		//float * mag_Buf = m_mag_Data->GetData();
		float * standDivSig_Buf = m_standDivData->GetData();
		float * standDivSig_R_Buf = m_standDiv_R_Data->GetData();
		float * standDivSig_R12_Buf = m_standDiv_R12_Data->GetData();
		
		

		{
			int nSize = m_standDivData->GetSize();

			for( int i=0; i < nSize; i++ )
			{
				//mag_Buf[ i ] = 0;
				standDivSig_Buf[ i ] = 0;
				standDivSig_R_Buf[ i ] = 0;				
				standDivSig_R12_Buf[ i ] = 0;				
			}
		}

//		m_standDiv_Buf = m_src->GetStandDiv_Mag_Image(
//			)->GetPixAt(0, 0);



		for( int i = a_nofEIs_BefLine; i < m_edgeInfoArr.GetSize(); i++ )
		{
			EdgeScan::EdgeInfo & rEI = m_edgeInfoArr[ i ];


			if( 4 == nDir && 
				a_sli->GetLineNum() == 128 )
				i = i;


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

				MinIndexFinder< float > minFnd;

				for( int i = rEI.EdgeIOL; i >= rEI.Root1_IOL; i-- )
				{
					const float valNetI = difBuf[ i ] - difValBef;

					float valSlope = valNetI / difNetBef;

					valSlope *= 100;

					minFnd.AddValue(valSlope, i);

					int nIOA_I = ioa_Buf[ i ];

					slopeGlob_Buf[ nIOA_I ] = valSlope;

					EI_GlobArr_Buf[ nIOA_I ] = & rEI;

					{
						EdgeScan::PixInfo & rPI = m_pixInfoArr[ nIOA_I ];

						if( valSlope > rPI.maxSlopeVal )
							rPI.maxSlopeVal = valSlope;

						if( valSlope < rPI.minSlopeVal )
							rPI.minSlopeVal = valSlope;
					}

					if( m_isMaxDirArr[ i ] )
						m_slopeOfMaxDifArr[ nIOA_I ] = valSlope;

					if( bNotFound_Med )
					{
						//if( valSlope < 35 )
						if( valSlope < 50 )
						{
							rEI.Med1_IOL = i;

							//slopeGlob_Buf[ nIOA_I ] = 255;

							bNotFound_Med = false;
						}
					}
					else if( bNotFound_Inr )
					{
						if( valSlope > minFnd.FindMin() + 15 ||
							valSlope > minFnd.FindMin() * 1.3 ||							
							( abs(rEI.EdgeIOL - i) / abs(rEI.EdgeIOL - rEI.Med1_IOL) ) >= 2 ||
							valSlope < 15 
							)
						{
							rEI.InrR1_IOL = minFnd.FindMinIdx();

							bNotFound_Inr = false;

							//break;
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

				MinIndexFinder< float > minFnd;

				for( int i = rEI.EdgeIOL + 1; i <= rEI.Root2_IOL; i++ )
				{
					const float valNetI = difBuf[ i ] - difValAft;

					float valSlope = valNetI / difNetAft;

					valSlope *= 100;

					minFnd.AddValue(valSlope, i);

					int nIOA_I = ioa_Buf[ i ];

					slopeGlob_Buf[ nIOA_I ] = valSlope;

					EI_GlobArr_Buf[ nIOA_I ] = & rEI;

					{
						EdgeScan::PixInfo & rPI = m_pixInfoArr[ nIOA_I ];

						if( valSlope > rPI.maxSlopeVal )
							rPI.maxSlopeVal = valSlope;

						if( valSlope < rPI.minSlopeVal )
							rPI.minSlopeVal = valSlope;
					}

					if( m_isMaxDirArr[ i ] )
						m_slopeOfMaxDifArr[ nIOA_I ] = valSlope;

					if( bNotFound_Med )
					{
						//if( valSlope < 35 )
						if( valSlope < 50 )
						{
							rEI.Med2_IOL = i;

							//slopeGlob_Buf[ nIOA_I ] = 255;

							bNotFound_Med = false;
						}
					}
					else if( bNotFound_Inr )
					{
						//if( valSlope > 60 )
						if( valSlope > minFnd.FindMin() + 15 ||
							valSlope > minFnd.FindMin() * 1.3 ||							
							( abs(rEI.EdgeIOL - i) / abs(rEI.EdgeIOL - rEI.Med2_IOL) ) >= 2 ||
							valSlope < 15 
							)
						{
							rEI.InrR2_IOL = minFnd.FindMinIdx();

							bNotFound_Inr = false;

							//break;
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



///////////----------------------------------------------


			if( 6395 == rEI.Index )
				i = i;


			imgPop_R1->Reset();
			popIdx_Arr_R1.ResetSize();
		
			imgPop_R2->Reset();
			popIdx_Arr_R2.ResetSize();
		
			imgPop_E->Reset();
			popIdx_Arr_E.ResetSize();


			for( int j = rEI.Med1_IOL; j >= rEI.InrR1_IOL; j-- )
			//for( int j = rEI.EdgeIOL; j >= rEI.InrR1_IOL; j-- )
			{
				int nIOA_J = ioa_Buf[ j ];
				popIdx_Arr_R1.PushBack( nIOA_J );
			}
			//imgPop_R1->AddRange_Thick( 
			imgPop_R1->AddRange( 
				& popIdx_Arr_R1[ 0 ], popIdx_Arr_R1.GetSize() );

			rEI.StandDiv_R1 = imgPop_R1->CalcStandDiv();

			for( int j = rEI.Med1_IOL; j >= rEI.InrR1_IOL; j-- )
			{
				int nIOA_J = ioa_Buf[ j ];
				standDivSig_R_Buf[ j ] = rEI.StandDiv_R1;
			}



			for( int j = rEI.Med2_IOL; j <= rEI.InrR2_IOL; j++ )
			//for( int j = rEI.EdgeIOL; j <= rEI.InrR2_IOL; j++ )
			{
				int nIOA_J = ioa_Buf[ j ];
				popIdx_Arr_R2.PushBack( nIOA_J );
			}
			//imgPop_R2->AddRange_Thick( 
			imgPop_R2->AddRange( 
				& popIdx_Arr_R2[ 0 ], popIdx_Arr_R2.GetSize() );

			rEI.StandDiv_R2 = imgPop_R2->CalcStandDiv();

			for( int j = rEI.Med2_IOL; j <= rEI.InrR2_IOL; j++ )
			{
				int nIOA_J = ioa_Buf[ j ];
				standDivSig_R_Buf[ j ] = rEI.StandDiv_R2;
			}



			for( int j = rEI.Med1_IOL; j <= rEI.Med2_IOL; j++ )
			{
				int nIOA_J = ioa_Buf[ j ];
				popIdx_Arr_E.PushBack( nIOA_J );
			}
			//imgPop_E->AddRange_Thick( 
			imgPop_E->AddRange( 
				& popIdx_Arr_E[ 0 ], popIdx_Arr_E.GetSize() );

			{
				//ImgPopulation_Simple * pop1 = 
					//dynamic_cast< ImgPopulation_Simple * > ((IImgPopulation *)imgPop_R2);
				
				//IImgPopulation * pop1 = (IImgPopulation *)imgPop_R2;

				//imgPop_R1->IncBy( pop1 );
				imgPop_R1->IncBy( (IImgPopulation *)imgPop_R2 );
			}

			rEI.StandDiv_R1_R2 = imgPop_R1->CalcStandDiv();			
			rEI.StandDiv_E = imgPop_E->CalcStandDiv();
			//rEI.StandDiv_E = rEI.StandDiv_R1_R2;
						
			//for( int j = rEI.Root1_IOL; j <= rEI.Root2_IOL; j++ )
			for( int j = rEI.Med1_IOL; j <= rEI.Med2_IOL; j++ )
			{
				int nIOA_J = ioa_Buf[ j ];
				standDivSig_Buf[ j ] = rEI.StandDiv_E;
				standDivSig_R12_Buf[ j ] = rEI.StandDiv_R1_R2;
			}


			{
				int nIOA_R = ioa_Buf[ rEI.InrR1_IOL ];
				int nIOA_Med = ioa_Buf[ rEI.Med1_IOL ];

				int nIOA_R_Peer = ioa_Buf[ rEI.InrR2_IOL ];

				//for( int j = rEI.Root1_IOL; j <= rEI.EdgeIOL; j++ )
				for( int j = rEI.InrR1_IOL; j <= rEI.EdgeIOL; j++ )
				{
					int nIOA_J = ioa_Buf[ j ];

					if( 130578 == nIOA_J )
					{
						m_someIOA_Arr.PushBack( nIOA_R );
					}

					EdgeScan::PixInfo & rPI = m_pixInfoArr[ nIOA_J ];

					EdgeScan::EdgeInfo ** ppEI_J = & m_maxEI_Arr[ nIOA_J ];

					if( NULL == *ppEI_J )
					{
						*ppEI_J = & rEI;
						rPI.nIOA_OfMax_EI_R = nIOA_R;
						rPI.nIOA_OfMax_EI_Med = nIOA_Med;
					}
					else
					{
						//if( rEI.StandDiv_E > (*ppEI_J)->StandDiv_E )
						//if( rEI.StandDiv_R1 < (*ppEI_J)->StandDiv_R1 )
						if( m_standDiv_Buf[ nIOA_R ] < m_standDiv_Buf[ rPI.nIOA_OfMax_EI_R ] )
						{
							float dif_R = m_src->CalcDif( nIOA_R, rPI.nIOA_OfMax_EI_R );

							float dif_R_Peer = m_src->CalcDif( nIOA_R_Peer, rPI.nIOA_OfMax_EI_R );

							if( dif_R < dif_R_Peer )
							{
								*ppEI_J = & rEI;
								rPI.nIOA_OfMax_EI_R = nIOA_R;
								rPI.nIOA_OfMax_EI_Med = nIOA_Med;
							}

						}
					}

				}
			}


			{
				int nIOA_R = ioa_Buf[ rEI.InrR2_IOL ];
				int nIOA_Med = ioa_Buf[ rEI.Med2_IOL ];

				int nIOA_R_Peer = ioa_Buf[ rEI.InrR1_IOL ];

				//for( int j = rEI.Root2_IOL; j >= rEI.EdgeIOL; j-- )
				for( int j = rEI.InrR2_IOL; j >= rEI.EdgeIOL; j-- )
				{
					int nIOA_J = ioa_Buf[ j ];

					if( 130578 == nIOA_J )
					{
						m_someIOA_Arr.PushBack( nIOA_R );
					}

					EdgeScan::PixInfo & rPI = m_pixInfoArr[ nIOA_J ];

					EdgeScan::EdgeInfo ** ppEI_J = & m_maxEI_Arr[ nIOA_J ];

					if( NULL == *ppEI_J )
					{
						*ppEI_J = & rEI;
						rPI.nIOA_OfMax_EI_R = nIOA_R;
						rPI.nIOA_OfMax_EI_Med = nIOA_Med;
					}
					else
					{
						//if( rEI.StandDiv_E > (*ppEI_J)->StandDiv_E )
						//if( rEI.StandDiv_R2 < (*ppEI_J)->StandDiv_R2 )
						if( m_standDiv_Buf[ nIOA_R ] < m_standDiv_Buf[ rPI.nIOA_OfMax_EI_R ] )
						{
							float dif_R = m_src->CalcDif( nIOA_R, rPI.nIOA_OfMax_EI_R );

							float dif_R_Peer = m_src->CalcDif( nIOA_R_Peer, rPI.nIOA_OfMax_EI_R );

							if( dif_R < dif_R_Peer )
							{
								*ppEI_J = & rEI;
								rPI.nIOA_OfMax_EI_R = nIOA_R;
								rPI.nIOA_OfMax_EI_Med = nIOA_Med;
							}

						}
					}

				}
			}






///////////----------------------------------------------


		}


		{
			float * confSig_Buf = m_confData->GetData();
			int nBufSiz = m_confData->GetSize();


			//float * scanBuf = m_scanData->GetData();

//--			F32ColorVal * colorBuf = (F32ColorVal *) m_src->GetPixAt( 0, 0 );

			


			for( int i=0; i < nBufSiz; i++ )
			{
				confSig_Buf[ i ] = 0;
//				confSig_Buf[ i ] = i / ( 5 + nBufSiz % 7 );
				//confSig_Buf[ i ] = scanBuf[ i ];
			
			}
			


			const int nEI_Bgn = a_nofEIs_BefLine;
			const int nEI_Lim = m_edgeInfoArr.GetSize();

			for( int k = nEI_Bgn; k < nEI_Lim; k++ )
			{
				EdgeScan::EdgeInfo & rEI = m_edgeInfoArr[ k ];




				if( rEI.IsCanceled )
					continue;


				float * difBuf = & smExt->m_difGlobArr[ 0 ];

				float * difSig_Buf = rEI.dif_HeadPtr;

				int * ioaBuf = rEI.ioa_HeadPtr;

				const float difVal_Edge = difBuf[ ioaBuf[ rEI.EdgeIOL ] ];

				//{
					int nIOA_E = ioaBuf[ rEI.EdgeIOL ];

//--					F32ColorVal * pColor_E = & colorBuf[ nIOA_E ];


					int nIOA_R1 = ioaBuf[ rEI.InrR1_IOL ];

					int nIOA_Med1 = ioaBuf[ rEI.Med1_IOL ];

					//const float difVal_R1 = difBuf[ nIOA_R1 ];

//--					F32ColorVal * pColor_R1 = & colorBuf[ nIOA_R1 ];


					//int nIOA_1 = ioaBuf[ rEI.InrR1_IOL ];
					int nIOA_1 = ioaBuf[ rEI.Med1_IOL ];

					const float difVal_1 = difBuf[ nIOA_1 ];

//--					F32ColorVal * pColor1 = & colorBuf[ nIOA_1 ];


//--					float colorDif_1_E = F32ColorVal::Sub( *pColor1, *pColor_E ).CalcMag();
					float colorDif_1_E = m_src->CalcDif( nIOA_1, nIOA_E);





					int nIOA_R2 = ioaBuf[ rEI.InrR2_IOL ];

					int nIOA_Med2 = ioaBuf[ rEI.Med2_IOL ];

					//const float difVal_R2 = difBuf[ nIOA_R2 ];

//--					F32ColorVal * pColor_R2 = & colorBuf[ nIOA_R2 ];



					//int nIOA_2 = ioaBuf[ rEI.InrR2_IOL ];
					int nIOA_2 = ioaBuf[ rEI.Med2_IOL ];

					const float difVal_2 = difBuf[ nIOA_2 ];

//--					F32ColorVal * pColor2 = & colorBuf[ nIOA_2 ];

//--					float colorDif_2_E = F32ColorVal::Sub( *pColor2, *pColor_E ).CalcMag();
					float colorDif_2_E = m_src->CalcDif( nIOA_2, nIOA_E );

					
//--					float colorDif = F32ColorVal::Sub( *pColor1, *pColor2 ).CalcMag();
					float colorDif = m_src->CalcDif( nIOA_1, nIOA_2 );

//--					float colorDif_R12 = F32ColorVal::Sub( *pColor_R1, *pColor_R2 ).CalcMag();
					float colorDif_R12 = m_src->CalcDif( nIOA_R1, nIOA_R2 );
					


					{
//--						float unitDif = CalcColorUnitDif( *pColor1, *pColor2 );

//--						colorDif *= ( 1 + unitDif );
						//colorDif *= ( 0.8 + unitDif );
						//colorDif *= ( 0.6 + unitDif );

						//if( difMag > 440 )
							//difMag = 440;
					}



					//float horzDif_Tot = rEI.InrR2_IOL - rEI.InrR1_IOL;
					float horzDif_Tot = rEI.Med2_IOL - rEI.Med1_IOL;

					//float colorDif_2 = colorDif * 10 / horzDif_Tot;
					//float colorDif_2 = colorDif * 5 / horzDif_Tot;

					float colorDif_2 = colorDif_R12 * 5 / horzDif_Tot;

					rEI.Scaled_EdgeVal = colorDif_2;

					//if( 54152 == rEI.Index )
					//if( 9935 == rEI.Index )
					//if( 4935 == rEI.Index )
					//if( 28339 == rEI.Index )
					//if( 4663 == rEI.Index )
					//if( 9711 == rEI.Index )
					//if( 1687 == rEI.Index )
					//if( 1686 == rEI.Index )				
					
					if( 15399 == rEI.Index )				
					//if( 56375 == rEI.Index )				
						k = k;


					//if( 400 == rEI.Index )
					if( 2988 == rEI.Index )
						k = k;

					//bool bSrcVerified = m_src->VerifyEdge(
					//	nIOA_R1, nIOA_Med1, nIOA_E, nIOA_Med2, nIOA_R2 );

					//if( ! bSrcVerified )
					//{
					//	rEI.Set_DoesConflict( false );
					//	//goto DoesConflict_Done;
					//}

					{
						// Hthm Tmp
						//goto DoesConflict_Done;
					}


					//if( colorDif_2 / difVal_Edge < 0.8 ||
					//if( colorDif / difVal_Edge < 0.8 ||
					//	colorDif_1_E / colorDif > 0.7 ||
					//	colorDif_2_E / colorDif > 0.7 )
					//{
					//	rEI.Set_DoesConflict( false );
					//	//goto DoesConflict_Done;
					//}
					//else
					//	k = k;


//////////////////////////////////////////////////////////////////


					{
						//m_standDiv_Buf = m_src->GetStandDiv_Mag_Image(
							//)->GetPixAt(0, 0);

						float standDev_E = m_standDiv_Buf[nIOA_E];
						float standDev_1 = m_standDiv_Buf[nIOA_R1];
						float standDev_2 = m_standDiv_Buf[nIOA_R2];


						//if( // colorDif < standDev_E * 2.6 ||
						//	colorDif_R12 < colorDif * 0.8 )
						//{
						//	rEI.Set_DoesConflict( false );
						//	//goto DoesConflict_Done;
						//}


						{
							//float standDev_E_06 = standDev_E * 0.66666;
							//float standDev_E_06 = standDev_E * 0.4;
							//float standDev_E_06 = standDev_E * 0.2;
							float standDev_E_06 = standDev_E * 0.6;

							if( standDev_2 > standDev_E_06 ||
								standDev_1 > standDev_E_06 )
							{
								if( 36098 == rEI.Index )
									k = k;

								//	Hthm Tmp
								//rEI.IsCanceled = true;

								//rEI.Set_DoesConflict( false );

								k = k;

								//goto DoesConflict_Done;
							}
						}
					}









					// Hthm Tmp
					//goto DoesConflict_Done;


					if( 11340 == rEI.Index )
						k = k;


					if( difVal_Edge < 20 && rEI.StandDiv_E < 20 )
					{
						//	Hthm Tmp
						rEI.IsCanceled = true;

						rEI.Set_DoesConflict( false );
						//goto DoesConflict_Done;
					}


					if( //( difVal_Edge < 20 && rEI.StandDiv_E < 20 ) || 
						rEI.StandDiv_E < 12
						|| rEI.StandDiv_R1_R2 < 20
						) 
					{
						//	Hthm Tmp
						//rEI.IsCanceled = true;

						//rEI.Set_DoesConflict( false );
						//goto DoesConflict_Done;
					}

					if( rEI.StandDiv_R1 / rEI.StandDiv_E > 0.5
						|| rEI.StandDiv_R2 / rEI.StandDiv_E > 0.5
						)
					{
						//	Hthm Tmp
						//rEI.IsCanceled = true;

						//rEI.Set_DoesConflict( false );
						//goto DoesConflict_Done;
					}

				//}




DoesConflict_Done:

				if( rEI.DoesConflict() )
				{
					confSig_Buf[ rEI.EdgeIOL ] = 70;
				}
				else
				{
					//confSig_Buf[ rEI.EdgeIOL ] = - 50;
					confSig_Buf[ rEI.EdgeIOL ] = 30;

					//	Hthm Tmp
					//rEI.IsCanceled = true;
				}



			}




		}

	}



	EdgeScanMgr::DataDraw * ScanTryMgr_6::AddSigDraw( ISingFilt1D2Ref a_filt, 
		bool a_bDraw, U8ColorVal a_color, int a_shiftX, int a_shiftY, float a_scale )
	{
		m_filtSys->GetFiltArr().PushBack( (IFilt1D2Ref) a_filt );

		return AddSigDraw( a_filt->GetOutputReader(), 
			a_bDraw, a_color, a_shiftX, a_shiftY, a_scale );
	}


	void ScanTryMgr_6::ShowSignals()
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





	void ScanTryMgr_6::PrepareFiltSys()
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



	void ScanTryMgr_6::PrepareFiltSys_Thesis()
	{
		int nShiftY_1 = 100;
		int nShiftY_2 = 180;
		

		m_filtSys = new Filt1D2SysEmpty( m_nMaxScanSize );
		

		{
			AddSigDraw( ToRdr( m_angDifData ), false, u8ColorVal( 255, 0, 255 ), 0, nShiftY_1 );
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
				//true, GetLastDraw().Color, 0, GetLastDraw().ShiftY );
				//true, GetLastDraw().Color, 0, nShiftY_1 );
				false, GetLastDraw().Color, 0, nShiftY_1 );
		}
		ISingFilt1D2DataReaderRef locRdr = GetLastDraw().DataRdr;


		{
			AddSigDraw( ToRdr( m_scanData_2 ), true, 
				u8ColorVal( 0, 255, 170 ), 0, nShiftY_1 );
		}

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
				AddSigDraw( new DifFilt1D2( m_nDiameter, GetLastDraw().DataRdr ), 
					//true, GetLastDraw().Color, 0, nShiftY_1, 3 );
					//true, u8ColorVal( 0, 100, 155 ), 0, nShiftY_1, 1 );				
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

				AddSigDraw( filt1, 
					//true, pDif3Draw->Color, 0, nShiftY_2 + 10 );
					false, pDif3Draw->Color, 0, nShiftY_1 );
					//false, pDif3Draw->Color, 0, 0 );

				pk_out_2 = filt1->GetOutput_2_Reader();

				AddSigDraw( pk_out_2, 
						true, u8ColorVal( 255, 255, 255 ), 0, 0 );
					//false, pDif3Draw->Color, 0, 0 );

				//AddSigDraw( filt1->GetOutput_3_Reader(), 
						//true, u8ColorVal( 0, 0, 255 ), 0, nShiftY_1 );

				//AddSigDraw( filt1->GetOutput_4_Reader(), 
						//true, u8ColorVal( 255, 0, 0 ), 0, nShiftY_1 );

				//AddSigDraw( filt1->GetOutput_5_Reader(), 
						//true, u8ColorVal( 0, 0, 255 ), 0, nShiftY_1 );
			}

		}


///////////////////////

		//AddSigDraw( DeltaSignal::CreateAt( 12 ), 
			//true, u8ColorVal( 0, 0, 255 ) );


		{
			AddSigDraw( ToRdr( m_confData ), true, 
			//AddSigDraw( ToRdr( m_scanData ), true, 
				u8ColorVal( 0, 0, 255 ), 0, nShiftY_1 );
		}


		

		{
			AddSigDraw( ToRdr( m_mag_Data ), true, 
				u8ColorVal( 255, 255, 255 ), 0, nShiftY_1 );
		}

		{
			AddSigDraw( ToRdr( m_standDivData ), true, 
				u8ColorVal( 0, 255, 255 ), 0, 0 );
		}

		{
			AddSigDraw( ToRdr( m_standDiv_R_Data ), true, 
				u8ColorVal( 180, 100, 255 ), 0, 0 );
		}

		{
			AddSigDraw( ToRdr( m_standDiv_R12_Data ), true, 
				u8ColorVal( 180, 255, 100 ), 0, 0 );
		}

		

		
	}


	void ScanTryMgr_6::PrepareFiltSys_Thesis_2()
	{
		int nShiftY_1 = 100;
		int nShiftY_2 = 180;
		

		m_filtSys = new Filt1D2SysEmpty( m_nMaxScanSize );
		

		{
			AddSigDraw( ToRdr( m_angDifData ), false, u8ColorVal( 255, 0, 255 ), 0, nShiftY_1 );
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
			AddSigDraw( ToRdr( m_locData ), true, m_colorLoc.ToU8() );
			//AddSigDraw( ToRdr( m_locData ), false, m_colorLoc.ToU8() );

			AddSigDraw( new AvgFilt1D2( 5, GetLastDraw().DataRdr ),
				//true, GetLastDraw().Color, 0, GetLastDraw().ShiftY );
				//true, GetLastDraw().Color, 0, nShiftY_1 );
				false, GetLastDraw().Color, 0, nShiftY_1 );
		}
		ISingFilt1D2DataReaderRef locRdr = GetLastDraw().DataRdr;

		{
			//AddSigDraw( ToRdr( m_scanData_2 ), true, 
			AddSigDraw( ToRdr( m_scanData_2 ), false, 
				u8ColorVal( 0, 255, 170 ), 0, nShiftY_1 );
		}

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
				AddSigDraw( new DifFilt1D2( m_nDiameter, GetLastDraw().DataRdr ), 
					//true, GetLastDraw().Color, 0, nShiftY_1, 3 );
					//true, u8ColorVal( 0, 100, 155 ), 0, nShiftY_1, 1 );				
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
				PeakMarkerFilt1D2_2 * filt1 = new PeakMarkerFilt1D2_2( 
					5, scanRdr, ToRdr( m_standDivData ), locRdr, 
					dif2LocRdr, angDifRdr, GetLastDraw().DataRdr, ZCPosRdr, ToRdr( m_ioaData ), 
					m_edgeInfoArr, m_pixInfoArr );

				AddSigDraw( filt1, 
					//true, pDif3Draw->Color, 0, nShiftY_2 + 10 );
					false, pDif3Draw->Color, 0, nShiftY_1 );
					//false, pDif3Draw->Color, 0, 0 );

				pk_out_2 = filt1->GetOutput_2_Reader();

				AddSigDraw( pk_out_2, 
						true, u8ColorVal( 255, 255, 255 ), 0, 0 );
					//false, pDif3Draw->Color, 0, 0 );

				//AddSigDraw( filt1->GetOutput_3_Reader(), 
						//true, u8ColorVal( 0, 0, 255 ), 0, nShiftY_1 );

				//AddSigDraw( filt1->GetOutput_4_Reader(), 
						//true, u8ColorVal( 255, 0, 0 ), 0, nShiftY_1 );

				//AddSigDraw( filt1->GetOutput_5_Reader(), 
						//true, u8ColorVal( 0, 0, 255 ), 0, nShiftY_1 );
			}

		}


///////////////////////

		//AddSigDraw( DeltaSignal::CreateAt( 12 ), 
			//true, u8ColorVal( 0, 0, 255 ) );


		{
			AddSigDraw( ToRdr( m_confData ), true, 
			//AddSigDraw( ToRdr( m_scanData ), true, 
				u8ColorVal( 0, 0, 255 ), 0, nShiftY_1 );
		}


		

		//{
		//	AddSigDraw( ToRdr( m_mag_Data ), true, 
		//		u8ColorVal( 255, 255, 255 ), 0, nShiftY_1 );
		//}

		//{
		//	AddSigDraw( ToRdr( m_standDivData ), true, 
		//		u8ColorVal( 0, 255, 255 ), 0, 0 );
		//}

		//{
		//	AddSigDraw( ToRdr( m_standDiv_R_Data ), true, 
		//		u8ColorVal( 180, 100, 255 ), 0, 0 );
		//}

		//{
		//	AddSigDraw( ToRdr( m_standDiv_R12_Data ), true, 
		//		u8ColorVal( 180, 255, 100 ), 0, 0 );
		//}


		
		{
			AddSigDraw( ToRdr( m_stDivLocData ), true, 
				u8ColorVal( 0, 255, 225 ), 0, nShiftY_1 );
		}

		
	}




	void ScanTryMgr_6::PrepareFiltSys_2()
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
			AddSigDraw( ToRdr( m_locData ), true, m_colorLoc.ToU8() );
			//AddSigDraw( ToRdr( m_locData ), false, m_colorLoc.ToU8() );

			AddSigDraw( new AvgFilt1D2( 5, GetLastDraw().DataRdr ),
				//true, GetLastDraw().Color, 0, GetLastDraw().ShiftY );
				false, GetLastDraw().Color, 0, GetLastDraw().ShiftY );
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

				//const int nAvgCnt = 1;
				const int nAvgCnt = 0;

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


	void ScanTryMgr_6::PrepareAperSizes()
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


	void ScanTryMgr_6::PrepareDeepRoots()
	{
		//int nDeepCnt = 1;
		int nDeepCnt = 2;
		//int nDeepCnt = 7;


		for( int i=0; i < m_edgeInfoArr.GetSize(); i++ )
		{
			EdgeScan::EdgeInfo & rEI = m_edgeInfoArr[ i ];

			if( rEI.IsCanceled )
				continue;

			{
				int nIOA_R = rEI.ioa_HeadPtr[ rEI.InrR1_IOL ];

				int nIOA_DeepR = nIOA_R;

				int nIOA_DeepMed = rEI.ioa_HeadPtr[ rEI.Med1_IOL ];

				EdgeScan::EdgeInfo * pEI_Deep = & rEI;

				EdgeScan::PixInfo * pPI;
				
				for( int i=0; i < nDeepCnt; i++ )
				{
					pPI = & m_pixInfoArr[ nIOA_DeepR ];

					if( 100928 == pPI->nIndex )
						i = i;

					Hcpl_ASSERT( pPI->nIOA_OfMax_EI_R >= 0 );

					if( pPI->nIOA_OfMax_EI_R < 0 )
						continue;
					
					pEI_Deep = m_maxEI_Arr[ pPI->nIndex ];

					nIOA_DeepR = pPI->nIOA_OfMax_EI_R;

					Hcpl_ASSERT( pPI->nIOA_OfMax_EI_Med >= 0 );

					nIOA_DeepMed = pPI->nIOA_OfMax_EI_Med;
				}

				EdgeScan::PixInfo * pPI_R = & m_pixInfoArr[ nIOA_R ];

				pPI_R->nIOA_VeryDeep_R = nIOA_DeepR;
				pPI_R->nIOA_VeryDeep_Med = nIOA_DeepMed;

				pPI_R->pEI_VeryDeep = pEI_Deep;
			}


			{
				int nIOA_R = rEI.ioa_HeadPtr[ rEI.InrR2_IOL ];

				int nIOA_DeepR = nIOA_R;

				int nIOA_DeepMed = rEI.ioa_HeadPtr[ rEI.Med2_IOL ];

				EdgeScan::EdgeInfo * pEI_Deep = & rEI;

				EdgeScan::PixInfo * pPI;
				
				for( int i=0; i < nDeepCnt; i++ )
				{
					pPI = & m_pixInfoArr[ nIOA_DeepR ];

					if( 100928 == pPI->nIndex )
						i = i;

					Hcpl_ASSERT( pPI->nIOA_OfMax_EI_R >= 0 );

					if( pPI->nIOA_OfMax_EI_R < 0 )
						continue;
					
					pEI_Deep = m_maxEI_Arr[ pPI->nIndex ];

					nIOA_DeepR = pPI->nIOA_OfMax_EI_R;

					Hcpl_ASSERT( pPI->nIOA_OfMax_EI_Med >= 0 );

					nIOA_DeepMed = pPI->nIOA_OfMax_EI_Med;
				}

				EdgeScan::PixInfo * pPI_R = & m_pixInfoArr[ nIOA_R ];

				pPI_R->nIOA_VeryDeep_R = nIOA_DeepR;
				pPI_R->nIOA_VeryDeep_Med = nIOA_DeepMed;

				pPI_R->pEI_VeryDeep = pEI_Deep;
			}

		}




	}


	void ScanTryMgr_6::PostProcessEI_Arr_2()
	{
		PrepareDeepRoots();

		const int nofDirs = m_scanDirExtArr.GetSize();

		//float * markBuf = pScanDbgCore->GetMarkImg()->GetPixAt( 0, 0 );
		

		ScanTryDbg::ScanTryDbgCore * pScanDbgCore = &ScanTryDbg::Core;

		float * markBuf = pScanDbgCore->GetMarkImg()->GetPixAt( 0, 0 );

		//float * markBuf = ScanTryDbg::Core.GetMarkImg()->GetPixAt( 0, 0 );

		MinIndexFinder< float > minFnd;

		float * maxDif_Buf = & m_maxDifArr[ 0 ];

		float * slopeOfMax_Buf = & m_slopeOfMaxDifArr[ 0 ];


//--		F32ColorVal * colorBuf = (F32ColorVal *) m_src->GetPixAt( 0, 0 );

		CvSize srcSiz = m_src->GetSize();

		for( int nDir = 0; nDir < nofDirs; nDir++ )
		{
			EdgeScan::ScanDirMgrExtRef smExt = m_scanDirExtArr[ nDir ];

			float * difBuf = & smExt->m_difGlobArr[ 0 ];

			float * slopeBuf = & smExt->m_slopeGlobArr[ 0 ];


			const int nDirNorm = smExt->GetDirNorm( nofDirs );
			
			EdgeScan::ScanDirMgrExtRef smExtNorm = m_scanDirExtArr[ nDirNorm ];

			float * difBuf_Norm = & smExtNorm->m_difGlobArr[ 0 ];

			float * slopeBuf_Norm = & smExtNorm->m_slopeGlobArr[ 0 ];


			IImgPopulationRef imgPop_D1 = m_src->CreatePopulation();

			IImgPopulationRef imgPop_D2 = m_src->CreatePopulation();



			for( int i = smExt->nEI_Bgn; i < smExt->nEI_Lim; i++ )
			{
				EdgeScan::EdgeInfo & rEI = m_edgeInfoArr[ i ];

				//if( 11741 == rEI.Index )
				if( 1285 == rEI.Index )
					i = i;

				if( rEI.IsCanceled )
					continue;

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
						if( slopeBuf_Norm[ nIOA ] > 30 )
						//if( slopeBuf_Norm[ nIOA ] > 10 )
						//if( slopeOfMax_Buf[ nIOA ] > 10 )							
						{
							//	Hthm Tmp
							//rEI.IsCanceled = true;
							//continue;
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
						if( slopeBuf_Norm[ nIOA ] > 30 )
						//if( slopeBuf_Norm[ nIOA ] > 10 )
						//if( slopeOfMax_Buf[ nIOA ] > 10 )							
						{
							//	Hthm Tmp
							//rEI.IsCanceled = true;
							//continue;
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

					//rEI.InrR1_IOL = minFnd.FindMinIdx();

					{
						int nIOA_I = ioaBuf[ rEI.InrR1_IOL ];

						EdgeScan::PixInfo & rPI = m_pixInfoArr[ nIOA_I ];

						if( rPI.maxSlopeVal >= 60 )
						{
							//rEI.IsCanceled = true;
							//continue;
						}
					}

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

					//rEI.InrR2_IOL = minFnd.FindMinIdx();

					{
						int nIOA_I = ioaBuf[ rEI.InrR2_IOL ];

						EdgeScan::PixInfo & rPI = m_pixInfoArr[ nIOA_I ];

						if( rPI.maxSlopeVal >= 60 )
						{
							//rEI.IsCanceled = true;
							//continue;
						}
					}

				}


				if( difVal_Edge < 20 )
				{
					//rEI.IsCanceled = true;
					//continue;
				}





				{
					float horzDif_Bef = rEI.EdgeIOL - rEI.InrR1_IOL;
					float horzDif_Aft = rEI.InrR2_IOL - rEI.EdgeIOL;
					float horzDif_Tot = rEI.InrR2_IOL - rEI.InrR1_IOL;

					if( horzDif_Bef / horzDif_Tot < 0.35 ||
						horzDif_Aft / horzDif_Tot < 0.35 )
					{
						//rEI.IsCanceled = true;
						//continue;
					}

				}


/*
				{
					int nIOA_E = ioaBuf[ rEI.EdgeIOL ];

					F32ColorVal * pColor_E = & colorBuf[ nIOA_E ];

					//int nIOA_1 = ioaBuf[ rEI.InrR1_IOL ];
					int nIOA_1 = ioaBuf[ rEI.Med1_IOL ];

					const float difVal_1 = difBuf[ nIOA_1 ];

					F32ColorVal * pColor1 = & colorBuf[ nIOA_1 ];

					float colorDif_1_E = F32ColorVal::Sub( *pColor1, *pColor_E ).CalcMag();
					
					//int nIOA_2 = ioaBuf[ rEI.InrR2_IOL ];
					int nIOA_2 = ioaBuf[ rEI.Med2_IOL ];

					const float difVal_2 = difBuf[ nIOA_2 ];

					F32ColorVal * pColor2 = & colorBuf[ nIOA_2 ];

					float colorDif_2_E = F32ColorVal::Sub( *pColor2, *pColor_E ).CalcMag();
					
					float colorDif = F32ColorVal::Sub( *pColor1, *pColor2 ).CalcMag();

					//float horzDif_Tot = rEI.InrR2_IOL - rEI.InrR1_IOL;
					float horzDif_Tot = rEI.Med2_IOL - rEI.Med1_IOL;

					float colorDif_2 = colorDif * 10 / horzDif_Tot;
					//float colorDif_2 = colorDif * 5 / horzDif_Tot;

					rEI.Scaled_EdgeVal = colorDif_2;

					//if( 54152 == rEI.Index )
					//if( 9935 == rEI.Index )
					//if( 4935 == rEI.Index )
					//if( 28339 == rEI.Index )
					//if( 4663 == rEI.Index )
					//if( 9711 == rEI.Index )
					//if( 1687 == rEI.Index )
					if( 1686 == rEI.Index )						
						i = i;

					if( colorDif_2 / difVal_Edge < 0.8 ||
						colorDif_1_E / colorDif > 0.8 ||
						colorDif_2_E / colorDif > 0.8 )
					{
						rEI.Set_DoesConflict( false );

						//rEI.IsCanceled = true;
						//continue;
					}
					else
						i = i;
					
					if( rEI.Scaled_EdgeVal < 17 )
						rEI.Set_DoesConflict( false );

					if( difVal_Edge < 20 )
						rEI.Set_DoesConflict( false );
				}
*/

////////////////////////////////////////////////////////////////////////////////

				{
					int nIOA_R1 = ioaBuf[ rEI.InrR1_IOL ];

					int nIOA_Med1 = ioaBuf[ rEI.Med1_IOL ];

					EdgeScan::PixInfo & rPI_R1 = m_pixInfoArr[ nIOA_R1 ];

					int nIOA_Deep_R1 = rPI_R1.nIOA_VeryDeep_R;

					Hcpl_ASSERT( nIOA_Deep_R1 >= 0 );

					int nIOA_R2 = ioaBuf[ rEI.InrR2_IOL ];

					int nIOA_Med2 = ioaBuf[ rEI.Med2_IOL ];

					EdgeScan::PixInfo & rPI_R2 = m_pixInfoArr[ nIOA_R2 ];

					int nIOA_Deep_R2 = rPI_R2.nIOA_VeryDeep_R;

					Hcpl_ASSERT( nIOA_Deep_R2 >= 0 );


					float colorDif_R1_R2 = m_src->CalcDif( nIOA_R1, nIOA_R2 );

					float colorDif_M1_M2 = m_src->CalcDif( nIOA_Med1, nIOA_Med2 );

					float colorDif_D1_D2 = m_src->CalcDif( nIOA_Deep_R1, nIOA_Deep_R2 );

					//if( 8500 == rEI.Index )
					//if( 41236 == rEI.Index ||
						//41225 == rEI.Index )
					if( 11340 == rEI.Index )
					{
						i = i;						
						//rEI.IsCanceled = true;
						//continue;
					}

					if( colorDif_R1_R2 < colorDif_M1_M2 * 0.8 )
					{
						rEI.IsCanceled = true;
						continue;
					}

					if( colorDif_D1_D2 < colorDif_R1_R2 * 0.8 )
					{
						rEI.IsCanceled = true;
						continue;
					}


					float u_Dif_D1_D2 = m_src->Calc_UnitDif( nIOA_Deep_R1, nIOA_Deep_R2 );

					float u_Dif_R1_R2 = m_src->Calc_UnitDif( nIOA_R1, nIOA_R2 );


					if( u_Dif_D1_D2 < ImgDataElm_Simple::Get_FarUnitDif() &&
						u_Dif_R1_R2 < ImgDataElm_Simple::Get_FarUnitDif() )
					{

						imgPop_D1->Reset();

						imgPop_D1->AddElm_Thick( nIOA_Deep_R1 );

						float sd_D1 = imgPop_D1->CalcStandDiv();


						imgPop_D2->Reset();

						imgPop_D2->AddElm_Thick( nIOA_Deep_R2 );

						float sd_D2 = imgPop_D2->CalcStandDiv();


						imgPop_D1->IncBy( imgPop_D2 );

						float sd_D1_D2 = imgPop_D1->CalcStandDiv();


						float sd_D_Max = ( sd_D1 > sd_D2 ) ? sd_D1 : sd_D2;

						//if( sd_D_Max > sd_D1_D2 * 0.33 )
						if( sd_D_Max > sd_D1_D2 * 0.4 )
						{
							rEI.IsCanceled = false;
							rEI.Set_DoesConflict( false );

							continue;
						}
					}


					//TTTTTTT



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

						if( distToCent < ScanTryDbg::Core.markDistToCent )
						//if( false )
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
					//if( false )
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

						if( distToCent < ScanTryDbg::Core.markDistToCent )
						//if( false )
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



	F32ImageRef ScanTryMgr_6::GenSlopeImg( int a_nDir )
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


	F32ImageRef ScanTryMgr_6::GenFinDifImg( int a_nDir )
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


	F32ImageRef ScanTryMgr_6::GenMaxSlopeImg()
	{
		CvSize srcSiz = m_src->GetSize();

		F32ImageRef ret = F32Image::Create( srcSiz, 1 );

		float * retBuf = ret->GetPixAt( 0, 0 );

		for( int i=0; i < m_pixInfoArr.GetSize(); i++ )
		{
			float val = m_pixInfoArr[ i ].maxSlopeVal;

			//if( val > 50 )
			if( val > 60 )
				val = 100;
			else if( val < 35 )
			//if( val < 15 )
			//if( val > 70 )
			//if( val > 95 )
				val = 50;
			else
				val = 0;

			if( 100 == val )
				i = i;

			val *= (float) 255 / 100;

			retBuf[ i ] = val;
		}

		return ret;
	}


	F32ImageRef ScanTryMgr_6::GenMinSlopeImg()
	{
		CvSize srcSiz = m_src->GetSize();

		F32ImageRef ret = F32Image::Create( srcSiz, 1 );

		float * retBuf = ret->GetPixAt( 0, 0 );

		for( int i=0; i < m_pixInfoArr.GetSize(); i++ )
		{
			float val = ( 100 - m_pixInfoArr[ i ].minSlopeVal );

			//if( val > 30 )
			if( val > 60 )
			//if( val > 95 )
				val = 100;
			else
				val = 0;

			val *= (float) 255 / 100;

			retBuf[ i ] = val;
		}

		return ret;
	}


}


