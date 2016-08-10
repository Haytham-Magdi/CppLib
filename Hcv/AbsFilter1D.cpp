#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\AbsFilter1D.h>

#include <math.h>

namespace Hcv
{
	using namespace Hcpl;
	using namespace Hcpl::Math;


	float AbsFilter1D::DoCalcOutput()
	{
		//float outVal = m_valQue.size();
		//float outVal = -999;

		//ThrowHcvException();

		float outVal = fabs( m_valQue.GetBack() );

		return outVal;
	}

}