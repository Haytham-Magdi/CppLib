#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\LinePath.h>

namespace Hcv
{
	using namespace Hcpl;
	//using namespace Hcpl::Math;

	LinePath::LinePath( int a_x1, int a_y1, int a_x2, int a_y2 ) :
//		m_x1(a_x1), m_y1(a_y1), m_x2(a_x2), m_y2(a_y2)
		m_bgnPnt(a_x1, a_y1), m_endPnt(a_x2, a_y2), m_nDoneSteps(0)
	{

		m_curPnt = m_bgnPnt;

		//m_nReqSteps = abs( (int) ( m_endPnt.x - m_bgnPnt.x ) );

		//const F32Point deltaPnt = F32Point::Sub( (F32Point)m_endPnt, (F32Point)m_bgnPnt );
		F32Point deltaPnt = F32Point::Sub( (F32Point)m_endPnt, (F32Point)m_bgnPnt );

		m_nReqSteps = (int) ( deltaPnt.CalcMag() + 0.555555 );

		m_stepPnt = deltaPnt.DividBy( m_nReqSteps );

		 

		//m_stepPnt = { 1, 0 };		
	}


	bool LinePath::MoveNext()
	{
		if( m_nDoneSteps >= m_nReqSteps )
			return false;

		m_curPnt.IncBy( m_stepPnt );
		m_nDoneSteps++;

		return true;
	}







}