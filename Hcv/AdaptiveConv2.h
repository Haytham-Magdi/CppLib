#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\ConvKernel.h>

namespace Hcv
{
	class AdaptiveConv2 : public ConvKernelFriend
	{
	private :
		AdaptiveConv2() {}
	
	public :
		static AdaptiveConv2 * Create(ConvKernelRef a_convKernel);
		static AdaptiveConv2 * CreateBlur(int a_aprSiz, int a_nSpace = 0)
		{
			return Create(ConvKernel::CreateBlur(a_aprSiz, a_nSpace));
		}

		S16ImageRef GenResult(S16ImageRef a_src, S16ImageRef a_grad);

	protected:
		ConvKernelRef m_convKernel; 
	};

	typedef Hcpl::ObjRef< AdaptiveConv2 > AdaptiveConv2Ref;
}