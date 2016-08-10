#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\NegDiagonalLinePath.h>

namespace Hcv
{
	using namespace Hcpl;
	//using namespace Hcpl::Math;

	NegDiagonalLinePath::NegDiagonalLinePath( int a_x1, int a_y1, int a_x2, int a_y2 ) :
		LinePath( a_x1, a_y1, a_x2, a_y2 )
	{

		F32Point deltaPnt = F32Point::Sub( (F32Point)m_endPnt, (F32Point)m_bgnPnt );

		if( deltaPnt.x != - deltaPnt.y )
			ThrowHcvException();

		m_stepVal = deltaPnt.x / m_nReqSteps;

		m_accumVal = 0;
	}


	bool NegDiagonalLinePath::MoveNext()
	{

		F32Point lastRoundPnt;
		F32Point newRoundPnt;

		newRoundPnt = m_curPnt.Round();			

		do
		{
			m_nDoneSteps++;

			if( m_nDoneSteps > m_nReqSteps )
				return false;

			m_accumVal += m_stepVal;

			m_curPnt = m_bgnPnt;
			m_curPnt.IncBy( m_accumVal, - m_accumVal );
			m_curPnt = m_curPnt.Round();			


			lastRoundPnt = newRoundPnt;

			newRoundPnt = m_curPnt;			

		}while( true == F32Point::Compare( newRoundPnt, lastRoundPnt ) );
		//}while( false );

		return true;

/*
		if( m_nDoneSteps >= m_nReqSteps )
			return false;

		if( F32Point::Compare( this->GetCurrent(), (F32Point)m_endPnt ) )
			m_nDoneSteps = m_nDoneSteps;

		m_accumVal += m_stepVal;

		m_nDoneSteps++;

		return true;
		*/
	}






}