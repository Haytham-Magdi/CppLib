#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Cpp\Math\mathLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>

#include <Lib\Hcv\Signal1D.h>


namespace Hcv
{
	using namespace Hcpl::Math;
	using namespace std;


	class Signal1DDrawing : FRM_Object
	{
	public:

		Signal1DDrawing()
		{
		}

		Signal1DDrawing( Signal1DRef a_signal, U8ColorVal a_dspColor, float a_scale = 1,
			int a_nShiftX = 0, int a_nShiftY = 0 )
		{
			Signal = a_signal;
			DspColor = a_dspColor;
			Scale = a_scale;

			ShiftX = a_nShiftX;
			ShiftY = a_nShiftY;
		}

	public:
		Signal1DRef Signal;
		U8ColorVal DspColor;
		float Scale;
		int ShiftX;
		int ShiftY;
	};

	typedef Hcpl::ObjRef< Signal1DDrawing > Signal1DDrawingRef;


	class Signal1DViewer : FRM_Object
	{
	protected :


	public:
		Signal1DViewer( int a_nScaleX = 1 );

		void AddSignal( Signal1DRef a_signal )
		{
			AddSignal( a_signal, u8ColorVal(255, 0, 0) );
		}

		void AddSignal( Signal1DRef a_signal, const U8ColorVal a_dspColor, 
			float a_nScale = 1, int a_nShiftX = 0, int a_nShiftY = 0 );

		void AddSignalDrawing( Signal1DDrawingRef a_sigDrw )
		{
			AddSignal( a_sigDrw->Signal, a_sigDrw->DspColor, a_sigDrw->Scale,
				a_sigDrw->ShiftX, a_sigDrw->ShiftY );
		}

		void AddSignalDrawingColl( IIterator_REF( Signal1DDrawingRef ) a_sigDrwIter );

		U8ImageRef GenDisplayImage();

	protected:
		vector< Signal1DDrawing > m_signalInfoVect;

		U8ColorVal m_bkgColor;
		int m_nMaxSignalLength;

		int m_nScaleX;

		const int m_margX;
		const int m_margY;
		
	};

	typedef Hcpl::ObjRef< Signal1DViewer > Signal1DViewerRef;
	

}
