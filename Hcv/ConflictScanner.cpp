#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\ConflictScanner.h>

namespace Hcv
{
	using namespace Hcpl;
	//using namespace Hcpl::Math;

	//ConflictScanner::ConflictScanner( F32ImageRef a_srcImg, LinePathRef a_lp  ) : 
	ConflictScanner::ConflictScanner( F32ImageRef a_srcImg ) : 
	m_rgnIndexCalc(a_srcImg->GetWidth(), a_srcImg->GetHeight()), //m_filtSysBld( this ),
		m_nAprSize( GlobalStuff::AprSize1D )
	{
		m_srcImg = a_srcImg;

		m_resImg = a_srcImg->Clone();
		

		//m_lp = a_lp;


		DoPrepare();

		//DoProcess( );
	}

	void ConflictScanner::ProcessLine( LinePathRef a_lp )
	{
		Reset();

		m_lp = a_lp;

		DoProcess( );
	}


	F32ImageRef ConflictScanner::GetResultImg()
	{

		return m_resImg;
	}

	U8ImageRef ConflictScanner::GetSignalDspImg()
	{

		return m_sigDspImg;
	}


	void ConflictScanner::DoPrepare()
	{
		InitRgnInfoVect();

		int maxLen = -1;
		{
			CvSize srcImgSiz = m_srcImg->GetSize();
			maxLen = sqrt( (double)( Sqr( srcImgSiz.width ) + Sqr( srcImgSiz.height ) ) ) * 1.3;
		}

		m_inputRgnPtrVect.SetCapacity( maxLen );

		m_edgeRgnPtrVect.SetCapacity( maxLen );
		
		m_rootRgnPtrVect.SetCapacity( maxLen );

		m_rootIOLPairVect.SetCapacity( maxLen );

		m_filterOutputMgrVect.SetCapacity( 300 );

		CreateFilterSystem();
	}



