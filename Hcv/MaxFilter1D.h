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



//#include <deque>      // for deque
//#include <iostream>   // for cout, endl
//#include <list>       // for list
#include <queue>      // for queue
//#include <string>     // for string


namespace Hcv
{
	using namespace Hcpl::Math;
	using namespace std;

	class MaxFilter1D : public NonLinearFilter1DBase
	{
	public:

		MaxFilter1D( int a_nAprSiz = 3 )
		{
			m_nLength = a_nAprSiz;			
		}


		virtual int GetShift() { return m_nLength / 2; }
		virtual int GetLength() { return m_nLength; }
		
		virtual IFilter1D * Clone() { return new MaxFilter1D( m_nLength ); }

		// Mem Bug !
		//static MaxFilter1D * Create( int a_nAprSiz );

	protected:

		virtual float DoCalcOutput();

	protected:
		int m_nLength;

	};

	typedef Hcpl::ObjRef< MaxFilter1D > MaxFilter1DRef;


}
