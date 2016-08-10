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

#include <Lib\Hcv\ColorFilter1D.h>


//#include <deque>      // for deque
//#include <iostream>   // for cout, endl
//#include <list>       // for list
#include <queue>      // for queue
//#include <string>     // for string


namespace Hcv
{
	using namespace Hcpl::Math;
	using namespace std;

	class CompositeColorFilter1D;

	class CompositeColorFilter1D : public LinearColorFilter1DBase
	{
	public:

		CompositeColorFilter1D( IFilter1DRef a_singleFilter )
		{
			m_flt0 = a_singleFilter->Clone();
			m_flt1 = a_singleFilter->Clone();
			m_flt2 = a_singleFilter->Clone();			
		}

		virtual void InputVal(F32ColorVal a_val);
		virtual F32ColorVal DoCalcOutput();
		virtual bool HasOutput();
		virtual void ResetInput();

		virtual int GetShift();
		virtual int GetLength();


	protected:

		IFilter1DRef m_flt0;
		IFilter1DRef m_flt1;
		IFilter1DRef m_flt2;
	};

	typedef Hcpl::ObjRef< CompositeColorFilter1D > CompositeColorFilter1DRef;




}
