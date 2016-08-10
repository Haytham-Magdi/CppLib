#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\PeakDetectorFilter1D.h>

#include <math.h>

namespace Hcv
{
	using namespace Hcpl;
	using namespace Hcpl::Math;


	float PeakDetectorFilter1D::DoCalcOutput()
	{
		float convVal = Convolve();
		float avgVal = LinearAvgFilter1D::DoCalcOutput();

		//float orgVal = m_valQue._Get_container()[ GetShift() ];
		
		
		float outVal;

		//outVal = convVal;// / ( maskAvg * 255 );


		float lrDif;
		{
			float nrLeftVal = m_valQue._Get_container()[ GetShift() ] -
				m_valQue._Get_container()[ GetShift() - 1 ];

			float nrRightVal = m_valQue._Get_container()[ GetShift() + 1 ] -
				m_valQue._Get_container()[ GetShift() ];

			lrDif = fabs( nrRightVal - nrLeftVal );
			if( lrDif < 1 )
				lrDif = 1;
		}

		float leftVal = m_valQue._Get_container()[ GetShift() ] -
			m_valQue._Get_container()[ 0 ];

		float rightVal = m_valQue._Get_container()[ GetLength() - 1 ] -
			m_valQue._Get_container()[ GetShift() ];

		if( -rightVal > leftVal )
			rightVal = -leftVal;
		else
			leftVal = -rightVal;


		//outVal = ( leftVal - rightVal ) / lrDif;
		outVal = ( leftVal - rightVal );

		if( outVal < 0 || leftVal < 0 || rightVal > 0 )
			outVal = 0;

		//if( outVal <= m_lastOutput )
		//	outVal = 0;

//		if( avgVal > maskAvg )
//			outVal *= ( maskAvg / avgVal );

		if( outVal > 200 )
			outVal = outVal;


/*
		if( avgVal > maskAvg )
		{
			outVal = convVal * maskAvg / avgVal;
		}
		else
		{
			outVal = convVal;
		}
*/

		//return outVal / 5;
		return outVal;		
	}

	void PeakDetectorFilter1D::CreateMask()
	{
		float peakVal = 100;
		const int peakPos = GetShift();

		{
			const int bgnPos = 0;

			float val = 0;

			const float valStep = peakVal / ( peakPos - bgnPos );

			for( int i=0; i <= peakPos; i++, val += valStep )
			{
				m_factorVect[i] = val;
			}
		}

		{
			const int endPos = GetLength() - 1;

			float val = 0;

			const float valStep = peakVal / ( endPos - peakPos );

			for( int i = endPos; i >= peakPos; i--, val += valStep )
			{
				m_factorVect[i] = val;
			}
		}

		{
			float factSum = 0;

			for( int i=0; i < m_factorVect.size(); i++ )
				factSum += m_factorVect[i];

			for( int i=0; i < m_factorVect.size(); i++ )
				m_factorVect[i] /= factSum;

			maskAvg = factSum / m_factorVect.size();
		}
	}

}