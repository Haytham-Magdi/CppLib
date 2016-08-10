#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\ImageItrMgr.h>

namespace Hcv
{
	using namespace Hcpl;
	//using namespace Hcpl::Math;


	void ImageItrMgr::Init( CvSize a_imgSiz,
		FixedVector< LineLimit > & a_lineLimit_H_Arr, 
		FixedVector< LineLimit > & a_lineLimit_V_Arr )
	{
		m_imgSiz = a_imgSiz;

		//PrepareProvArr();
//	}


//	void ImageItrMgr::PrepareProvArr()
//	{
		m_lineItrProv_Arr.SetSize( 4 );

		//	H Forward
		{
			FixedVector< LineLimit > & rLineLimitArr = a_lineLimit_H_Arr;

			ImageLineItrProvider & rProv = m_lineItrProv_Arr[ 0 ];
			rProv.Init( m_imgSiz, ImageLineItrProvider::ProvType_H );

			FixedVector< ImageLineItr > & rLineItrArr = rProv.GetLineItrArr();
			rLineItrArr.SetSize( rLineLimitArr.GetSize() );

			Hcpl_ASSERT( rLineItrArr.GetSize() == rLineLimitArr.GetSize() );

			for( int i=0; i < rLineItrArr.GetSize(); i++ )
			{
				ImageLineItr & rLineItr = rLineItrArr[ i ];
				LineLimit & rLineLimit = rLineLimitArr[ i ];

				int y = i;

				int nFarBgn = y * m_imgSiz.width;
				int nFarEnd = ( y + 1 ) * m_imgSiz.width - 1;

				rLineItr.Init( i, rLineLimit.nBgnIdx, 
					rLineLimit.nEndIdx, 1, nFarBgn, nFarEnd );
			}
		}

		//	V Forward
		{
			FixedVector< LineLimit > & rLineLimitArr = a_lineLimit_V_Arr;

			ImageLineItrProvider & rProv = m_lineItrProv_Arr[ 1 ];
			rProv.Init( m_imgSiz, ImageLineItrProvider::ProvType_V );

			FixedVector< ImageLineItr > & rLineItrArr = rProv.GetLineItrArr();
			rLineItrArr.SetSize( rLineLimitArr.GetSize() );

			Hcpl_ASSERT( rLineItrArr.GetSize() == rLineLimitArr.GetSize() );

			IndexCalc2D idxCalc( m_imgSiz.width, m_imgSiz.height );

			for( int i=0; i < rLineItrArr.GetSize(); i++ )
			{
				ImageLineItr & rLineItr = rLineItrArr[ i ];
				LineLimit & rLineLimit = rLineLimitArr[ i ];

				int x = i;

				int nFarBgn = x;
				int nFarEnd = idxCalc.Calc( x, m_imgSiz.height - 1 );

				rLineItr.Init( i, rLineLimit.nBgnIdx, 
					rLineLimit.nEndIdx, m_imgSiz.width, 
					nFarBgn, nFarEnd );
			}
		}

		//	H Backward
		{
			FixedVector< LineLimit > & rLineLimitArr = a_lineLimit_H_Arr;

			ImageLineItrProvider & rProv = m_lineItrProv_Arr[ 2 ];
			rProv.Init( m_imgSiz, ImageLineItrProvider::ProvType_H );

			FixedVector< ImageLineItr > & rLineItrArr = rProv.GetLineItrArr();
			rLineItrArr.SetSize( rLineLimitArr.GetSize() );

			Hcpl_ASSERT( rLineItrArr.GetSize() == rLineLimitArr.GetSize() );

			for( int i=0; i < rLineItrArr.GetSize(); i++ )
			{
				ImageLineItr & rLineItr = rLineItrArr[ i ];
				LineLimit & rLineLimit = rLineLimitArr[ i ];

				int y = i;

				int nFarBgn = y * m_imgSiz.width;
				int nFarEnd = ( y + 1 ) * m_imgSiz.width - 1;

				rLineItr.Init( i, rLineLimit.nEndIdx, 
					rLineLimit.nBgnIdx, - 1, nFarEnd, nFarBgn );
			}
		}

		//	V Backward
		{
			FixedVector< LineLimit > & rLineLimitArr = a_lineLimit_V_Arr;

			ImageLineItrProvider & rProv = m_lineItrProv_Arr[ 3 ];
			rProv.Init( m_imgSiz, ImageLineItrProvider::ProvType_V );

			FixedVector< ImageLineItr > & rLineItrArr = rProv.GetLineItrArr();
			rLineItrArr.SetSize( rLineLimitArr.GetSize() );

			Hcpl_ASSERT( rLineItrArr.GetSize() == rLineLimitArr.GetSize() );

			IndexCalc2D idxCalc( m_imgSiz.width, m_imgSiz.height );

			for( int i=0; i < rLineItrArr.GetSize(); i++ )
			{
				ImageLineItr & rLineItr = rLineItrArr[ i ];
				LineLimit & rLineLimit = rLineLimitArr[ i ];

				int x = i;

				int nFarBgn = x;
				int nFarEnd = idxCalc.Calc( x, m_imgSiz.height - 1 );

				rLineItr.Init( i, rLineLimit.nEndIdx, 
					rLineLimit.nBgnIdx, - m_imgSiz.width,
					nFarEnd, nFarBgn );
			}
		}


	}




}