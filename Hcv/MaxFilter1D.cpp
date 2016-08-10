#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\MaxFilter1D.h>

#include <math.h>

namespace Hcv
{
	using namespace Hcpl;
	using namespace Hcpl::Math;


	float MaxFilter1D::DoCalcOutput()
	{
		float maxVal = m_valQue[ 0 ];
		int nMaxIdx = 0;

		for( int i=1; i < m_valQue.GetSize(); i++ )
		{
			if( m_valQue[ i ] >  maxVal )
			{
				nMaxIdx = i;
				maxVal = m_valQue[ i ];
			}
		}

		return maxVal;
	}


}