	void ConflictScanner::CreateFilterSystem()
	{
		m_filtSys = NULL;

		int halfAprSize = m_nAprSize / 2;
		if( 0 == halfAprSize % 2 )
			halfAprSize++;


		{
			ColorFilter1DSystemBuilder fsb1( this );

			int lastMainID = -1;
/*			{
				lastMainID = AddFilter( &fsb1, 
					new CompositeColorFilter1D( LinearAvgFilter1D::Create( m_nAprSize ) ), 
					//new CompositeColorFilter1D( LinearAvgFilter1D::Create( 7 ) ), 
					//new AvgOutputMgr( this ), lastMainID);
					new DummyOutputMgr( this ), lastMainID);										
			}*/

			{
				lastMainID = AddFilter( &fsb1, 
					new CompositeColorFilter1D( LinearDiffFilter1D::Create( m_nAprSize ) ),
					new DummyOutputMgr( this ), lastMainID);
			}

			int diff1SingID = -1;
			{
				diff1SingID = AddColorToSingleConverter( &fsb1,					
					new SingleOutputMgr( this, u8ColorVal(255, 255, 255) ));
					//new DummyOutputMgr( this ));
			}

			{
				AddFilter( &fsb1, LinearAvgFilter1D::Create( halfAprSize ),										
					//new SingleOutputMgr( this, u8ColorVal(255, 0, 0) ));				
					new DummyOutputMgr( this ));
			}

			// by hthm tmp
			//m_filtSys = fsb1.GetResult();
			//m_filtSys->ResetInput();
			//return;



/*
			{
				AddFilter( &fsb1, LinearDiffFilter1D::Create( m_nAprSize ),					
					//new SingleOutputMgr( this, u8ColorVal(0, 128, 255) ));						
					new DummyOutputMgr( this ));										
			}

			{
				AddFilter( &fsb1, new AbsFilter1D(),					
					//new SingleOutputMgr( this, u8ColorVal(0, 128, 255) ));									
					new SingleOutputMgr( this, u8ColorVal(255, 128, 0) ));									
					//new DummyOutputMgr( this ));										
			}
*/


			{
				AddFilter( &fsb1, new PeakCleanerFilter1D( m_nAprSize ),
					//new SingleOutputMgr( this, u8ColorVal(0, 128, 255) ));						
					new DummyOutputMgr( this ));										
			}


			{
				AddFilter( &fsb1, new PeakMarkerFilter1D( 20 ),
					new Diff1MarkerOutputMgr( this, 
						new SingleOutputMgr( this, u8ColorVal(0, 128, 0) )) );				
						//new DummyOutputMgr( this )) );										
			}

/*
			{
				AddFilter( &fsb1, new ValleyMarkerFilter1D( 3 ),
					new Diff2MarkerOutputMgr( this, 
						new SingleOutputMgr( this, u8ColorVal(255, 0, 0) )), diff1SingID );										
						//new DummyOutputMgr( this )), diff1SingID );										
			}
*/

/*
			{
				AddFilter( &fsb1, new ValleyMaximizerFilter1D( m_nAprSize ),
				//AddFilter( &fsb1, new ValleyMaximizerFilter1D( halfAprSize ),
					//new SingleOutputMgr( this, u8ColorVal(160, 0, 160) ), diff1SingID);
					new DummyOutputMgr( this ), diff1SingID);										
			}

*/
/*			
			{
				AddFilter( &fsb1, LinearAvgFilter1D::Create( halfAprSize ),										
					new SingleOutputMgr( this, u8ColorVal(255, 0, 0) ));				
					//new SingleOutputMgr( this, u8ColorVal(160, 0, 160) ));
					//new DummyOutputMgr( this ));
			}
*/
/*
			{
				AddFilter( &fsb1, new PeakMarkerFilter1D( 10 ),
					new Diff2MarkerOutputMgr( this, 
						new SingleOutputMgr( this, u8ColorVal(255, 0, 0) )) );				
						//new DummyOutputMgr( this )) );										

			}
*/
/*
			{
				lastMainID = AddFilter( &fsb1, 
					new CompositeColorFilter1D( LinearDiffFilter1D::Create( m_nAprSize ) ),
					new DummyOutputMgr( this ), lastMainID);
			}

			{
				AddColorToSingleConverter( &fsb1,
					//new SingleOutputMgr( this, u8ColorVal(160, 0, 160) ));
					//new SingleOutputMgr( this, u8ColorVal(0, 255, 150) ));
					new DummyOutputMgr( this ));										
			}


			{
				AddFilter( &fsb1, LinearAvgFilter1D::Create( halfAprSize ),										
					//new DummyOutputMgr( this ));
					new SingleOutputMgr( this, u8ColorVal(160, 0, 160) ));
			}


			{
				AddFilter( &fsb1, new PeakCleanerFilter1D( m_nAprSize ),										
					new SingleOutputMgr( this, u8ColorVal(0, 255, 150) ));
					//new SingleOutputMgr( this, u8ColorVal(0, 128, 255) ));						
					//new DummyOutputMgr( this ));
			}

			{
				AddFilter( &fsb1, new PeakMarkerFilter1D( ),
					new Diff2MarkerOutputMgr( this, u8ColorVal(0, 0, 255) ));				
			}
*/

			{
				//AddFilter( &fsb1, LinearAvgFilter1D::Create( halfAprSize ),										
				//	new SingleOutputMgr( this, u8ColorVal(0, 128, 255) ));						
			}


/*
			{
				AddFilter( &fsb1, LinearDiffFilter1D::Create( m_nAprSize ),										
					new SingleOutputMgr( this, u8ColorVal(160, 0, 160) ));						
			}
*/

/*
			{
				AddFilter( &fsb1, LinearDiffFilter1D::Create( m_nAprSize ),					
					//new SingleOutputMgr( this, u8ColorVal(0, 128, 255) ));						
					new DummyOutputMgr( this ));										
			}




			{
				AddFilter( &fsb1, new AbsFilter1D(),					
					//new SingleOutputMgr( this, u8ColorVal(0, 128, 255) ));									
					new SingleOutputMgr( this, u8ColorVal(255, 128, 0) ));									
			}

			{
				//AddFilter( &fsb1, LinearAvgFilter1D::Create( 15 ),					
					//new SingleOutputMgr( this, u8ColorVal(0, 128, 255) ));									
			}

			{
				lastMainID = AddFilter( &fsb1, 
					new CompositeColorFilter1D( LinearDiffFilter1D::Create( m_nAprSize ) ),
					new DummyOutputMgr( this ), lastMainID);
			}

			{
				AddColorToSingleConverter( &fsb1,
					new SingleOutputMgr( this, u8ColorVal(160, 0, 160) ));
					//new SingleOutputMgr( this, u8ColorVal(0, 255, 150) ));
					//new DummyOutputMgr( this ));										
			}
*/
			m_filtSys = fsb1.GetResult();
			m_filtSys->ResetInput();
		}


	}




	int ConflictScanner::AddFilter( ColorFilter1DSystemBuilder * pFiltSysBld, 
		IColorFilter1DRef a_filt, IFilterOutputMgrRef a_outMgr, int a_nSrcID )
	{
		int id = pFiltSysBld->AddFilter( a_filt, a_nSrcID );						

		m_filterOutputMgrVect.PushBack( a_outMgr );

		return id;
	}
	

