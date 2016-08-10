#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Cpp\Math\mathLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <queue>      

#include <Lib\Hcv\Filter1D.h>


namespace Hcv
{
	using namespace Hcpl::Math;
	using namespace std;

	class IColorFilter1D : FRM_Object
	{
	public:

		virtual void InputVal(F32ColorVal a_val) = 0;
		virtual bool HasOutput() = 0;
		virtual int GetShift() = 0;
		virtual int GetLength() = 0;
		virtual void ResetInput() = 0;
		virtual bool IsLinear() = 0;

		//virtual IColorFilter1D * Clone() = 0;

		void IncPosAccum( Filter1DPosAccum * pPosAccum )
		{
			pPosAccum->Add( this->GetLength() - 1, this->GetShift() );
		}
		
		virtual F32ColorVal CalcOutput()
		{
			m_lastOutput = DoCalcOutput();

			return m_lastOutput;
		}

		F32ColorVal GetLastOutput()
		{
			return m_lastOutput;
		}

		F32ColorVal ProcessVal(F32ColorVal a_val)
		{
			InputVal(a_val);
			return CalcOutput();
		}

		int GetBackShift()
		{
			return GetLength() - 1 - GetShift();
		}

	protected:

		virtual F32ColorVal DoCalcOutput() = 0;
		F32ColorVal m_lastOutput;


	};

	typedef Hcpl::ObjRef< IColorFilter1D > IColorFilter1DRef;




	class ColorFilter1DBase : public IColorFilter1D
	{
	public:

		ColorFilter1DBase() : m_valQue(1000) {}		

		virtual void InputVal(F32ColorVal a_val);
		virtual int GetShift();
		virtual void ResetInput();
		virtual bool HasOutput();

	protected:

		FixedDeque<F32ColorVal> m_valQue;

	};



	class LinearColorFilter1DBase : public ColorFilter1DBase
	{
	public:

		virtual bool IsLinear()
		{
			return true;
		}
	};


	class NonLinearColorFilter1DBase : public ColorFilter1DBase
	{
	public:

		virtual bool IsLinear()
		{
			return false;
		}
	};


}
