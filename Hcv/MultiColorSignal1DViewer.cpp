#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\MultiColorSignal1DViewer.h>

namespace Hcv
{
	using namespace Hcpl;
	using namespace Hcpl::Math;


	MultiColorSignal1DViewer::MultiColorSignal1DViewer( Signal1DViewerRef a_sv )
	{
		m_sv = a_sv;

		if( NULL == a_sv )
			m_sv = new Signal1DViewer();
	}


	void MultiColorSignal1DViewer::DrawValueSignals( 
		FixedVector< float > & a_valArr, FixedVector< U8ColorVal > & a_colorArr )
	{
		Hcpl::Debug::Assert( a_valArr.GetSize() == a_colorArr.GetSize() );

		FixedVector< float > & valArr = a_valArr;

		{				
			const int nScaleW = 800 / valArr.GetSize() + 1;

			//Signal1DViewerRef a_sv1 = new Signal1DViewer();
			int i;

			for( int i=0; i < valArr.GetSize(); i++ )
			{
				Signal1DBuilder sb1( 1000, i * nScaleW );

				float val = valArr[i];

				for( int k=0; k < nScaleW; k++ )
				{
					sb1.AddValue( val );
				}

				//U8ColorVal color1 = u8ColorVal( val, val, val );
				U8ColorVal color1 = a_colorArr[ i ];				

				m_sv->AddSignal( sb1.GetResult(), color1 );
			}


		}



	}


}