	int ConflictScanner::AddFilter( ColorFilter1DSystemBuilder * pFiltSysBld, 
		IFilter1DRef a_filt, IFilterOutputMgrRef a_outMgr, int a_nSrcID )
	{
		int id = pFiltSysBld->AddFilter( a_filt, a_nSrcID );						

		m_filterOutputMgrVect.PushBack( a_outMgr );

		return id;
	}
	

	int ConflictScanner::AddColorToSingleConverter( ColorFilter1DSystemBuilder * pFiltSysBld, 
		IFilterOutputMgrRef a_outMgr, int a_nSrcID )
	{
		int id = pFiltSysBld->AddColorToSingleConverter( a_nSrcID );						

		m_filterOutputMgrVect.PushBack( a_outMgr );

		return id;
	}


	void ConflictScanner::InitRgnInfoVect()
	{
		CvSize srcSiz = m_srcImg->GetSize();

		m_rgnInfoVect.SetSize(srcSiz.width * srcSiz.height);

		int indexOnImage = 0;

		for(int y=0; y < srcSiz.height; y++)
		{
			for(int x=0; x < srcSiz.width; x++)
			{	
				RgnInfo * pRgn = GetPointRgn(x, y);

				pRgn->pImgPix = (F32ColorVal *) m_srcImg->GetPixAt( x, y );
				pRgn->pShowPix = (F32ColorVal *) m_resImg->GetPixAt( x, y );

				//pRgn->pShowPix->AssignVal( 0, 0, 0 );

				pRgn->pos.x = x;
				pRgn->pos.y = y;	

				pRgn->IndexOnImage = indexOnImage++;
			}
		}

	}


	void ConflictScanner::DoProcess()
	{
		F32ImageRef img1 = m_srcImg;

		F32ImageRef img1Dsp = m_resImg;

		LinePathRef lp = m_lp;
		

		{
			F32ColorVal color1 = { 180, 0, 0 };

			IIterator_REF( F32Point ) iter1 = lp;


			F32ColorVal * pix2 = NULL;


			int indexOnLine = 0;

			do
			{
				//F32Point cp = lp->GetCurrent();
				F32Point cp = iter1->GetCurrent();

				{
					F32ColorVal * pix88 = 
						(F32ColorVal *)img1Dsp->GetPixAt( (int)cp.x, (int)cp.y );

					//*pix88 = color1;
				}

				{
					RgnInfo * pRgn = GetPointRgn( (int)cp.x, (int)cp.y );

					pRgn->IndexOnLine = indexOnLine++;
					
					m_inputRgnPtrVect.PushBack( pRgn );				
				}

				{
					pix2 = (F32ColorVal *)img1->GetPixAt( (int)cp.x, (int)cp.y );

					static int cnt1 = 0;

					if( 0 == ( cnt1 % 1 ) )
						m_filtSys->InputVal( *pix2 );

					cnt1++;
				}


			//}while(lp->MoveNext());
			}while(iter1->MoveNext());

/*
///*


			{
				Signal1DViewerRef sv2 = new Signal1DViewer();

				for( int i=0; i < m_filterOutputMgrVect.GetSize(); i++ )
					m_filterOutputMgrVect[i]->ProvideSignal( sv2 );

				m_sigDspImg = sv2->GenDisplayImage();
			}
*/
//*/
			DoProcessEdges();
		}

	}


	void ConflictScanner::ReceiveFilterOutput( int a_nFilterID, F32ColorVal a_outVal, int a_nBackShift )
	{
		IFilterOutputMgrRef outMgr = m_filterOutputMgrVect[ a_nFilterID ];

		outMgr->ProcessOutput( a_nFilterID, a_outVal, a_nBackShift );
	}

	void ConflictScanner::ReceiveFilterOutput( int a_nFilterID, float a_outVal, int a_nBackShift )
	{
		IFilterOutputMgrRef outMgr = m_filterOutputMgrVect[ a_nFilterID ];

		outMgr->ProcessOutput( a_nFilterID, a_outVal, a_nBackShift );
	}

	void ConflictScanner::NotifyAllOutputDone( )
	{

	}


