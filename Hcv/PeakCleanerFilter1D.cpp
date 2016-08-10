#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\PeakCleanerFilter1D.h>

#include <math.h>

namespace Hcv
{
	using namespace Hcpl;
	using namespace Hcpl::Math;


	float PeakCleanerFilter1D::DoCalcOutput()
	{
		float outVal;


		{
			float lrDif;
			float nrLeftVal = m_valQue[ GetShift() ] -
				m_valQue[ GetShift() - 1 ];

			if( nrLeftVal < 0 )
				return 0;

			float nrRightVal = m_valQue[ GetShift() + 1 ] -
				m_valQue[ GetShift() ];

			if( nrRightVal > 0 )
				return 0;

			lrDif = fabs( nrRightVal - nrLeftVal );
			if( lrDif < 1 )
				lrDif = 1;
		}

		float leftVal = m_valQue[ GetShift() ] -
			m_valQue[ 0 ];

//		if( nrRightVal > 0 )
//			return 0;

		float rightVal = m_valQue[ GetLength() - 1 ] -
			m_valQue[ GetShift() ];

		// minVal
		if( -rightVal > leftVal )
			rightVal = -leftVal;
		else
			leftVal = -rightVal;

		// maxVal, Bad res
		//if( -rightVal > leftVal )
		//	leftVal = -rightVal;
		//else
		//	rightVal = -leftVal;


		//outVal = ( leftVal - rightVal ) / lrDif;
		outVal = ( leftVal - rightVal );

		if( outVal < 0 || leftVal < 0 || rightVal > 0 )
			outVal = 0;

		//outVal = leftVal * 2;

		//if( outVal <= m_lastOutput )
		//	outVal = 0;

//		if( avgVal > maskAvg )
//			outVal *= ( maskAvg / avgVal );

		if( outVal > 200 )
			outVal = outVal;


		return outVal;		
	}


}