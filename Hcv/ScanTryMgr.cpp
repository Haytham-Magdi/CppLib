#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\ScanTryMgr.h>

#define ARR_SIZ 3

namespace Hcv
{
	using namespace Hcpl;
	//using namespace Hcpl::Math;

	using namespace Hcv::Ns_ImgGradMgr;

	ScanTryMgr::ScanTryMgr( int a_x, int a_y )
	{

		//a_x = 121;
		//a_y = 349;

		//a_x = 270;
		//a_y = 199;

		//a_x = 118;
		//a_y = 342;

		//a_x = 350;
		//a_y = 342;

		//a_x = 249;
		//a_y = 261;

		//a_x = 632;
		//a_y = 396;



		m_sigDrawVect.SetCapacity( 300 );

		const int nRadius = GlobalStuff::AprSize1D / 2;
		const int nDiameter = nRadius * 2 + 1;
		//const int nRadius = 3;
		

		F32ImageRef src = GlobalStuff::GetLinePathImg();

		//CircDiff2Ref cd1 = new CircDiff2( src, nRadius );
		//ImgGradMgr igm1( src, 7, 8 );
		ImgGradMgr igm1( src, nDiameter, 8 );		
		//ImgGradMgr igm1( src, 7, 4 );




		//LineStepper ls1;
		{
			ImgGradMgr::FwdBkdPoints fBPnts = igm1.GetFwdBkdMatrix().GetAt( a_x, a_y );

			CvSize srcSiz = src->GetSize();

			IndexCalc2D idx2D( srcSiz.width, srcSiz.height );

			const int nPI = idx2D.Calc( a_x, a_y );

			S32ImageRef dirImg = igm1.GetGradDirImg();
			Hcpl::Int32 * dirBuf = dirImg->GetPixAt( 0, 0 );

			const int nDir = dirBuf[ nPI ];

/*
			{
				Signal1DRef sig1 = igm1.GenLineSignal( F32Point( a_x, a_y ), nDir );

				Signal1DViewer sv1;

				sv1.AddSignal( sig1 );

				ShowImage( sv1.GenDisplayImage(), "G D L S" );
			}
*/




			F32ImageRef difMagImg = igm1.GetGradMagImg();
			

			//ls1.SetPoints( fBPnts.FwdPnt, fBPnts.BkdPnt );

			
			ScanDirMgrCollRef sdmColl = igm1.GetScanDirMgrColl();


			ScanDirMgr & rScanMgr = sdmColl->GetMgrAt( nDir );

			FixedVector< ScanDirMgr::PixLineInfo > & pliArr =
				rScanMgr.GetPixLineInfoArr();

			ScanDirMgr::PixLineInfo & rPli = pliArr[ nPI ];

			const int nBPI = rScanMgr.GetBgnIndexArr()[ rPli.nLineNum ];

			FixedVector< int > & rShiftArr  = rScanMgr.GetShiftIndexArr();


			F32ImageRef dspImg = src->Clone();

			{
				Signal1DBuilder sb1;
				Signal1DBuilder sb2;
				Signal1DBuilder sb3;

				ScanLineItrRef sli = igm1.GenScanLineItr( nPI, nDir );

				sli->GotoFirst();

				F32ColorVal * dspImgBuf = (F32ColorVal *) dspImg->GetPixAt( 0, 0 );

				//const F32ColorVal colorScan = { 0, 255, 0 };
				//const F32ColorVal colorLoc = { 255, 0, 0 };

				m_colorScan.AssignVal( 255, 0, 0 );
				m_colorLoc.AssignVal( 0, 180, 0 );

				FixedVector< float > & gradDataArr = igm1.GetGradDataArr();

				const int nofDirs = sdmColl->GetNofMgrs();

				do
				{
					const int nCPI = sli->GetCurPI();

					F32ColorVal color1;

					const int nCurDir = dirBuf[ nCPI ];

					{	
						const int nValIdx = nDir + nCPI * nofDirs;
						float val = gradDataArr[ nValIdx ];

						sb1.AddValue( val );
					}

					{	
						const int nValIdx = nCurDir + nCPI * nofDirs;							
						float val = gradDataArr[ nValIdx ];

						sb2.AddValue( val );
					}

					float angDif = sdmColl->GetDirAngDif( nDir, nCurDir );

					//if( angDif < 30 )
					//if( angDif < 25 )
					if( angDif < 15 )
						color1 = m_colorScan;
					else
						color1 = m_colorLoc;

					//sb3.AddValue( angDif + 50 );
					sb3.AddValue( angDif );

					//dspImgBuf[ nCPI ] = color1;

					F32Point pnt1 = F32Point::CreateFromIndex( nCPI, srcSiz );
					DrawCircle( dspImg, pnt1, color1.ToU8(), 2 );
				}
				while( sli->MoveNext() );

				DrawCircle( dspImg, F32Point( a_x, a_y ), 
					u8ColorVal( 0, 0, 255 ), 2 );

				m_sigLoc = sb2.GetResult();
				m_sigScan = sb1.GetResult();

				m_sigAngDif = sb3.GetResult();
				{
					const int nSize = m_sigLoc->m_data.size();

					float * pBufLoc = & m_sigLoc->m_data[ 0 ];
					float * pBufAng = & m_sigAngDif->m_data[ 0 ];

					for( int i=0; i < nSize; i++ )
					{
						if( 0 == pBufLoc[ i ] )
							pBufAng[ i ] = 0;
					}
				}

				
				{
					const int nSI = rScanMgr.GetShiftStartArr()[ rPli.nLineNum ];

					m_sigPosDelta = DeltaSignal::CreateAt( rPli.nIOL - nSI );
				}

			}

			GlobalStuff::ShowLinePathImg( dspImg );
			//ShowImage( igm1.GenIsPeakImg(), "IsPeakImg" );

		}


		PrepareFiltSys();

		PrepareAperSizes();

		m_filtSys->Proceed();

		ShowResult();
	}


