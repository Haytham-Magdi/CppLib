#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\ValleyMaximizerFilter1D.h>

#include <math.h>

namespace Hcv
{
	using namespace Hcpl;
	using namespace Hcpl::Math;


	float ValleyMaximizerFilter1D::DoCalcOutput()
	{
		float outVal;


		{
			float lrDif;
			float nrLeftVal = m_valQue[ GetShift() ] -
				m_valQue[ GetShift() - 1 ];

			float nrRightVal = m_valQue[ GetShift() + 1 ] -
				m_valQue[ GetShift() ];

			lrDif = fabs( nrRightVal - nrLeftVal );
			if( lrDif < 1 )
				lrDif = 1;
		}

		float leftVal = m_valQue[ GetShift() ] -
			m_valQue[ 0 ];

		float rightVal = m_valQue[ GetLength() - 1 ] -
			m_valQue[ GetShift() ];


		//if( -leftVal > rightVal )
			//rightVal = -leftVal;
		//else
			//leftVal = -rightVal;

		// maxVal, Bad res
		//if( -rightVal > leftVal )
		//	leftVal = -rightVal;
		//else
		//	rightVal = -leftVal;


		//outVal = ( leftVal - rightVal ) / lrDif;
		outVal = rightVal - leftVal;

		//if( outVal < 0 || leftVal < 0 || rightVal > 0 )
		if( outVal < 0 )
			outVal = 0;

		//outVal = outVal * outVal;

		//if( outVal <= m_lastOutput )
		//	outVal = 0;

//		if( avgVal > maskAvg )
//			outVal *= ( maskAvg / avgVal );

		if( outVal > 200 )
			outVal = outVal;


		return outVal;		
	}


}