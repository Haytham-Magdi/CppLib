#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\LineStepperByAngle.h>

#define M_PI 3.14159265358979323846

namespace Hcv
{
	using namespace Hcpl;
	//using namespace Hcpl::Math;


	LineStepperByAngle::LineStepperByAngle( float a_fwdX, float a_fwdY, float a_angDig ) //:
		//m_bgnFwdPnt(a_fwdX, a_fwdY), m_bgnBkdPnt(a_bkdX, a_bkdY)
	{
		SetValues( a_fwdX, a_fwdY, a_angDig );
	}

	void LineStepperByAngle::SetValues( float a_fwdX, float a_fwdY, float a_angDig )
	{
		m_bgnFwdPnt = F32Point(a_fwdX, a_fwdY);
		//m_bgnBkdPnt = F32Point(a_bkdX, a_bkdY);
		m_bgnBkdPnt = m_bgnFwdPnt;

		m_nFwdDoneSteps = 0;
		m_nBkdDoneSteps = 0;

		m_curFwdPnt = m_bgnFwdPnt;
		m_curBkdPnt = m_bgnBkdPnt;

		//m_nReqSteps = abs( (int) ( m_bgnBkdPnt.x - m_bgnFwdPnt.x ) );

		//const F32Point deltaPnt = F32Point::Sub( (F32Point)m_bgnBkdPnt, (F32Point)m_bgnFwdPnt );
		//F32Point deltaPnt = F32Point::Sub( (F32Point)m_bgnFwdPnt, (F32Point)m_bgnBkdPnt );

		const float angRad = a_angDig * M_PI / 180;

		F32Point deltaPnt;

		deltaPnt.x = cos( angRad );
		deltaPnt.y = sin( angRad );
		
		
		//m_nReqSteps = (int) ( deltaPnt.CalcMag() + 0.55555555555 );

		F32Point absDeltaPnt;
		
		absDeltaPnt.x = fabs( deltaPnt.x );
		absDeltaPnt.y = fabs( deltaPnt.y );

		float maxAbsXY = ( absDeltaPnt.x > absDeltaPnt.y ) ? absDeltaPnt.x : absDeltaPnt.y;

		Hcpl_ASSERT( maxAbsXY > 0.001 );

		m_stepPnt = deltaPnt.DividBy( maxAbsXY );

		//m_stepPnt.x = (int)( m_stepPnt.x * 1000 ) / (float) 1000;
		//m_stepPnt.y = (int)( m_stepPnt.y * 1000 ) / (float) 1000;
		 

		//m_stepPnt = { 1, 0 };		
	}


	bool LineStepperByAngle::FwdMoveNext()
	{		
		m_curFwdPnt.IncBy( m_stepPnt.x, m_stepPnt.y );
		m_nFwdDoneSteps++;

		return true;
	}

	bool LineStepperByAngle::BkdMoveNext()
	{
		m_curBkdPnt.IncBy( - m_stepPnt.x, - m_stepPnt.y );
		m_nBkdDoneSteps++;

		return true;
	}







}