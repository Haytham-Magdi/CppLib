#pragma once

#include <Lib\Hcv\CvIncludes.h>

namespace Hcpl
{
	
	inline void KillObj(IplImage * a_ptr)
	{
		cvReleaseImage(&a_ptr);
	}

	inline void KillObj(CvMat * a_ptr)
	{
		cvReleaseMat(&a_ptr);
	}


}