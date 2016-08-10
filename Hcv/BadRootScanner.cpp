#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\BadRootScanner.h>

namespace Hcv
{
	using namespace Hcpl;
	//using namespace Hcpl::Math;

	//BadRootScanner::BadRootScanner( F32ImageRef a_srcImg, LinePathRef a_lp  ) : 
	BadRootScanner::BadRootScanner( F32ImageRef a_srcImg ) : 
	m_rgnIndexCalc(a_srcImg->GetWidth(), a_srcImg->GetHeight()), //m_filtSysBld( this ),
		m_nAprSize( GlobalStuff::AprSize1D )
	{
		m_srcImg = a_srcImg;

		m_resImg = a_srcImg->Clone();
		

		//m_lp = a_lp;


		DoPrepare();

		//DoProcess( );
	}

	void BadRootScanner::ProcessLine( LinePathRef a_lp )
	{
		Reset();

		m_lp = a_lp;

		DoProcess( );
	}


	F32ImageRef BadRootScanner::GetResultImg()
	{

		return m_resImg;
	}

	U8ImageRef BadRootScanner::GetSignalDspImg()
	{

		return m_sigDspImg;
	}


	void BadRootScanner::DoPrepare()
	{
		InitRgnInfoVect();

		int maxLen = -1;
		{
			CvSize srcImgSiz = m_srcImg->GetSize();
			maxLen = sqrt( (double)( Sqr( srcImgSiz.width ) + Sqr( srcImgSiz.height ) ) ) * 1.3;
		}

		m_inputRgnPtrVect.SetCapacity( maxLen );

		m_localGradVect.SetCapacity( maxLen );

		m_postDif1Vect.SetCapacity( maxLen );

		m_edgeRgnPtrVect.SetCapacity( maxLen );
		
		m_rootRgnPtrVect.SetCapacity( maxLen );

		m_rootIOLPairVect.SetCapacity( maxLen );

		m_vertixInfoVect.SetCapacity( maxLen );		

		m_filterOutputMgrVect.SetCapacity( 300 );

		CreateFilterSystem();
	}



	void BadRootScanner::CreateFilterSystem()
	{
		m_filtSys = NULL;

		int halfAprSize = m_nAprSize / 2;
		if( 0 == halfAprSize % 2 )
			halfAprSize++;


		{
			ColorFilter1DSystemBuilder fsb1( this );

			int lastMainID = -10;



/////////////


/////////////



			{
				AddColorToSingleConverter( &fsb1,					
					//new SingleOutputMgr( this, u8ColorVal(255, 255, 255) ));
					//new DummyOutputMgr( this ));

					new DebugOutputMgr( this, 							
						new DummyOutputMgr( this )) );
			}


			{
				AddFilter( &fsb1, new MaxFilter1D( 3 ),										
					//new SingleOutputMgr( this, u8ColorVal(0, 140, 255) ) );				
					//new DummyOutputMgr( this ));

					new DebugOutputMgr( this, 							
					//new PostDiff1OutputMgr( this, 												
						new DummyOutputMgr( this )) );

			}


			{
				int nofAvgs = 3;
				//int nofAvgs = 1;

				//int nAprSiz = 5;
				//int nAprSiz = halfAprSize ;
				int nAprSiz = ( halfAprSize > 5 ) ? 5 : halfAprSize;

				for(int i=0; i < nofAvgs; i++)
				{
					if( i == nofAvgs - 1 )
					{
						AddFilter( &fsb1, LinearAvgFilter1D::Create( nAprSiz ),										
							new PostDiff1OutputMgr( this, 							
								//new SingleOutputMgr( this, u8ColorVal(255, 0, 0) )) );				
								new DummyOutputMgr( this )) );
					}
					else
					{
						AddFilter( &fsb1, LinearAvgFilter1D::Create( nAprSiz ),										
							//new DummyOutputMgr( this ) );

							new DebugOutputMgr( this, 							
								new DummyOutputMgr( this )) );
					}
				}
			}


			int diff1SingID = fsb1.GetFilterCnt() - 1;

			{
				AddFilter( &fsb1, new PeakMarkerFilter1D( 20 ),
				//AddFilter( &fsb1, new PeakMarkerFilter1D( 1 ),
					new Diff1MarkerOutputMgr( this, 
						//new SingleOutputMgr( this, u8ColorVal(0, 128, 0) )), diff1SingID );				
						new DummyOutputMgr( this )) );										
			}


			{
				AddFilter( &fsb1, new ValleyMarkerFilter1D( 3 ),
					new Diff2MarkerOutputMgr( this, 
						//new SingleOutputMgr( this, u8ColorVal(0, 0, 255) )), diff1SingID );				
						new DummyOutputMgr( this )), diff1SingID );										


					//new SingleOutputMgr( this, u8ColorVal(160, 0, 160) ), diff1SingID);
					//new DummyOutputMgr( this ), diff1SingID);										
			}

			m_filtSys = fsb1.GetResult();
			m_filtSys->ResetInput();
		}


	}




