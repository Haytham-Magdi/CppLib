#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\PNEdgeBuilder.h>

namespace Hcv
{
	using namespace Hcpl;
	using namespace Hcpl::Math;

	PNEdgeBuilder::PNEdgeBuilder(F32ImageRef a_src, int a_nAprSiz) :
		m_nMaxOrder(3), m_nAprSiz(a_nAprSiz)
	{
		m_src = a_src;

		m_derivesX.resize( GetMaxOrder() + 1 );
		m_derivesX[0] = m_src;

		m_derivesY.resize( GetMaxOrder() + 1 );
		m_derivesY[0] = m_src;

		m_deriveMags.resize( GetMaxOrder() + 1 );
		m_deriveMags[0] = m_src;
	}


	F32ImageRef PNEdgeBuilder::GenImageX(int a_nOrder)
	{
		F32ImageRef ret = m_derivesX[ a_nOrder ];

		if( NULL != ret )
			return ret;

		F32ImageRef src = GenImageX( a_nOrder - 1 );

		ret = F32Image::Create( src->GetSize(),
			src->GetNofChannels());


		HCV_CALL(

			cvSobel(
				src->GetIplImagePtr(),
				ret->GetIplImagePtr(),
				1,
				0,
				GetAprSiz()
					);
			   );

		float denom = -1;
		
		switch( GetAprSiz() )
		{
		case 3:
			denom = 4;
			break;
		case 5:
			denom = 48;
			break;
		case 7:
			denom = 576;
			break;
		default:
			return NULL;
		}


		ret =  GenMultByNumImg( ret, (float) 1 / denom );
		
		m_derivesX[ a_nOrder ] = ret;
		
		return ret;
	}


	F32ImageRef PNEdgeBuilder::GenImageY(int a_nOrder)
	{
		F32ImageRef ret = m_derivesY[ a_nOrder ];

		if( NULL != ret )
			return ret;

		F32ImageRef src = GenImageY( a_nOrder - 1 );

		ret = F32Image::Create( src->GetSize(),
			src->GetNofChannels());


		HCV_CALL(

			cvSobel(
				src->GetIplImagePtr(),
				ret->GetIplImagePtr(),
				0,
				1,
				GetAprSiz()
					);
			   );

		float denom = -1;
		
		switch( GetAprSiz() )
		{
		case 3:
			denom = 4;
			break;
		case 5:
			denom = 48;
			break;
		case 7:
			denom = 576;
			break;
		default:
			return NULL;
		}


		ret =  GenMultByNumImg( ret, (float) 1 / denom );
		
		m_derivesY[ a_nOrder ] = ret;
		
		return ret;
	}


}