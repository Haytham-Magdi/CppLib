#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Cpp\Math\mathLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>

#include <Lib\Hcv\Filter1D.h>

#include <Lib\Hcv\ConvFilter1D.h>



//#include <deque>      // for deque
//#include <iostream>   // for cout, endl
//#include <list>       // for list
#include <queue>      // for queue
//#include <string>     // for string


namespace Hcv
{
	using namespace Hcpl::Math;
	using namespace std;

	class AbsFilter1D : public NonLinearFilter1DBase
	{
	public:

		virtual int GetShift() { return 0; }
		virtual int GetLength() { return 1; }
		
		virtual IFilter1D * Clone() { return new AbsFilter1D(); }

		// Mem Bug !
		//static AbsFilter1D * Create( int a_nAprSiz );

	protected:

		virtual float DoCalcOutput();

	protected:

	};

	typedef Hcpl::ObjRef< AbsFilter1D > AbsFilter1DRef;


}
