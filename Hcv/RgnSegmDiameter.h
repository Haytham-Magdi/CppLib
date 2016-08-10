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


	class RgnSegmDiameter
	{
	public:

		inline void SetDiamFull( int a_nDiamFull );
		
		//inline void SetDiamInr_1( int a_nDiamInr_1 );
		void SetDiamInr_1( int a_nDiamInr_1 );

		int GetDiamFull()
		{
			return m_nDiamFull;
		}


		int GetDiamCore()
		{
			return m_nDiamCore;
		}

		int GetDiamInr_1()
		{
			return m_nDiamInr_1;
		}
		  
		int GetDiamInr_2()
		{
			//return m_nDiamInr_2;
			return m_nDiamInr_1;
		}



	protected:


		int CalcDiamCore()
		{
			return ( m_nDiamFull - 2 ) / 3;
		}

		int CalcDiamInr_1()
		{			
			int nInr_1 = GetDiamCore() * 2 + 1;
			return nInr_1;
		}
		  
		int CalcDiamInr_2()
		{
			int nInr_2 = m_nDiamFull - GetDiamInr_1() + 1;
			return nInr_2;
		}



	protected:
		
		int m_nDiamFull;
		
		int m_nDiamCore;
		
		int m_nDiamInr_1;
		
		int m_nDiamInr_2;
	};


	typedef Hcpl::ObjRef< RgnSegmDiameter > RgnSegmDiameterRef;

}