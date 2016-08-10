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


namespace Hcv
{
	using namespace Hcpl::Math;
	using namespace std;

	class IFilter1D : FRM_Object
	{
	public:

		virtual void InputVal(float a_val) = 0;
		virtual bool HasOutput() = 0;
		virtual int GetShift() = 0;
		virtual int GetLength() = 0;
		virtual void ResetInput() = 0;
		virtual bool IsLinear() = 0;
		
		virtual IFilter1D * Clone() = 0;

		virtual float CalcOutput()
		{
			m_lastOutput = DoCalcOutput();

			return m_lastOutput;
		}

		float GetLastOutput()
		{
			return m_lastOutput;
		}

		float ProcessVal(float a_val)
		{
			InputVal(a_val);
			return CalcOutput();
		}

		int GetBackShift()
		{
			return GetLength() - 1 - GetShift();
		}

	protected:

		virtual float DoCalcOutput() = 0;
		float m_lastOutput;


	};

	typedef Hcpl::ObjRef< IFilter1D > IFilter1DRef;




	class Filter1DBase : public IFilter1D
	{
	public:

		Filter1DBase() : m_char('B'), m_valQue(1000) {}		

		virtual void InputVal(float a_val);
		virtual int GetShift();
		virtual void ResetInput();
		virtual bool HasOutput();

	protected:

		FixedDeque<float> m_valQue;
		char m_char;

	};



	class LinearFilter1DBase : public Filter1DBase
	{
	public:

		virtual bool IsLinear()
		{
			return true;
		}
	};


	class NonLinearFilter1DBase : public Filter1DBase
	{
	public:

		virtual bool IsLinear()
		{
			return false;
		}
	};


	class Filter1DPosAccum : FRM_Object
	{
	public:

		Filter1DPosAccum( )
		{
			Init( 0, 0 );
		}

		Filter1DPosAccum( int a_nInitPos, int a_nInitAnchPos )
		{
			Init( a_nInitPos, a_nInitAnchPos );
		}

		Filter1DPosAccum( Filter1DPosAccum & a_src )
		{
			Init( a_src.m_nPos, a_src.m_nAnchPos );
		}

		int GetPos()
		{
			return m_nPos;
		}

		int GetAnchPos()
		{
			return m_nAnchPos;
		}

		void Reset()
		{
			Init( 0, 0 );
		}

		int GetBackAnchDiff()
		{
			return m_nPos - m_nAnchPos;
		}

		void Add( IFilter1DRef a_flt )
		{
			m_nPos += a_flt->GetLength() - 1;
			
			m_nAnchPos += a_flt->GetShift();
		}

		void Add( int a_nInitPos, int a_nInitAnchPos )
		{
			m_nPos += a_nInitPos;
			m_nAnchPos += a_nInitAnchPos;
		}

		void Init( int a_nInitPos, int a_nInitAnchPos )
		{
			m_nPos = a_nInitPos;
			m_nAnchPos = a_nInitAnchPos;
		}

	protected:

		int m_nPos;
		int m_nAnchPos;

	};



}
