#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\ScanDirMgrColl.h>

namespace Hcv
{
	using namespace Hcpl;
	//using namespace Hcpl::Math;

	ScanDirMgrColl::ScanDirMgrColl( int a_nofDirs, CvSize a_siz ) : 
		m_angDifMap( a_nofDirs, a_nofDirs )
	{
		m_scanMgrArr.SetSize( a_nofDirs );

		const float stepAngDig = (float)180 / a_nofDirs;

		float angDig = 0;

		ScanDirMgr::s_id = 0;

		for( int i=0; i < a_nofDirs; i++, angDig += stepAngDig )
		{	
			ScanDirMgr & rSdm = m_scanMgrArr[ i ];

			rSdm.Init( angDig, a_siz );
		}

		for( int i=0; i < a_nofDirs; i++ )
		{
			for( int j=0; j <= i; j++ )
			{
				ScanDirMgr & rSdmI = m_scanMgrArr[ i ];
				ScanDirMgr & rSdmJ = m_scanMgrArr[ j ];

				float dif = CalcAngDif( rSdmI.GetAngleByDigree(),
					rSdmJ.GetAngleByDigree() );

				m_angDifMap.GetAt( i, j ) = dif;
				m_angDifMap.GetAt( j, i ) = dif;
			}
		}

		angDig = angDig;
	}

	float ScanDirMgrColl::CalcAngDif( float a_angDig1, float a_angDig2 )
	{
		Hcpl_ASSERT( a_angDig1 >= 0 && a_angDig2 < 180 );

		if( a_angDig1 > 90 )
			//a_angDig1 -= 2 * ( a_angDig1 - 90 );
			a_angDig1 = 180 - a_angDig1;

		if( a_angDig2 > 90 )
			a_angDig2 = 180 - a_angDig2;

		float dif = fabs( a_angDig1 - a_angDig2 );

		return dif;
	}


}