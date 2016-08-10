#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Cpp\Math\mathLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>

#include <Lib\Hcv\ScanDirMgr.h>

namespace Hcv
{
	//using namespace Hcpl::Math;


	class ScanDirMgrColl : FRM_Object
	{
	public:

		ScanDirMgrColl( int a_nofDirs, CvSize a_siz );

		ScanDirMgr & GetMgrAt( int a_index )
		{
			return m_scanMgrArr[ a_index ];
		}

		int GetNofMgrs()
		{
			return m_scanMgrArr.GetSize();
		}

		float GetDirAngDif( int a_nDir1, int a_nDir2 )
		{
			return m_angDifMap.GetAt( a_nDir1, a_nDir2 );
		}

	protected:

		float CalcAngDif( float a_angDig1, float a_angDig2 );

	protected:

		FixedVector< ScanDirMgr > m_scanMgrArr;

		//FixedVector< float > m_angDifMap;
		Accessor2D< float > m_angDifMap;
	};


	typedef Hcpl::ObjRef< ScanDirMgrColl > ScanDirMgrCollRef;

}

