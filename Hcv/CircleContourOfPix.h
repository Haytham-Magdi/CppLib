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


	class CircleContourOfPix : FRM_Object
	{
	public:

		CircleContourOfPix( F32ImageRef a_srcImg, int a_radius );

		void PreparePix( int a_x, int a_y );

		FixedVector< float > & GetValArr()
		{
			return m_valArr;
		}

		FixedVector< bool > & GetGoodDirArr()
		{			
			return m_goodDirArr;
		}

		void ShowResult();

	protected:

		void PrepareValArr2( );

		void ShowValArrSignal( FixedVector< float > & a_valArr, char * a_sWndName );

		void InitDirArr();

		//void InitDirShowMap();

		//void ShowCicleImg();

	protected:

		F32ImageRef m_srcImg;
		int m_radius;

		int m_x;
		int m_y;

		FixedVector< F32Point > m_pntArr;

		FixedVector< float > m_valArr;
		FixedVector< float > m_valArr2;

		FixedVector< bool > m_goodArr;

		FixedVector< int > m_dirMap1;
		FixedVector< int > m_dirMap2;

		FixedVector< bool > m_goodDirArr;

		FixedVector< int > m_dirShowMap;

		IFilter1DRef m_avgFlt1;

		float m_midVal;
		float m_orderRatio;
	};


	typedef Hcpl::ObjRef< CircleContourOfPix > CircleContourOfPixRef;

}