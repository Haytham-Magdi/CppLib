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

	class LinearAvgFilter1D : public ConvFilter1D
	{
	public:

		LinearAvgFilter1D()
		{
			m_inpSum = 0;
		}

		virtual void InputVal(float a_val);

		virtual IFilter1D * Clone();

		virtual void ResetInput()
		{
			ConvFilter1D::ResetInput();
			m_inpSum = 0;
		}

		static IFilter1DRef Create( int a_nAprSiz );

		// Mem Bug !
		//static LinearAvgFilter1D * Create( int a_nAprSiz );

	protected:

		virtual float DoCalcOutput();

	protected:

		float m_inpSum;
	};

	typedef Hcpl::ObjRef< LinearAvgFilter1D > LinearAvgFilter1DRef;


}
