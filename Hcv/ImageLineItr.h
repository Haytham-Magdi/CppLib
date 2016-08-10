#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Cpp\Math\mathLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>

#include <Lib\Hcv\Image.h>

#include <Lib\Cpp\Common\NumberIterator.h>


namespace Hcv
{
	//using namespace Hcpl::Math;




	class LineLimit
	{
	public:

		int nBgnIdx;
		int nEndIdx;
	};


	class ImageLineItr // : FRM_Object
	{
	public:

		void Init( int a_nMyIdx, int a_nBgnIdx, int a_nEndIdx, int a_nIncVal,
			int a_nFarBgnIdx, int a_nFarEndIdx );

		void Init( int a_nMyIdx, int a_nBgnIdx, int a_nEndIdx, int a_nIncVal )
		{
			Init( a_nMyIdx, a_nBgnIdx, a_nEndIdx, a_nIncVal,
				a_nBgnIdx, a_nEndIdx );
		}


		Hcpl::NumberIterator GenBgnToEndItr()
		{
			Hcpl::NumberIterator itr;

			itr.Init( m_nBgnIdx, m_nEndIdx, m_nIncVal );

			return itr;
		}

		Hcpl::NumberIterator GenItr( int a_nBgn, int a_nEnd )
		{
			Hcpl::NumberIterator itr;

			itr.Init( a_nBgn, a_nEnd, m_nIncVal );

			return itr;
		}

		int GetMyIdx()
		{
			return m_nMyIdx;
		}

		int GetBgn()
		{
			return m_nBgnIdx;
		}

		int GetEnd()
		{
			return m_nEndIdx;
		}

		int GetAftBgn( int a_nofSteps )
		{
			return m_nBgnIdx + a_nofSteps * m_nIncVal;
		}

		int GetAftEnd( int a_nofSteps )
		{
			return m_nEndIdx + a_nofSteps * m_nIncVal;
		}

		int GetFarBgn()
		{
			return m_nFarBgnIdx;
		}

		int GetFarEnd()
		{
			return m_nFarEndIdx;
		}

		int GetIncVal()
		{
			return m_nIncVal;
		}

	protected:

		int m_nMyIdx;

		int m_nBgnIdx;
		int m_nEndIdx;

		int m_nFarBgnIdx;
		int m_nFarEndIdx;

		//int m_nCurIdx;

		int m_nIncVal;

	};


	//typedef Hcpl::ObjRef< ImageLineItr > ImageLineItrRef;

}