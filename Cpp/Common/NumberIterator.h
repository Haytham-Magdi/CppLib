#pragma once

#pragma inline_depth( 255 )

#include <stdlib.h>



#include <Lib\Cpp\Common\Debug.h>
#include <Lib\Cpp\Common\Object.h>


namespace Hcpl
{
	class NumberIterator	// : FRM_Object
	{
	public:

		NumberIterator() {}

		//NumberIterator


		void Init( int a_nBgn, int a_nEnd, int a_nStepVal )
		{
			m_nBgnVal = a_nBgn;
			m_nEndVal = a_nEnd;

			m_nCurVal = a_nBgn;

			m_nStepVal = a_nStepVal;

			m_nLimStep = ( a_nEnd - a_nBgn ) / m_nStepVal + 1;

			if( 142831 == m_nBgnVal )
				m_nBgnVal = m_nBgnVal;
		}

		int GetCurVal()
		{
			return m_nCurVal;
		}

		int GetValBef( int a_nofSteps )
		{
			int nVal = m_nCurVal - 
				m_nStepVal * a_nofSteps;

			return nVal;
		}

		int GetValAft( int a_nofSteps )
		{
			int nVal = m_nCurVal + 
				m_nStepVal * a_nofSteps;

			return nVal;
		}

		int CalcCurStep()
		{
			//int nStep = ( m_nCurVal - m_nBgnVal ) / m_nStepVal;
			int nStep = CalcStep( m_nCurVal );

			return nStep;
		}

		int CalcStep( int a_numVal )
		{
			int nStep = ( a_numVal - m_nBgnVal ) / m_nStepVal;

			return nStep;
		}

		void MoveNext()
		{
			m_nCurVal += m_nStepVal;
		}

		bool IsCurValid()
		{
			int nCurStep = CalcCurStep();

			bool bRet = nCurStep < m_nLimStep;

			//if( ! bRet )
				//bRet = bRet;

			return bRet;
		}

		int GetLimStep()
		{
			return m_nLimStep;
		}

		bool MoveNextAndValidate()
		{
			MoveNext();

			return IsCurValid();
		}

		void GotoBgn()
		{
			m_nCurVal = m_nBgnVal;
		}

		bool SetCurVal( int a_nCurVal )
		{
			bool bRet = 
				0 == ( a_nCurVal - m_nBgnVal ) %  m_nStepVal;

			Hcpl_ASSERT( bRet );

			if( ! bRet )
			{
				throw "Bad val";
			}

			m_nCurVal = a_nCurVal;

			bRet = IsCurValid();

			return bRet;
		}

		int GetStepVal()
		{
			return m_nStepVal;
		}

	protected:

		int m_nBgnVal;
		int m_nEndVal;
		int m_nCurVal;
		int m_nLimStep;
		int m_nStepVal;

	};


	#define NumberIterator_REF(T) Hcpl::ObjRef< NumberIterator<T> >



}