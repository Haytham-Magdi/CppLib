#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Cpp\Math\mathLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\Signal1D.h>

namespace Hcv
{

	class DeltaSignal : public IIterator< F32Point >
	{
	private:

		DeltaSignal( ) {}

	public:

		static Signal1DRef CreateAt( int a_nPos );

	protected:

	};


	typedef Hcpl::ObjRef< DeltaSignal > DeltaSignalRef;

}