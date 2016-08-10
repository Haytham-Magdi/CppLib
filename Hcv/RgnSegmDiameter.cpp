#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\RgnSegmDiameter.h>

namespace Hcv
{
	using namespace Hcpl;
	//using namespace Hcpl::Math;



	//inline void RgnSegmDiameter::SetDiamInr_1( int a_nDiamInr_1 )
	void RgnSegmDiameter::SetDiamInr_1( int a_nDiamInr_1 )
	{
		Hcpl_ASSERT( a_nDiamInr_1 % 2 > 0 );

		//int nInr_1 = GetDiamCore() * 2 + 1;

		int nDiamCore = ( a_nDiamInr_1 - 1 ) / 2;

		int nDiamFull = ( nDiamCore * 3 ) + 2;

		SetDiamFull( nDiamFull );
	}


	void RgnSegmDiameter::SetDiamFull( int a_nDiamFull )
	{
		Hcpl_ASSERT( 1 == a_nDiamFull % 2 );

		m_nDiamFull = a_nDiamFull;
	

		m_nDiamCore = CalcDiamCore();
		Hcpl_ASSERT( m_nDiamCore % 2 > 0 );
		
		m_nDiamInr_1 = CalcDiamInr_1();
		Hcpl_ASSERT( m_nDiamInr_1 % 2 > 0 );
		
		m_nDiamInr_2 = CalcDiamInr_2();
		Hcpl_ASSERT( m_nDiamInr_2 % 2 > 0 );
	}



}