	EdgeScanMgr::DataDraw * ScanTryMgr::AddSigDraw( ISingFilt1D2Ref a_filt, 
	//DataDraw * ScanTryMgr::AddSigDraw( ISingFilt1D2Ref a_filt, 
		bool a_bDraw, U8ColorVal a_color, int a_shiftX, int a_shiftY, float a_scale )
	{
		m_filtSys->GetFiltArr().PushBack( (IFilt1D2Ref) a_filt );

		//return EdgeScanMgr::AddSigDraw( a_filt->GetOutputReader(), 
		return AddSigDraw( a_filt->GetOutputReader(), 
			a_bDraw, a_color, a_shiftX, a_shiftY, a_scale );
		//return NULL;
	}


	void ScanTryMgr::ShowResult()
	{
		Signal1DViewer sv1;

		for( int i=0; i < m_sigDrawVect.GetSize(); i++ )
		{
			DataDraw & rSD = m_sigDrawVect[ i ];

			if( ! rSD.ShouldDraw )
				continue;

			//sv1.AddSignal( GenSignal( rSD.DataRdr, rSD.ShiftX, rSD.ShiftY ), rSD.Color );
			sv1.AddSignal( GenSignal( rSD.DataRdr ), rSD.Color,
				rSD.Scale, rSD.ShiftX, - rSD.ShiftY );
		}


		ShowImage( sv1.GenDisplayImage(), "G D L S" );
	}




	void ScanTryMgr::PrepareFiltSys_0()
	{
		const int nSize = m_sigLoc->m_data.size();

		m_filtSys = new Filt1D2SysEmpty( nSize );
		
		
		{
			AddSigDraw( m_sigLoc, true, m_colorLoc.ToU8() );
		}
		ISingFilt1D2DataReaderRef locRdr = GetLastDraw().DataRdr;


		{
			AddSigDraw( m_sigScan, true, m_colorScan.ToU8() );
		}
		ISingFilt1D2DataReaderRef scanRdr = GetLastDraw().DataRdr;


		AddSigDraw( m_sigAngDif, true, u8ColorVal( 255, 0, 255 ) );

		{
			AddSigDraw( new AvgFilt1D2( 7, locRdr ), 
				true, u8ColorVal( 255, 255, 255 ) );
		}
		ISingFilt1D2DataReaderRef avgRdr = GetLastDraw().DataRdr;


		AddSigDraw( m_sigPosDelta, true, u8ColorVal( 0, 0, 255 ) );
	}


