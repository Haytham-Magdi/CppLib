#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>


namespace Hcv
{
	typedef Hcpl::PtrRef<IplImage> IplImageRef;
	/*class IplImageRef : public Hcpl::PtrRef<IplImage>
	{
		IplImageRef() {}
		IplImageRef(IplImage * a_ptr) : Hcpl::PtrRef<IplImage>(a_ptr) {};
	};*/

	typedef Hcpl::PtrRef<CvMat> CvMatRef;
}