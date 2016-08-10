#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\ScaleFixFilter1D.h>

#include <math.h>

namespace Hcv
{
	using namespace Hcpl;
	using namespace Hcpl::Math;


	float ScaleFixFilter1D::DoCalcOutput()
	{
		float orgVal = m_valQue[ GetShift() ];
		float avgVal = LinearAvgFilter1D::DoCalcOutput();

		float outVal;

		if( avgVal > 1 )
		{
			outVal = 100 * orgVal / avgVal;
		}
		else
		{
			outVal = avgVal;
		}

		return outVal;
	}

}