	void ScanTryMgr::PrepareFiltSys()
	{
		const int nSize = m_sigLoc->m_data.size();

		int nShiftY_1 = 100;
		int nShiftY_2 = 180;
		

		m_filtSys = new Filt1D2SysEmpty( nSize );
		

		{
			//AddSigDraw( m_sigAngDif, true, u8ColorVal( 255, 0, 255 ), 0, nShiftY_1 - 20 );
			AddSigDraw( m_sigAngDif, false, u8ColorVal( 255, 0, 255 ) );

			//AddSigDraw( Signal1DBuilder::CreateConstSignal( 
				//0, GetLastDraw().DataRdr->GetSize() ),
				//false, u8ColorVal( 255, 0, 255 ) );

			AddSigDraw( new AvgFilt1D2( 5, GetLastDraw().DataRdr ),
				//true, GetLastDraw().Color, 0, nShiftY_2 );
				true, GetLastDraw().Color, 0, 0 );
				//false, GetLastDraw().Color );
		}
		ISingFilt1D2DataReaderRef angDifRdr = GetLastDraw().DataRdr;


		{
			AddSigDraw( Signal1DBuilder::CreateConstSignal(
				0, GetLastDraw().DataRdr->GetSize() ), 
				true, u8ColorVal( 0, 0, 0 ), 0, nShiftY_2 );
		}


///////////////////////


		{
			//AddSigDraw( m_sigLoc, true, m_colorLoc.ToU8() );
			AddSigDraw( m_sigLoc, false, m_colorLoc.ToU8() );

			AddSigDraw( new AvgFilt1D2( 5, GetLastDraw().DataRdr ),
				true, GetLastDraw().Color, 0, GetLastDraw().ShiftY );
		}
		ISingFilt1D2DataReaderRef locRdr = GetLastDraw().DataRdr;


		{
			//AddSigDraw( m_sigScan, false, m_colorScan.ToU8() );
			AddSigDraw( m_sigScan, true, m_colorScan.ToU8() );
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

				//for( int i=0; i < 0; i++ )
				for( int i=0; i < 1; i++ )
				//for( int i=0; i < 3; i++ )
				//for( int i=0; i < 5; i++ )
				{
					AddSigDraw( new AvgFilt1D2( nAprSiz, GetLastDraw().DataRdr ),
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

/*
			{
				AddSigDraw( new PeakMarkerFilt1D2( 5, scanRdr, locRdr, dif2LocRdr, 
					angDifRdr, GetLastDraw().DataRdr, ZCPosRdr ), 
					//true, pDif3Draw->Color, 0, nShiftY_2 + 10 );
					true, pDif3Draw->Color, 0, 0 );
					//false, pDif3Draw->Color, 0, nShiftY_2 + 10 );
			}
*/

		}


///////////////////////

		nShiftY_1 += 10;
		nShiftY_2 += 10;

/*

		AddSigDraw( Signal1DBuilder::CreateConstSignal(
			0, GetLastDraw().DataRdr->GetSize() ), 
			//true, u8ColorVal( 0, 0, 0 ), 0, nShiftY_1 );
			false, u8ColorVal( 0, 0, 0 ), 0, nShiftY_1 );
		
		//AddSigDraw( Signal1DBuilder::CreateConstSignal(
			//nShiftY_1, GetLastDraw().DataRdr->GetSize() ), 
			//true, u8ColorVal( 0, 0, 0 ) );


		{
			AddSigDraw( m_sigScan, true, m_colorScan.ToU8() );
		}
		ISingFilt1D2DataReaderRef scanRdr = GetLastDraw().DataRdr;


		{
			const int nAprSiz = 5;
			//const int nAprSiz = 7;

			for( int i=0; i < 1; i++ )
			//for( int i=0; i < 3; i++ )
			//for( int i=0; i < 5; i++ )
			{
				AddSigDraw( new AvgFilt1D2( nAprSiz, GetLastDraw().DataRdr ),
					false, GetLastDraw().Color, 0, GetLastDraw().ShiftY );
			}
			GetLastDraw().ShouldDraw = true;
		}


		{
			AddSigDraw( new DifFilt1D2( 7, GetLastDraw().DataRdr ), 
				//true, u8ColorVal( 0, 255, 255 ), 0, nShiftY_1, 3 );
				true, GetLastDraw().Color, 0, nShiftY_1, 3 );
		}
		ISingFilt1D2DataReaderRef dif2ScanRdr = GetLastDraw().DataRdr;


		{
			AddSigDraw( new PeakMarkerFilt1D2( 3, dif2ScanRdr, angDifRdr ), 
				true, GetLastDraw().Color, 0, nShiftY_2 );
		}
*/

		AddSigDraw( m_sigPosDelta, true, u8ColorVal( 0, 0, 255 ) );

		//AddSigDraw( DeltaSignal::CreateAt( 12 ), 
			//true, u8ColorVal( 0, 0, 255 ) );
	}


	void ScanTryMgr::PrepareAperSizes()
	{
		FixedVector< IFilt1D2Ref > & rFiltArr = m_filtSys->GetFiltArr();

		for( int i=0; i < rFiltArr.GetSize(); i++ )
		{
			IFilt1D2Ref filt = rFiltArr[ i ];

		}

	}


}