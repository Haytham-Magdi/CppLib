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

namespace Hcv
{
	//using namespace Hcpl::Math;


	class ImageLineItrProvider : FRM_Object
	{
	public:
		typedef enum 
		{ 
			ProvType_H, 
			ProvType_V 
		}Type;
		
	public:

		ImageLineItrProvider();

		void Init( CvSize a_imgSiz, Type a_type );

		FixedVector< ImageLineItr > & GetLineItrArr()
		{
			return m_lineItrArr;
		}

		ImageLineItr & GetItrForPixIdx( int a_nIdx );



	protected:
		
		Type m_type;

		CvSize m_imgSiz;
		
		FixedVector< ImageLineItr > m_lineItrArr;

		ImageLineItr m_badItr;
	};


	typedef Hcpl::ObjRef< ImageLineItrProvider > ImageLineItrProviderRef;

}