	void ConflictScanner::ColorOutputMgr::ProcessOutput( int a_nFilterID, F32ColorVal a_outVal, int a_nBackShift )
	{
		ProcessInner( a_nFilterID, a_outVal, a_nBackShift );
/*
///*
		if( NULL == m_sb0 )
		{
			 m_sb0 = new Signal1DBuilder( 700, a_nBackShift );
			 m_sb1 = new Signal1DBuilder( 700, a_nBackShift );
			 m_sb2 = new Signal1DBuilder( 700, a_nBackShift );			
		}

		m_sb0->AddValue( a_outVal.val0 );
		m_sb1->AddValue( a_outVal.val1 );
		m_sb2->AddValue( a_outVal.val2 );
*/
//*/		
	}

	void ConflictScanner::ColorOutputMgr::ProvideSignal( Signal1DViewerRef a_sv )
	{
		ProvideInnerSignal( a_sv );

/*
///*
		if( NULL != m_sb0 )
		{
			a_sv->AddSignal( m_sb0->GetResult(), m_valColor0 );
			a_sv->AddSignal( m_sb1->GetResult(), m_valColor1 );
			a_sv->AddSignal( m_sb2->GetResult(), m_valColor2 );		
		}

		m_sb0 = NULL;		
		m_sb1 = NULL;		
		m_sb2 = NULL;		
*/
//*/
	}



	void ConflictScanner::SingleOutputMgr::ProcessOutput( int a_nFilterID, float a_outVal, int a_nBackShift )
	{
		ProcessInner( a_nFilterID, a_outVal, a_nBackShift );
/*
///*
		if( NULL == m_sb )
		{
			 m_sb = new Signal1DBuilder( 700, a_nBackShift );
		}

		m_sb->AddValue( a_outVal );
*/
//*/

	}

	void ConflictScanner::SingleOutputMgr::ProvideSignal( Signal1DViewerRef a_sv )
	{
		ProvideInnerSignal( a_sv );
/*
///*
		if( NULL != m_sb )
		{
			a_sv->AddSignal( m_sb->GetResult(), m_color );
		}
		
		m_sb = NULL;		
*/
//*/
	}




	void ConflictScanner::Diff1MarkerOutputMgr::ProcessOutput( 
		int a_nFilterID, float a_outVal, int a_nBackShift )
	{
		ProcessInner( a_nFilterID, a_outVal, a_nBackShift );

		if( a_outVal > 0 )
		{
			RgnInfo * pRgn = m_pIlv->GetBackInputRgn( a_nBackShift );
				
			m_pIlv->m_edgeRgnPtrVect.PushBack( pRgn );

//////////////////////////////////


			int halfAprSize = m_pIlv->m_nAprSize / 2;
			if( 0 == halfAprSize % 2 )
				halfAprSize--;

			//{
				RgnInfo * pRgnR1 = m_pIlv->GetBackInputRgn( a_nBackShift + halfAprSize );				
				m_pIlv->m_rootRgnPtrVect.PushBack( pRgnR1 );
			//}

			//{
				RgnInfo * pRgnR2 = m_pIlv->GetBackInputRgn( a_nBackShift - halfAprSize );				
				m_pIlv->m_rootRgnPtrVect.PushBack( pRgnR2 );
			//}

			{
				RootIOLPair rip1;

				rip1.EdgeIOL = pRgn->IndexOnLine;
				rip1.EdgeVal = a_outVal;
				
				//rip1.IOL1 = pFstRootRgn->IndexOnLine;
				//rip1.IOL2 = pSndRootRgn->IndexOnLine;

				rip1.IOL1 = pRgnR1->IndexOnLine;
				rip1.IOL2 = pRgnR2->IndexOnLine;

				m_pIlv->m_rootIOLPairVect.PushBack( rip1 );
			}



		}

	}





	void ConflictScanner::Diff2MarkerOutputMgr::ProcessOutput( 
		int a_nFilterID, float a_outVal, int a_nBackShift )
	{
		ProcessInner( a_nFilterID, a_outVal, a_nBackShift );

		if( a_outVal > 0 )
		{
			RgnInfo * pRgn = m_pIlv->GetBackInputRgn( a_nBackShift );
				
			m_pIlv->m_rootRgnPtrVect.PushBack( pRgn );
		}
	}



