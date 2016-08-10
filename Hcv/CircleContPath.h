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


	class CircleContPath : public IIterator< F32Point >
	{
	public:

		//CircleContPath( int a_x1, int a_y1, int a_x2, int a_y2 );
		CircleContPath( int a_bgnDig, int a_limDig, int a_radius );

		F32Point GetCurrent()
		{
			return m_curPnt.Round();
			//return m_curPnt;
		}

		F32Point GetPointOfRad( float a_radVal )
		{
			F32Point pnt1;

			pnt1.x = cos( a_radVal ) * m_radius;
			pnt1.y = sin( a_radVal ) * m_radius;

			pnt1 = pnt1.Round();

			return pnt1;
		}

		float GetCurrentRadian()
		{
			return m_curRad;
		}

		bool MoveNext();




	protected:


		//const F32Point m_bgnPnt;
		//const F32Point m_endPnt;

		//int m_nReqSteps;
		//int m_nDoneSteps;

		F32Point m_curPnt;
		F32Point m_limPnt;

		float m_curRad;
		float m_limRad;

		float m_limRad_05;

		int m_radius;

		//F32Point m_stepPnt;
		float m_stepRad;
	};


	typedef Hcpl::ObjRef< CircleContPath > CircleContPathRef;

}