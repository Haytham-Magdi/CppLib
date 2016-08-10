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


	class LineStepper : FRM_Object
	{
	public:

		LineStepper( ){}

		LineStepper( float a_fwdX, float a_fwdY, float a_bkdX, float a_bkdY );

		void SetPoints( float a_fwdX, float a_fwdY, float a_bkdX, float a_bkdY );

		void SetPoints( F32Point & a_fwdPnt, F32Point & a_bkdPnt )
		{
			SetPoints( a_fwdPnt.x, a_fwdPnt.y, a_bkdPnt.x, a_bkdPnt.y );
		}

		F32Point GetCurrentFwd()
		{
			return m_curFwdPnt.Round();
		}

		F32Point GetCurrentBkd()
		{
			return m_curBkdPnt.Round();
		}

		bool FwdMoveNext();
		bool BkdMoveNext();




	protected:


		F32Point m_bgnFwdPnt;
		F32Point m_bgnBkdPnt;

		//int m_nReqSteps;
		int m_nFwdDoneSteps;
		int m_nBkdDoneSteps;

		F32Point m_curFwdPnt;
		F32Point m_curBkdPnt;

		F32Point m_stepPnt;
	};


	typedef Hcpl::ObjRef< LineStepper > LineStepperRef;

}