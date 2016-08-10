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

#include <Lib\Hcv\LinearAvgFilter1D.h>



//#include <deque>      // for deque
//#include <iostream>   // for cout, endl
//#include <list>       // for list
#include <queue>      // for queue
//#include <string>     // for string


namespace Hcv
{
	using namespace Hcpl::Math;
	using namespace std;

	class PeakDetectorFilter1D : public LinearAvgFilter1D
	{
	public:

		PeakDetectorFilter1D( int a_nAprSiz )
		{
			m_factorVect.resize( a_nAprSiz );
			CreateMask();
		}

		//virtual int GetShift() { return 0; }
		//virtual int GetLength() { return 1; }
		
		virtual IFilter1D * Clone() { return new PeakDetectorFilter1D( m_factorVect.size() ); }
		virtual bool IsLinear() { return false; }

		// Mem Bug !
		//static PeakDetectorFilter1D * Create( int a_nAprSiz );

	protected:

		void CreateMask();

		virtual float DoCalcOutput();

	protected:
		float maskAvg;

	};

	typedef Hcpl::ObjRef< PeakDetectorFilter1D > PeakDetectorFilter1DRef;


}
