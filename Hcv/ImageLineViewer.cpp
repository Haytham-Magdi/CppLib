#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\ImageLineViewer.h>

namespace Hcv
{
	using namespace Hcpl;
	//using namespace Hcpl::Math;

	ImageLineViewer::ImageLineViewer( F32ImageRef a_srcImg, LinePathRef a_lp  ) : 
	m_rgnIndexCalc(a_srcImg->GetWidth(), a_srcImg->GetHeight()), //m_filtSysBld( this ),
		m_nAprSize( GlobalStuff::AprSize1D )
	{
		m_srcImg = a_srcImg;

		m_resImg = a_srcImg->Clone();
		

		m_lp = a_lp;


		DoPrepare();

		DoProcess( );
	}


	F32ImageRef ImageLineViewer::GetResultImg()
	{

		return m_resImg;
	}

	U8ImageRef ImageLineViewer::GetSignalDspImg()
	{

		return m_sigDspImg;
	}


	void ImageLineViewer::DoPrepare()
	{
		InitRgnInfoVect();

		int maxLen = -1;
		{
			CvSize srcImgSiz = m_srcImg->GetSize();
			maxLen = sqrt( (double)( Sqr( srcImgSiz.width ) + Sqr( srcImgSiz.height ) ) ) * 1.3;
		}


		m_inputRgnPtrVect.SetCapacity( maxLen );
		
		int halfAprSize = m_nAprSize / 2;
		if( 0 == halfAprSize % 2 )
			halfAprSize++;

		{
			ColorFilter1DSystemBuilder fsb1( this );

			int lastMainID = -1;
			{
				lastMainID = AddFilter( &fsb1, 
					new CompositeColorFilter1D( LinearAvgFilter1D::Create( m_nAprSize ) ), 
					//new CompositeColorFilter1D( LinearAvgFilter1D::Create( 7 ) ), 
					//new AvgOutputMgr( this ), lastMainID);
					new DummyOutputMgr( this ), lastMainID);										
			}
			
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
				AddFilter( &fsb1, new PeakMarkerFilter1D( ),
					//new SingleOutputMgr( this, u8ColorVal(0, 128, 0) ));
					new Diff1MarkerOutputMgr( this, u8ColorVal(0, 128, 0) ));				
			}


			{
				AddFilter( &fsb1, new ValleyMaximizerFilter1D( m_nAprSize ),
				//AddFilter( &fsb1, new ValleyMaximizerFilter1D( halfAprSize ),
					//new SingleOutputMgr( this, u8ColorVal(160, 0, 160) ), diff1SingID);
					new DummyOutputMgr( this ), diff1SingID);										
			}

			{
				AddFilter( &fsb1, LinearAvgFilter1D::Create( halfAprSize ),										
					//new SingleOutputMgr( this, u8ColorVal(255, 0, 0) ));				
					new SingleOutputMgr( this, u8ColorVal(160, 0, 160) ));
					//new DummyOutputMgr( this ));
			}

			{
				AddFilter( &fsb1, new PeakMarkerFilter1D( ),
					new Diff2MarkerOutputMgr( this, u8ColorVal(0, 0, 255) ));				
			}

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




	int ImageLineViewer::AddFilter( ColorFilter1DSystemBuilder * pFiltSysBld, 
		IColorFilter1DRef a_filt, IFilterOutputMgrRef a_outMgr, int a_nSrcID )
	{
		int id = pFiltSysBld->AddFilter( a_filt, a_nSrcID );						

		m_filterOutputMgrVect.PushBack( a_outMgr );

		return id;
	}
	

	int ImageLineViewer::AddFilter( ColorFilter1DSystemBuilder * pFiltSysBld, 
		IFilter1DRef a_filt, IFilterOutputMgrRef a_outMgr, int a_nSrcID )
	{
		int id = pFiltSysBld->AddFilter( a_filt, a_nSrcID );						

		m_filterOutputMgrVect.PushBack( a_outMgr );

		return id;
	}
	

	int ImageLineViewer::AddColorToSingleConverter( ColorFilter1DSystemBuilder * pFiltSysBld, 
		IFilterOutputMgrRef a_outMgr, int a_nSrcID )
	{
		int id = pFiltSysBld->AddColorToSingleConverter( a_nSrcID );						

		m_filterOutputMgrVect.PushBack( a_outMgr );

		return id;
	}


	void ImageLineViewer::InitRgnInfoVect()
	{
		CvSize srcSiz = m_srcImg->GetSize();

		m_rgnInfoVect.SetSize(srcSiz.width * srcSiz.height);

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

			}
		}


	}


	void ImageLineViewer::DoProcess()
	{
		F32ImageRef img1 = m_srcImg;

		F32ImageRef img1Dsp = m_resImg;

		LinePathRef lp = m_lp;
		

		{
			F32ColorVal color1 = { 180, 0, 0 };

			IIterator_REF( F32Point ) iter1 = lp;


			F32ColorVal * pix2 = NULL;


			do
			{
				//F32Point cp = lp->GetCurrent();
				F32Point cp = iter1->GetCurrent();

				{
					F32ColorVal * pix88 = 
						(F32ColorVal *)img1Dsp->GetPixAt( (int)cp.x, (int)cp.y );

					*pix88 = color1;
				}

				{
					m_inputRgnPtrVect.PushBack( 
						GetPointRgn( (int)cp.x, (int)cp.y ) );
				}

				{
					pix2 = (F32ColorVal *)img1->GetPixAt( (int)cp.x, (int)cp.y );

					m_filtSys->InputVal( *pix2 );
				}


			//}while(lp->MoveNext());
			}while(iter1->MoveNext());

			{
				Signal1DViewerRef sv2 = new Signal1DViewer();

				for( int i=0; i < m_filterOutputMgrVect.GetSize(); i++ )
					m_filterOutputMgrVect[i]->ProvideSignal( sv2 );

				m_sigDspImg = sv2->GenDisplayImage();
			}
		}

	}


	void ImageLineViewer::ReceiveFilterOutput( int a_nFilterID, F32ColorVal a_outVal, int a_nBackShift )
	{
		IFilterOutputMgrRef outMgr = m_filterOutputMgrVect[ a_nFilterID ];

		outMgr->ProcessOutput( a_nFilterID, a_outVal, a_nBackShift );
	}

	void ImageLineViewer::ReceiveFilterOutput( int a_nFilterID, float a_outVal, int a_nBackShift )
	{
		IFilterOutputMgrRef outMgr = m_filterOutputMgrVect[ a_nFilterID ];

		outMgr->ProcessOutput( a_nFilterID, a_outVal, a_nBackShift );
	}

	void ImageLineViewer::NotifyAllOutputDone( )
	{

	}


	void ImageLineViewer::ColorOutputMgr::ProcessOutput( int a_nFilterID, F32ColorVal a_outVal, int a_nBackShift )
	{
		if( NULL == m_sb0 )
		{
			 m_sb0 = new Signal1DBuilder( 700, a_nBackShift );
			 m_sb1 = new Signal1DBuilder( 700, a_nBackShift );
			 m_sb2 = new Signal1DBuilder( 700, a_nBackShift );			
		}

		m_sb0->AddValue( a_outVal.val0 );
		m_sb1->AddValue( a_outVal.val1 );
		m_sb2->AddValue( a_outVal.val2 );
	}

	void ImageLineViewer::ColorOutputMgr::ProvideSignal( Signal1DViewerRef a_sv )
	{
		if( NULL != m_sb0 )
		{
			a_sv->AddSignal( m_sb0->GetResult(), m_valColor0 );
			a_sv->AddSignal( m_sb1->GetResult(), m_valColor1 );
			a_sv->AddSignal( m_sb2->GetResult(), m_valColor2 );		
		}
	}


	void ImageLineViewer::Diff1OutputMgr::ProcessOutput( int a_nFilterID, F32ColorVal a_outVal, int a_nBackShift )
	{
		if( NULL == m_sb )
		{
			 m_sb = new Signal1DBuilder( 700, a_nBackShift );
		}

		const float magVal = a_outVal.CalcMag();

		m_sb->AddValue( magVal );
	}

	void ImageLineViewer::Diff1OutputMgr::ProvideSignal( Signal1DViewerRef a_sv )
	{
		a_sv->AddSignal( m_sb->GetResult(), u8ColorVal(255, 255, 255) );
	}


	void ImageLineViewer::SingleOutputMgr::ProcessOutput( int a_nFilterID, float a_outVal, int a_nBackShift )
	{
		if( NULL == m_sb )
		{
			 m_sb = new Signal1DBuilder( 700, a_nBackShift );
		}

		m_sb->AddValue( a_outVal );
	}

	void ImageLineViewer::SingleOutputMgr::ProvideSignal( Signal1DViewerRef a_sv )
	{
		if( NULL != m_sb )
		{
			a_sv->AddSignal( m_sb->GetResult(), m_color );
		}
	}



	void ImageLineViewer::Diff2OutputMgr::ProcessOutput( int a_nFilterID, F32ColorVal a_outVal, int a_nBackShift )
	{
		if( NULL == m_sb )
		{
			 m_sb = new Signal1DBuilder( 700, a_nBackShift );
		}

		const float magVal = a_outVal.CalcMag();

		m_sb->AddValue( magVal );
		//m_sb->AddValue( 0.5 * magVal );
	}

	void ImageLineViewer::Diff2OutputMgr::ProvideSignal( Signal1DViewerRef a_sv )
	{
		a_sv->AddSignal( m_sb->GetResult(), u8ColorVal(160, 0, 160) );
	}




	void ImageLineViewer::Diff1MarkerOutputMgr::ProcessOutput( 
		int a_nFilterID, float a_outVal, int a_nBackShift )
	{
		SingleOutputMgr::ProcessOutput( a_nFilterID, a_outVal, a_nBackShift );

		if( a_outVal > 0 )
		{
			F32ColorVal color1 = { 0, 180, 0 };

			RgnInfo * pRgn = m_pIlv->GetBackInputRgn( a_nBackShift );
				
			//*pRgn->pShowPix = color1;

			HCV_CALL(

				cvCircle (
					m_pIlv->m_resImg->GetIplImagePtr(),
					cvPoint( pRgn->pos.x, pRgn->pos.y ),
					3,
					//cvScalar( color1.val0, color1.val1, color1.val2 )
					CV_RGB( color1.val2, color1.val1, color1.val0 )
					//CV_RGB( 0, 180, 0 )//, 1, 8
					//int thickness = 1,
					,-1 // CV_FILL
					//int connectivity = 8
				) );
		}

			
			


		//this->



		//a_sv->AddSignal( m_sb->GetResult(), u8ColorVal(0, 128, 255) );
		//a_sv->AddSignal( m_sb->GetResult(), m_color );
	}





	void ImageLineViewer::Diff2MarkerOutputMgr::ProcessOutput( 
		int a_nFilterID, float a_outVal, int a_nBackShift )
	{
		SingleOutputMgr::ProcessOutput( a_nFilterID, a_outVal, a_nBackShift );

		if( a_outVal > 0 )
		{
			F32ColorVal color1 = { 0, 0, 220 };

			RgnInfo * pRgn = m_pIlv->GetBackInputRgn( a_nBackShift );
				
			//*pRgn->pShowPix = color1;

			HCV_CALL(

				cvCircle (
					m_pIlv->m_resImg->GetIplImagePtr(),
					cvPoint( pRgn->pos.x, pRgn->pos.y ),
					3,
					//cvScalar( color1.val0, color1.val1, color1.val2 )
					CV_RGB( color1.val2, color1.val1, color1.val0 )
					//CV_RGB( 0, 180, 0 )//, 1, 8
					//int thickness = 1,
					,-1 // CV_FILL
					//int connectivity = 8
				) );
		}

			
			


		//this->



		//a_sv->AddSignal( m_sb->GetResult(), u8ColorVal(0, 128, 255) );
		//a_sv->AddSignal( m_sb->GetResult(), m_color );
	}










}
