#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Cpp\Math\mathLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>

#include <Lib\Hcv\ImageLineItr.h>

#include <Lib\Hcv\ImageLineItrProvider.h>

#include <Lib\Hcv\ImageItrMgr.h>



namespace Hcv
{
	//using namespace Hcpl::Math;


	class ImageNbrMgr : FRM_Object
	{
	public:

		class PixIndexNbr
		{
		public:

			int nIdxBef;
			int nIdxAft;
		};

	public:

		ImageNbrMgr( ImageItrMgrRef a_srcItr, int a_nAprSiz );

		void Prepare();

		FixedVector< PixIndexNbr > & GetNbr_H_Arr()			
		{				
			return m_nbr_H_Arr;
		}

		FixedVector< PixIndexNbr > & GetNbr_V_Arr()			
		{				
			return m_nbr_V_Arr;
		}

		FixedVector< PixIndexNbr > & GetNbr_Arr_ByNormIdx( int a_nNormIdx );

		int GetAprSiz()
		{
			return m_nAprSiz;
		}

	protected:
		
		void PrepareNbrArr( FixedVector< PixIndexNbr > & a_nbrArr,
			ImageLineItrProvider & a_prov );


	protected:

		ImageItrMgrRef m_srcItr;
		int m_nAprSiz;

		FixedVector< PixIndexNbr > m_nbr_H_Arr;
		FixedVector< PixIndexNbr > m_nbr_V_Arr;

	};


	typedef Hcpl::ObjRef< ImageNbrMgr > ImageNbrMgrRef;

}