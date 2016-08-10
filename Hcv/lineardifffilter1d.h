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

	class LinearDiffFilter1D : public ConvFilter1D
	{
	public:

		//LinearDiffFilter1D( float a_outFact = 1 )
		LinearDiffFilter1D( float a_outFact )
		{
			m_outFact = a_outFact;
		}

		virtual IFilter1D * Clone();

		static IFilter1DRef Create( int a_nAprSiz, float a_outFact = 1 );
		//static IFilter1DRef Create( int a_nAprSiz, float a_outFact );

		// Mem Bug !
		//static LinearDiffFilter1D * Create( int a_nAprSiz );

	protected:

		virtual float DoCalcOutput();

	protected:

		float m_outFact;
	};

	typedef Hcpl::ObjRef< LinearDiffFilter1D > LinearDiffFilter1DRef;


}
