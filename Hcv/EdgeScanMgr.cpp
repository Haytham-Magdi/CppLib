#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\EdgeScanMgr.h>

#define ARR_SIZ 3

namespace Hcv
{
	using namespace Hcpl;
	//using namespace Hcpl::Math;

/*
	void EdgeScanMgr::ShowResult()
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
*/


	ISingFilt1D2DataReaderRef EdgeScanMgr::GenDataReader( Signal1DRef a_sig )
	{
		const int nSize = a_sig->m_data.size();

		ISingFilt1D2DataMgrRef dataMgr =
			new SimpleFilt1D2DataMgr< float >( nSize );

		dataMgr->AssignData( & a_sig->m_data[ 0 ], nSize );

		return (ISingFilt1D2DataReaderRef ) dataMgr;
	}


	Signal1DRef EdgeScanMgr::GenSignal( ISingFilt1D2DataReaderRef a_rdr, 
			int a_nShiftX, int a_nShiftY )
	{
		Signal1DBuilder sb0( 1000, a_nShiftX );

		sb0.AddData( a_rdr->GetData(), a_rdr->GetSize() );

		Signal1DRef sig1 = sb0.GetResult();

		for( int i = 0; i < sig1->m_data.size(); i++ )
			sig1->m_data[ i ] += a_nShiftY;

		return sig1;
	}


	EdgeScanMgr::DataDraw * EdgeScanMgr::AddSigDraw( ISingFilt1D2DataReaderRef a_dataRdr, 
		bool a_bDraw, U8ColorVal a_color, int a_shiftX, int a_shiftY, float a_scale )
	{
		m_sigDrawVect.IncSize();

		DataDraw * pSigDraw = & m_sigDrawVect.GetBack();

		pSigDraw->SetValues( a_dataRdr, a_color, a_shiftX, a_shiftY, a_scale );

		pSigDraw->ShouldDraw = a_bDraw;

		return pSigDraw;
	}


	EdgeScanMgr::DataDraw * EdgeScanMgr::AddSigDraw( Signal1DRef a_sig, bool a_bDraw, 
		U8ColorVal a_color, int a_shiftX, int a_shiftY, float a_scale )
	{
		return AddSigDraw( GenDataReader( a_sig ), a_bDraw, a_color, 
			a_shiftX + a_sig->m_nBgn, a_shiftY, a_scale );
	}

/*
	EdgeScanMgr::DataDraw * EdgeScanMgr::AddSigDraw( ISingFilt1D2Ref a_filt, 
		bool a_bDraw, U8ColorVal a_color, int a_shiftX, int a_shiftY, float a_scale )
	{
		m_filtSys->GetFiltArr().PushBack( (IFilt1D2Ref) a_filt );

		return AddSigDraw( a_filt->GetOutputReader(), 
			a_bDraw, a_color, a_shiftX, a_shiftY, a_scale );
	}
*/


}
