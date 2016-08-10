#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Cpp\Math\mathLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <math.h>



#define M_PI 3.14159265358979323846

namespace Hcv
{
	//using namespace Hcpl::Math;


	class CircleContourOfPix5 : FRM_Object
	{
	public:

		CircleContourOfPix5( F32ImageRef a_srcImg, int a_radius );

		void PreparePix( int a_x, int a_y );

		FixedVector< float > & GetValArr()
		{
			return m_valArr;
		}

		void ShowResult();


		float GetAngleByRad()
		{
			return m_centerRad;
		}

		int GetAngleByDigree()
		{
			//int dig = (int)( m_centerRad * 180 / M_PI ) % 360;
			int dig = (int)( m_centerRad * 180 / M_PI ) % 180;

			return dig;
		}

		int GetCenterValIndex()
		{
			return m_nCenterIdx;
		}		

		float GetDiffMag()
		{
			//return m_valArr[ m_nCenterIdx ];
			return m_diffArr[ m_nCenterIdx ];			
		}		

		F32Point GetFwdPnt()
		{			
			return m_fwdPnt;
		}

		F32Point GetBkdPnt()
		{			
			return m_bkdPnt;
		}


	protected:

		void PrepareValArr2( );

		void ShowValArrSignal( FixedVector< float > & a_valArr, char * a_sWndName );

		void ShowDiffDir();

		//void ShowCicleImg();

	protected:

		F32ImageRef m_srcImg;
		int m_radius;

		int m_x;
		int m_y;

		FixedVector< F32Point > m_pntArr;
		FixedVector< float > m_radianArr;
		FixedVector< float > m_spaceDistArr;

		FixedVector< float > m_diffArr;

		FixedVector< float > m_valArr;

		float m_centerRad;
		int m_nCenterIdx;

		F32Point m_fwdPnt;
		F32Point m_bkdPnt;
	};


	typedef Hcpl::ObjRef< CircleContourOfPix5 > CircleContourOfPix5Ref;

}