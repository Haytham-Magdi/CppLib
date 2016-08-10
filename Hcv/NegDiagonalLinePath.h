#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Cpp\Math\mathLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>

namespace Hcv
{
	//using namespace Hcpl::Math;


	class NegDiagonalLinePath : public LinePath
	{
	public:

		NegDiagonalLinePath( int a_x1, int a_y1, int a_x2, int a_y2 );
/*
		F32Point GetCurrent()
		{
			F32Point curPnt = m_bgnPnt;

			//curPnt.IncBy( m_accumVal );

			int nAccumVal = (int) ( m_accumVal + 0.5555555555 );

			curPnt.IncBy( nAccumVal, - nAccumVal );

			return curPnt;
		}
*/
		bool MoveNext();




	protected:

/*
		const F32Point m_bgnPnt;
		const F32Point m_endPnt;

		int m_nReqSteps;
		int m_nDoneSteps;

		F32Point m_curPnt;

		F32Point m_stepPnt;
		*/

		float m_accumVal;
		float m_stepVal;
	};


	typedef Hcpl::ObjRef< NegDiagonalLinePath > NegDiagonalLinePathRef;

}