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

	class ScaleFixFilter1D : public LinearAvgFilter1D
	{
	public:

		ScaleFixFilter1D( int a_nAprSiz )
		{
			m_factorVect.resize( a_nAprSiz );
		}

		//virtual int GetShift() { return 0; }
		//virtual int GetLength() { return 1; }
		
		virtual IFilter1D * Clone() { return new ScaleFixFilter1D( m_factorVect.size() ); }
		virtual bool IsLinear() { return false; }

		// Mem Bug !
		//static ScaleFixFilter1D * Create( int a_nAprSiz );

	protected:

		virtual float DoCalcOutput();

	protected:

	};

	typedef Hcpl::ObjRef< ScaleFixFilter1D > ScaleFixFilter1DRef;


}
