#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\ImageLineItr.h>

namespace Hcv
{
	using namespace Hcpl;
	//using namespace Hcpl::Math;


	void ImageLineItr::Init( int a_nMyIdx, int a_nBgnIdx, int a_nEndIdx, int a_nIncVal,
			int a_nFarBgnIdx, int a_nFarEndIdx )
	{
		m_nMyIdx = a_nMyIdx;

		m_nBgnIdx = a_nBgnIdx;
		m_nEndIdx = a_nEndIdx;

		m_nFarBgnIdx = a_nFarBgnIdx;
		m_nFarEndIdx = a_nFarEndIdx;

		//m_nCurIdx = a_nBgnIdx;

		Hcpl_ASSERT( 0 != a_nIncVal );
		m_nIncVal = a_nIncVal;
	}

/*
	bool ImageLineItr::SetInitVal( int a_nIdxVal )
	{
		bool bOk = ( 0 == ( m_nEndIdx - a_nIdxVal ) % m_nIncVal );

		if( ! bOk )
			goto End;

		if( m_nIncVal > 0 )
		{
			bOk = ( a_nIdxVal >= m_nBgnIdx && a_nIdxVal <= m_nEndIdx );
		}
		else
		{
			bOk = ( a_nIdxVal >= m_nEndIdx && a_nIdxVal <= m_nBgnIdx );
		}

End:
		if( bOk )
		{
			m_nCurIdx = a_nIdxVal;
			
			return true;
		}
		else
		{
			m_nCurIdx = m_nEndIdx + m_nIncVal;

			return false;
		}
	}
*/

	//float ImageLineItr::Get_FarUnitDif()


}