	int BadRootScanner::AddFilter( ColorFilter1DSystemBuilder * pFiltSysBld, 
		IColorFilter1DRef a_filt, IFilterOutputMgrRef a_outMgr, int a_nSrcID )
	{
		int id = pFiltSysBld->AddFilter( a_filt, a_nSrcID );						

		m_filterOutputMgrVect.PushBack( a_outMgr );

		return id;
	}
	

	int BadRootScanner::AddFilter( ColorFilter1DSystemBuilder * pFiltSysBld, 
		IFilter1DRef a_filt, IFilterOutputMgrRef a_outMgr, int a_nSrcID )
	{
		int id = pFiltSysBld->AddFilter( a_filt, a_nSrcID );						

		m_filterOutputMgrVect.PushBack( a_outMgr );

		return id;
	}
	

	int BadRootScanner::AddColorToSingleConverter( ColorFilter1DSystemBuilder * pFiltSysBld, 
		IFilterOutputMgrRef a_outMgr, int a_nSrcID )
	{
		int id = pFiltSysBld->AddColorToSingleConverter( a_nSrcID );						

		m_filterOutputMgrVect.PushBack( a_outMgr );

		return id;
	}


	void BadRootScanner::InitRgnInfoVect()
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


	void BadRootScanner::DoProcess()
	{
		F32ImageRef img1 = m_srcImg;

		F32ImageRef img1Dsp = m_resImg;

		LinePathRef lp = m_lp;
		

		{
			F32ColorVal color1 = { 180, 0, 0 };

			IIterator_REF( F32Point ) iter1 = lp;


			F32ColorVal * pix2 = NULL;


			int indexOnLine = 0;

			const float divFact = pow( 3.0, 0.5 );

			do
			{
				//F32Point cp = lp->GetCurrent();
				F32Point cp = iter1->GetCurrent();


				{
					RgnInfo * pRgn = GetPointRgn( (int)cp.x, (int)cp.y );

					pRgn->IndexOnLine = indexOnLine++;
					
					m_inputRgnPtrVect.PushBack( pRgn );				

					m_localGradVect.PushBack( 0 );
					//m_localGradVect.PushBack( 1 );

					m_postDif1Vect.PushBack( 0 );
				}

				{
					//pix2 = (F32ColorVal *)img1->GetPixAt( (int)cp.x, (int)cp.y );
					
					float * pVal0 = img1->GetPixAt( (int)cp.x, (int)cp.y );

					float val = *pVal0 / divFact;


					F32ColorVal color1;

					color1.AssignVal( val, val, val );


					static int cnt1 = 0;

					if( 0 == ( cnt1 % 1 ) )
						m_filtSys->InputVal( color1 );

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

			//DoProcessEdges();
			DoProcessEdges2();
		}

	}


	void BadRootScanner::ReceiveFilterOutput( int a_nFilterID, F32ColorVal a_outVal, int a_nBackShift )
	{
		IFilterOutputMgrRef outMgr = m_filterOutputMgrVect[ a_nFilterID ];

		outMgr->ProcessOutput( a_nFilterID, a_outVal, a_nBackShift );
	}

	void BadRootScanner::ReceiveFilterOutput( int a_nFilterID, float a_outVal, int a_nBackShift )
	{
		IFilterOutputMgrRef outMgr = m_filterOutputMgrVect[ a_nFilterID ];

		outMgr->ProcessOutput( a_nFilterID, a_outVal, a_nBackShift );
	}

	void BadRootScanner::NotifyAllOutputDone( )
	{

	}


	void BadRootScanner::ColorOutputMgr::ProcessOutput( int a_nFilterID, F32ColorVal a_outVal, int a_nBackShift )
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

	void BadRootScanner::ColorOutputMgr::ProvideSignal( Signal1DViewerRef a_sv )
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



	void BadRootScanner::SingleOutputMgr::ProcessOutput( int a_nFilterID, float a_outVal, int a_nBackShift )
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

	void BadRootScanner::SingleOutputMgr::ProvideSignal( Signal1DViewerRef a_sv )
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



	void BadRootScanner::DebugOutputMgr::ProcessOutput( 
		int a_nFilterID, float a_outVal, int a_nBackShift )
	{
		ProcessInner( a_nFilterID, a_outVal, a_nBackShift );

		//if( a_outVal < 0 )
		if( a_outVal < -10 )
			a_outVal = a_outVal;

		a_outVal = a_outVal;
	}


	void BadRootScanner::PostDiff1OutputMgr::ProcessOutput( 
		int a_nFilterID, float a_outVal, int a_nBackShift )
	{
		ProcessInner( a_nFilterID, a_outVal, a_nBackShift );

		if( a_outVal < 0 )
			a_outVal = a_outVal;

		m_pIlv->m_postDif1Vect.GetAtBack( a_nBackShift ) = a_outVal;
	}



	void BadRootScanner::LocalGradOutputMgr::ProcessOutput( 
		int a_nFilterID, float a_outVal, int a_nBackShift )
	{
		ProcessInner( a_nFilterID, a_outVal, a_nBackShift );

		m_pIlv->m_localGradVect.GetAtBack( a_nBackShift ) = a_outVal;
	}



	void BadRootScanner::Diff1MarkerOutputMgr::ProcessOutput( 
		int a_nFilterID, float a_outVal, int a_nBackShift )
	{
		ProcessInner( a_nFilterID, a_outVal, a_nBackShift );

		if( a_outVal < 0 )
			a_outVal = a_outVal;

		if( a_outVal > 0 )
		{
			RgnInfo * pRgn = m_pIlv->GetBackInputRgn( a_nBackShift );

			{
				if( 'P' == m_pIlv->m_vertixInfoVect.GetBack().Type )
					return;

				m_pIlv->m_vertixInfoVect.IncSize();

				VertixInfo & rVI = m_pIlv->m_vertixInfoVect.GetBack();

				rVI.Type = 'P';
				rVI.Value = a_outVal;
				rVI.IOL = pRgn->IndexOnLine;

			}
				
		}
	}





	void BadRootScanner::Diff2MarkerOutputMgr::ProcessOutput( 
		int a_nFilterID, float a_outVal, int a_nBackShift )
	{
		ProcessInner( a_nFilterID, a_outVal, a_nBackShift );

		if( a_outVal < 0 )
			a_outVal = a_outVal;

		if( a_outVal < 0 )
		{
			RgnInfo * pRgn = m_pIlv->GetBackInputRgn( a_nBackShift );

			{
				m_pIlv->m_vertixInfoVect.IncSize();

				VertixInfo & rVI = m_pIlv->m_vertixInfoVect.GetBack();

				rVI.Type = 'V';
				rVI.Value = a_outVal;
				rVI.IOL = pRgn->IndexOnLine;
			}

		}
	}


	void BadRootScanner::DoProcessEdges2()
	{

		for( int i = 0; i < m_vertixInfoVect.GetSize(); i++ )
		//for( int i = 1; i < m_vertixInfoVect.GetSize() - 1; i++ )
		{


			//VertixInfo & rViBef = m_vertixInfoVect[ i - 1 ];
			VertixInfo & rViCur = m_vertixInfoVect[ i ];
			//VertixInfo & rViAft = m_vertixInfoVect[ i + 1 ];

			

			if( 'P' == rViCur.Type )
			{
				if( i > 0 )
				{
					if( 'P' == m_vertixInfoVect[ i - 1 ].Type )
						continue;
				}

				//int halfAprSize = m_pIlv->m_nAprSize / 2;
				//int halfAprSize = GlobalStuff::AprSize1D / 2;
				int halfAprSize = m_nAprSize / 2;
				//if( 0 == halfAprSize % 2 )
				//	halfAprSize--;


				{
					RootIOLPair rip1;

					rip1.EdgeIOL = rViCur.IOL;
					rip1.EdgeVal = rViCur.Value;

					{
						bool bDone = false;

						for( int j=i; j >= 0; j-- )
						{
							if( 'P' == m_vertixInfoVect[ j ].Type )
							{
								continue;
							}
							else
							{
								rip1.IOL1 = m_vertixInfoVect[ j ].IOL;
								rip1.ValueR1 = m_vertixInfoVect[ j ].Value;

								bDone = true;
								break;
							}
						}

						if( false == bDone )
						{
							if( 0 == rViCur.IOL )
								continue;

							rip1.IOL1 = 0;
							rip1.ValueR1 = 0;


							bDone = true;							
						}
					}


					{
						bool bDone = false;

						for( int j=i; j < m_vertixInfoVect.GetSize(); j++ )
						{
							if( 'P' == m_vertixInfoVect[ j ].Type )
							{
								continue;
							}
							else
							{
								rip1.IOL2 = m_vertixInfoVect[ j ].IOL;
								rip1.ValueR2 = m_vertixInfoVect[ j ].Value;

								bDone = true;
								break;
							}
						}

						if( false == bDone )
						{
							if( this->m_inputRgnPtrVect.GetSize() - 1 == rViCur.IOL )
								continue;

							rip1.IOL2 = this->m_inputRgnPtrVect.GetSize() - 1;
							rip1.ValueR2 = 0;

							bDone = true;							
						}
					}



					//rip1.IOL1 = rViCur.IOL - halfAprSize;
					//rip1.IOL2 = rViCur.IOL + halfAprSize;

					m_rootIOLPairVect.PushBack( rip1 );
				}


			}


		}

	}


	void BadRootScanner::DoProcessEdges()
	{
/*
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


*/
	}


	//RgnInfo * BadRootScanner::FindRootRgnBef( RgnInfo * a_pEdgeRgnBef, RgnInfo * a_pEdgeRgn )
	BadRootScanner::RgnInfo * BadRootScanner::FindRootRgnBef( BadRootScanner::RgnInfo * a_pEdgeRgn )
	{
		if( 0 == m_rootRgnPtrVect.GetSize() )
			return NULL;

		const int edgeIOL = a_pEdgeRgn->IndexOnLine;

		if( edgeIOL < m_rootRgnPtrVect[ m_rootIdx ]->IndexOnLine )
			return NULL;

		for( ; m_rootIdx < m_rootRgnPtrVect.GetSize(); m_rootIdx++ )
		{
			BadRootScanner::RgnInfo * pRootRgnAft = m_rootRgnPtrVect[ m_rootIdx ];

			if( pRootRgnAft->IndexOnLine > edgeIOL )
			{
				return m_rootRgnPtrVect[ m_rootIdx - 1 ];
			}

		}

		if( m_rootIdx >= m_rootRgnPtrVect.GetSize() )
			return m_rootRgnPtrVect[ m_rootIdx - 1 ];
	}


	//RgnInfo * BadRootScanner::FindRootRgnAft( RgnInfo * a_pEdgeRgn , RgnInfo * a_pEdgeRgnAft)
	BadRootScanner::RgnInfo * BadRootScanner::FindRootRgnAft( int a_limIOL )
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





	void BadRootScanner::DrawCircle( RgnInfo * a_pRgn, U8ColorVal a_color )
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


	void BadRootScanner::Reset()
	{
		m_inputRgnPtrVect.ResetSize();

		m_localGradVect.ResetSize();

		m_postDif1Vect.ResetSize();

		m_edgeRgnPtrVect.ResetSize();

		m_rootRgnPtrVect.ResetSize();

		m_rootIOLPairVect.ResetSize();

		m_vertixInfoVect.ResetSize();

		m_rootIdx = 0;


		m_filtSys->ResetInput();

		//CreateFilterSystem();


		
		

	}







}
