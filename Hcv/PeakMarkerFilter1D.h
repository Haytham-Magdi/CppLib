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

	class PeakMarkerFilter1D : public NonLinearFilter1DBase
	{
	public:

		PeakMarkerFilter1D( float a_threshold = 20, int a_nAprSiz = 3 )
		{
			m_nLength = a_nAprSiz;
			m_threshold = a_threshold;
		}


		virtual int GetShift() { return m_nLength / 2; }
		virtual int GetLength() { return m_nLength; }
		
		virtual IFilter1D * Clone() { return new PeakMarkerFilter1D( m_threshold, m_nLength ); }

		// Mem Bug !
		//static PeakMarkerFilter1D * Create( int a_nAprSiz );

	protected:

		virtual float DoCalcOutput();

	protected:
		int m_nLength;
		float m_threshold;

	};

	typedef Hcpl::ObjRef< PeakMarkerFilter1D > PeakMarkerFilter1DRef;


}
