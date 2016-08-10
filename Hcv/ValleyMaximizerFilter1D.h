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

	class ValleyMaximizerFilter1D : public NonLinearFilter1DBase
	{
	public:
		//Filter1DBase() : m_char('B') {}		

		ValleyMaximizerFilter1D( int a_nAprSiz )// : m_char('V') 
		{
			m_nLength = a_nAprSiz;
			m_char = 'V';
		}


		virtual int GetShift() { return m_nLength / 2; }
		virtual int GetLength() { return m_nLength; }
		
		virtual IFilter1D * Clone() { return new ValleyMaximizerFilter1D( m_nLength ); }

		virtual void InputVal(float a_val)
		{
			NonLinearFilter1DBase::InputVal(a_val);
		}

		virtual void ResetInput()
		{
			NonLinearFilter1DBase::ResetInput();
			//Filter1DBase::ResetInput();
		}


		// Mem Bug !
		//static ValleyMaximizerFilter1D * Create( int a_nAprSiz );

	protected:

		virtual float DoCalcOutput();

	protected:
		int m_nLength;

	};

	typedef Hcpl::ObjRef< ValleyMaximizerFilter1D > ValleyMaximizerFilter1DRef;


}
