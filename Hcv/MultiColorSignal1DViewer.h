#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Cpp\Math\mathLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>

//#include <Lib\Hcv\Signal1D.h>

#include <Lib\Hcv\Signal1DViewer.h>



namespace Hcv
{
	using namespace Hcpl::Math;
	using namespace std;

/*
	class Signal1DDrawing : FRM_Object
	{
	public:

		Signal1DDrawing()
		{
		}

		Signal1DDrawing( Signal1DRef a_signal, U8ColorVal a_dspColor, float a_scale )
		{
			Signal = a_signal;
			DspColor = a_dspColor;
			Scale = a_scale;
		}

	public:
		Signal1DRef Signal;
		U8ColorVal DspColor;
		float Scale;
	};

	typedef Hcpl::ObjRef< Signal1DDrawing > Signal1DDrawingRef;
*/

	class MultiColorSignal1DViewer : FRM_Object
	{
	protected :


	public:
		MultiColorSignal1DViewer( Signal1DViewerRef a_sv = NULL );

		U8ImageRef GenDisplayImage()
		{
			return m_sv->GenDisplayImage();
		}

		Signal1DViewerRef GetSignalViewer()
		{
			return m_sv;
		}

		void DrawValueSignals( FixedVector< float > & a_valArr, FixedVector< U8ColorVal > & a_colorArr );

	protected:

		Signal1DViewerRef m_sv;		
	};

	typedef Hcpl::ObjRef< MultiColorSignal1DViewer > MultiColorSignal1DViewerRef;
	

}
