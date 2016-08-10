#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>
#include <math.h>

#include <Lib\Hcv\CircleContPath.h>

#define M_PI 3.14159265358979323846

namespace Hcv
{
	using namespace Hcpl;
	//using namespace Hcpl::Math;

	//CircleContPath::CircleContPath( int a_x1, int a_y1, int a_x2, int a_y2 ) :
		//m_bgnPnt(a_x1, a_y1), m_endPnt(a_x2, a_y2), m_nDoneSteps(0)
	CircleContPath::CircleContPath( int a_bgnDig, int a_limDig, int a_radius )
	{
		m_radius = a_radius;

		float bgnRad = m_curRad = a_bgnDig * ( M_PI / 180 );

		m_curPnt = GetPointOfRad( bgnRad );

		m_limRad = a_limDig * ( M_PI / 180 );

		m_limRad_05 = m_limRad / 2;

		m_limPnt = GetPointOfRad( m_limRad );

		m_stepRad = 0.5 * ( M_PI / 180 );
	}


	bool CircleContPath::MoveNext()
	{
		F32Point lastRoundPnt;
		F32Point newPnt;

		float newRad = m_curRad;

		do
		{
			newRad += m_stepRad;

			if( newRad >= m_limRad )
				return false;

			newPnt = GetPointOfRad( newRad );

			if( newRad >= m_limRad_05 )
			{
				if( F32Point::Compare( newPnt, m_limPnt ) )
					return false;
			}

			if( ! F32Point::Compare( m_curPnt, newPnt ) )
				break;

		//}while( F32Point::Compare( m_curPnt, newPnt ) );
		}while( true );


		m_curRad = newRad;
		m_curPnt = newPnt;

		return true;
	}







}