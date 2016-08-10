#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\ValleyMarkerFilter1D.h>

#include <math.h>

namespace Hcv
{
	using namespace Hcpl;
	using namespace Hcpl::Math;


	float ValleyMarkerFilter1D::DoCalcOutput()
	{
		float medInp = m_valQue[ m_valQue.GetSize() / 2 ];

		float leftInp = m_valQue.GetFront();
		float rightInp = m_valQue.GetBack();


		float leftDif = medInp - leftInp;
		float rightDif = medInp - rightInp;

		if( leftDif > 0 || rightDif > 0 )
			return 0;

		if( 0 == leftDif && 0 == rightDif )
			return 0;

		return -100;

	}


}