	void ConflictScanner::DoProcessEdges()
	{
		U8ColorVal colorEdge( 0, 230, 0 );
		U8ColorVal colorHalfEdge( 200, 200, 100 );

		//U8ColorVal colorRoot( 0, 0, 230 );
		U8ColorVal colorRoot( 255, 0, 0 );
		U8ColorVal colorMedRoot( 200, 100, 200 );
		//U8ColorVal colorMedRoot( 200, 200, 50 );


		{
			for(int i=0; i < m_rootRgnPtrVect.GetSize(); i++)
			{
				//U8ColorVal color1 = SerialColorMgr::GetCurrent();
				//SerialColorMgr::MoveNext();

				RgnInfo * pRgn = m_rootRgnPtrVect[i];

				//DrawCircle( pRgn, colorMedRoot );

			}
		}


		m_rootIdx = 0;

		//for(int i=0, j=0; i < m_edgeRgnPtrVect.GetSize(); i++)
		for(int i=0; i < m_edgeRgnPtrVect.GetSize(); i++)
		{
			//U8ColorVal color1 = { 0, 180, 0 };
			U8ColorVal color1 = colorEdge;
			
			//U8ColorVal color1 = SerialColorMgr::GetCurrent();
			//SerialColorMgr::MoveNext();

			RgnInfo * pEdgeRgn = m_edgeRgnPtrVect[i];

			const int edgeIOL = pEdgeRgn->IndexOnLine;

			RgnInfo * pFstRootRgn = FindRootRgnBef( pEdgeRgn );

			if( NULL == pFstRootRgn )
				continue;

			RgnInfo * pSndRootRgn = FindRootRgnAft( );

			if( NULL == pSndRootRgn )
				continue;

			//RootIOLPair rip1;

			//rip1.EdgeIOL = pEdgeRgn->IndexOnLine;
			//rip1.IOL1 = pFstRootRgn->IndexOnLine;
			//rip1.IOL2 = pSndRootRgn->IndexOnLine;

			//m_rootIOLPairVect.PushBack( rip1 );

			DrawCircle( pFstRootRgn, colorRoot);
			DrawCircle( pSndRootRgn, colorRoot );
			DrawCircle( pEdgeRgn, colorEdge );
		}



	}


	//RgnInfo * ConflictScanner::FindRootRgnBef( RgnInfo * a_pEdgeRgnBef, RgnInfo * a_pEdgeRgn )
	ConflictScanner::RgnInfo * ConflictScanner::FindRootRgnBef( ConflictScanner::RgnInfo * a_pEdgeRgn )
	{
		if( 0 == m_rootRgnPtrVect.GetSize() )
			return NULL;

		const int edgeIOL = a_pEdgeRgn->IndexOnLine;

		if( edgeIOL < m_rootRgnPtrVect[ m_rootIdx ]->IndexOnLine )
			return NULL;

		for( ; m_rootIdx < m_rootRgnPtrVect.GetSize(); m_rootIdx++ )
		{
			ConflictScanner::RgnInfo * pRootRgnAft = m_rootRgnPtrVect[ m_rootIdx ];

			if( pRootRgnAft->IndexOnLine > edgeIOL )
			{
				return m_rootRgnPtrVect[ m_rootIdx - 1 ];
			}

		}

		if( m_rootIdx >= m_rootRgnPtrVect.GetSize() )
			return m_rootRgnPtrVect[ m_rootIdx - 1 ];
	}


	//RgnInfo * ConflictScanner::FindRootRgnAft( RgnInfo * a_pEdgeRgn , RgnInfo * a_pEdgeRgnAft)
	ConflictScanner::RgnInfo * ConflictScanner::FindRootRgnAft( int a_limIOL )
	{
		if( -1 == a_limIOL )
		{
			if( m_rootRgnPtrVect.GetSize() == m_rootIdx )
				return NULL;
		}
		else
		{
			if( m_rootRgnPtrVect[ m_rootIdx ]->IndexOnLine >= a_limIOL )
				return NULL;
		}

		return m_rootRgnPtrVect[ m_rootIdx ];
	}





	void ConflictScanner::DrawCircle( RgnInfo * a_pRgn, U8ColorVal a_color )
	{

		a_pRgn->pShowPix->val0 = a_color.val0;
		a_pRgn->pShowPix->val1 = a_color.val1;
		a_pRgn->pShowPix->val2 = a_color.val2;



/*
		HCV_CALL(

			cvCircle (
				m_resImg->GetIplImagePtr(),
				cvPoint( a_pRgn->pos.x, a_pRgn->pos.y ),
				1,
				CV_RGB( a_color.val2, a_color.val1, a_color.val0 ),
				-1 // CV_FILL
			) );
*/
	}


	void ConflictScanner::Reset()
	{
		m_inputRgnPtrVect.ResetSize();

		m_edgeRgnPtrVect.ResetSize();

		m_rootRgnPtrVect.ResetSize();

		m_rootIOLPairVect.ResetSize();

		m_rootIdx = 0;


		m_filtSys->ResetInput();

		//CreateFilterSystem();


		
		

	}







}
