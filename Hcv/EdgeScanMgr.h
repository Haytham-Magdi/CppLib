#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Cpp\Math\mathLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>


#include <Lib\Hcv\EdgeScan.h>

#include <Lib\Hcv\Filt1D2DataMgr.h>
#include <Lib\Hcv\SingFilt1D2.h>



namespace Hcv
{
	//using namespace Hcpl::Math;


	class EdgeScanMgr : FRM_Object
	{
	public:

	protected:

		class DataDraw : FRM_Object
		{
		public:

			DataDraw( )
			{
				ShouldDraw = false;
			}

			DataDraw( ISingFilt1D2DataReaderRef a_dr )
			{
				SetValues( a_dr );
			}

			DataDraw( ISingFilt1D2DataReaderRef a_dr, U8ColorVal a_color, 
				int a_shiftX = 0, int a_shiftY = 0, float a_scale = 1 )
			{
				SetValues( a_dr, a_color, a_shiftX, a_shiftY );
			}

			void SetValues( ISingFilt1D2DataReaderRef a_dr, U8ColorVal a_color, 
				int a_shiftX = 0, int a_shiftY = 0, float a_scale = 1 )
			{
				SetValues( a_dr );
				ShouldDraw = true;
				Color = a_color;
				ShiftX = a_shiftX;
				ShiftY = a_shiftY;
				Scale = a_scale;
			}

			void SetValues( ISingFilt1D2DataReaderRef a_dr )
			{
				DataRdr = a_dr;
				ShouldDraw = false;
				ShiftX = 0;
				ShiftY = 0;
				Scale = 1;
			}

		//protected:

			ISingFilt1D2DataReaderRef DataRdr;
			U8ColorVal Color;
			bool ShouldDraw;
			int ShiftX;
			int ShiftY;
			float Scale;
		};

	public:
		

	protected:


		Signal1DRef GenSignal( ISingFilt1D2DataReaderRef a_rdr, 
			int a_nShiftX = 0, int a_nShiftY = 0 );

		ISingFilt1D2DataReaderRef GenDataReader( Signal1DRef a_sig );


		virtual DataDraw * AddSigDraw( Signal1DRef a_sig, bool a_bDraw = false, 
			U8ColorVal a_color = u8ColorVal( 0, 0, 0 ), 
			int a_shiftX = 0, int a_shiftY = 0, float a_scale = 1 );

		virtual DataDraw * AddSigDraw( ISingFilt1D2DataReaderRef a_dataRdr, bool a_bDraw = false, 
			U8ColorVal a_color = u8ColorVal( 0, 0, 0 ), 
			int a_shiftX = 0, int a_shiftY = 0, float a_scale = 1 );

		virtual DataDraw * AddSigDraw( ISingFilt1D2Ref a_filt, bool a_bDraw = false, 
			U8ColorVal a_color = u8ColorVal( 0, 0, 0 ), 
			int a_shiftX = 0, int a_shiftY = 0, float a_scale = 1 ) = 0;


		DataDraw & GetLastDraw()
		{ 
			return m_sigDrawVect.GetBack();
		}


	protected:

		FixedVector< DataDraw > m_sigDrawVect;
	};


	typedef Hcpl::ObjRef< EdgeScanMgr > EdgeScanMgrRef;

}