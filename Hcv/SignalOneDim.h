#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Cpp\Math\mathLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>


namespace Hcv
{
	using namespace Hcpl::Math;
	using namespace std;

	class SignalOneDim : FRM_Object
	{
	public:
		vector<int> m_data;
		int m_nBgn;		
	};

	typedef Hcpl::ObjRef< SignalOneDim > SignalOneDimRef;


	class SignalOneDimViewer : FRM_Object
	{
	protected :

		class SignInfo
		{
		public:
			SignalOneDimRef Signal;
			U8ColorVal DspColor;
			float Scale;
		};

	public:
		SignalOneDimViewer();

		void AddSignal( SignalOneDimRef a_signal )
		{
			AddSignal( a_signal, u8ColorVal(255, 0, 0) );
		}

		void AddSignal( SignalOneDimRef a_signal, U8ColorVal a_dspColor, float a_nScale = 1);

		U8ImageRef GenDisplayImage();

	protected:
		vector< SignInfo > m_signalInfoVect;

		U8ColorVal m_bkgColor;
		int m_nMaxSignalLength;
		
	};

	typedef Hcpl::ObjRef< SignalOneDimViewer > SignalOneDimViewerRef;
	

	SignalOneDimRef GenHorzLineFromImage(S16ImageRef a_src);
	SignalOneDimRef GenHorzLineFromImage(S16ImageRef a_src, int a_y, 
		int a_nChannel = 0);
	S16ImageRef GenImageFromHorzSignal(SignalOneDimRef a_sig, int a_nHeight);


}
