#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Cpp\Math\mathLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>

#include <Lib\Hcv\LinePath.h>

namespace Hcv
{
	//using namespace Hcpl::Math;


	class LinePathInt : public LinePath
	{
	public:

		LinePathInt( int a_x1, int a_y1, int a_x2, int a_y2 );

		F32Point GetCurrent()
		{
			return m_curPnt.Round();
			//return m_curPnt;
		}

		bool MoveNext();




	protected:


		const F32Point m_bgnPnt;
		const F32Point m_endPnt;

		int m_nReqSteps;
		int m_nDoneSteps;

		F32Point m_curPnt;

		F32Point m_stepPnt;
	};


	typedef Hcpl::ObjRef< LinePathInt > LinePathIntRef;

}