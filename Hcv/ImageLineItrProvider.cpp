#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\ImageLineItrProvider.h>

namespace Hcv
{
	using namespace Hcpl;
	//using namespace Hcpl::Math;


	ImageLineItrProvider::ImageLineItrProvider()
	{
		m_badItr.Init( -1, 10000, - 10000, 1 );
	}

	void ImageLineItrProvider::Init( CvSize a_imgSiz, Type a_type )
	{
		m_imgSiz = a_imgSiz;

		m_type = a_type;

		if( ProvType_H == m_type )
		{
			m_lineItrArr.SetSize( m_imgSiz.height );
		}
		else
		{
			m_lineItrArr.SetSize( m_imgSiz.width );
		}
				
	}

	ImageLineItr & ImageLineItrProvider::GetItrForPixIdx( int a_nIdx )
	{
		//ThrowHcvException();


		int nItrIdx;

		if( ProvType_H == m_type )
		{
			nItrIdx = a_nIdx / m_imgSiz.width;
		}
		else
		{
			nItrIdx = a_nIdx % m_imgSiz.width;
		}

		ImageLineItr & rRet = m_lineItrArr[ nItrIdx ];

		//if( ! rRet.SetInitVal( a_nIdx ) )
			//return m_badItr;

		return rRet;

		//return m_badItr;
	}


}