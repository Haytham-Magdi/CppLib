#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\SignalOneDim.h>

namespace Hcv
{
	using namespace Hcpl;
	using namespace Hcpl::Math;


	SignalOneDimViewer::SignalOneDimViewer()
	{
		m_nMaxSignalLength = 255;
		m_bkgColor = u8ColorVal(128, 180, 180);
	}

	void SignalOneDimViewer::AddSignal( SignalOneDimRef a_signal, U8ColorVal a_dspColor, float a_nScale )
	{
		int signMax = a_signal->m_nBgn + a_signal->m_data.size();

		if( signMax > m_nMaxSignalLength )
			m_nMaxSignalLength = signMax;

		SignInfo si;

		si.Signal = a_signal;
		si.DspColor = a_dspColor;
		si.Scale = a_nScale;

		this->m_signalInfoVect.push_back(si);
	}


	U8ImageRef SignalOneDimViewer::GenDisplayImage()
	{
		U8ImageRef ret = U8Image::Create( cvSize( m_nMaxSignalLength, 256 ), 3);


		U8ChannelRef retCh0 = ret->GetAt(0);
		U8ChannelRef retCh1 = ret->GetAt(1);
		U8ChannelRef retCh2 = ret->GetAt(2);


		retCh0->SetAll( m_bkgColor.val0 );
		retCh1->SetAll( m_bkgColor.val1 );
		retCh2->SetAll( m_bkgColor.val2 );


		for(int i=0; i < m_signalInfoVect.size(); i++)
		{
			U8ColorVal color = m_signalInfoVect[i].DspColor;
			int x = m_signalInfoVect[i].Signal->m_nBgn;

			vector<int> & rData = m_signalInfoVect[i].Signal->m_data;

			const float scale = m_signalInfoVect[i].Scale;

			for(int j = 1; j < rData.size(); j++, x++)
			{
				int y = 255 - (rData[j] * scale);
				int y0 = 255 - (rData[j-1] * scale);

				//retCh0->SetAt( x, y, color.val0 );
				//retCh1->SetAt( x, y, color.val1 );
				//retCh2->SetAt( x, y, color.val2 );

				cvLine (
					ret->GetIplImagePtr(), 
					cvPoint(x-1, y0), 
					cvPoint(x, y), 
					CV_RGB(color.val2, color.val1, color.val0),
					1 );
			}
		}


		return ret;
	}


}