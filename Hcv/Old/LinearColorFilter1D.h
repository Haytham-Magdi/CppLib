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

	class LinearColorFilter1D;

	typedef Hcpl::ObjRef< LinearColorFilter1D > LinearColorFilter1DRef;

	class LinearColorFilter1D : FRM_Object
	{
	public:

		LinearColorFilter1D( IFilter1DRef a_singleFilter )
		{
			m_flt0 = ( ConvFilter1D * )a_singleFilter->Clone();
			m_flt1 = ( ConvFilter1D * )a_singleFilter->Clone();
			m_flt2 = ( ConvFilter1D * )a_singleFilter->Clone();			
		}

		virtual void InputVal(F32ColorVal & a_val);
		virtual F32ColorVal CalcOutput();
		F32ColorVal GetLastOutput();
		virtual bool HasOutput();

		virtual int GetShift();
		virtual int GetLength();

		F32ColorVal ProcessVal(F32ColorVal & a_val)
		{
			InputVal(a_val);
			return CalcOutput();
		}

		static Hcpl::ObjRef< LinearColorFilter1D > CreateAvgFilter( int a_nAprSiz );


		friend class LinearColorFilter1DBuilder;


	protected:

		IFilter1DRef m_flt0;
		IFilter1DRef m_flt1;
		IFilter1DRef m_flt2;
	};




}
