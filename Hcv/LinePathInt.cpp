#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\LinePathInt.h>

namespace Hcv
{
	using namespace Hcpl;
	//using namespace Hcpl::Math;

	LinePathInt::LinePathInt( int a_x1, int a_y1, int a_x2, int a_y2 ) :
		LinePath( a_x1, a_y1, a_x2, a_y2 ),
//		m_x1(a_x1), m_y1(a_y1), m_x2(a_x2), m_y2(a_y2)
		m_bgnPnt(a_x1, a_y1), m_endPnt(a_x2, a_y2), m_nDoneSteps(0)
	{

		m_curPnt = m_bgnPnt;

		//m_nReqSteps = abs( (int) ( m_endPnt.x - m_bgnPnt.x ) );

		//const F32Point deltaPnt = F32Point::Sub( (F32Point)m_endPnt, (F32Point)m_bgnPnt );
		F32Point deltaPnt = F32Point::Sub( (F32Point)m_endPnt, (F32Point)m_bgnPnt );

		m_nReqSteps = (int) ( deltaPnt.CalcMag() + 0.55555555555 );

		m_stepPnt = deltaPnt.DividBy( m_nReqSteps );

		m_stepPnt.x = (int)( m_stepPnt.x * 1000 ) / (float) 1000;
		m_stepPnt.y = (int)( m_stepPnt.y * 1000 ) / (float) 1000;

		 

		//m_stepPnt = { 1, 0 };		
	}


	bool LinePathInt::MoveNext()
	{
		F32Point lastRoundPnt;
		F32Point newRoundPnt;

		newRoundPnt = m_curPnt.Round();			

		do
		{
			m_nDoneSteps++;

			//if( m_nDoneSteps >= m_nReqSteps )
			if( m_nDoneSteps > m_nReqSteps )
				return false;

			lastRoundPnt = newRoundPnt;
			m_curPnt.IncBy( m_stepPnt );

			newRoundPnt = m_curPnt.Round();			

		}while( true == F32Point::Compare( newRoundPnt, lastRoundPnt ) );
		//}while( false );

		return true;
	}







}