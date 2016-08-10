#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\CircDiff.h>

namespace Hcv
{
	using namespace Hcpl;
	//using namespace Hcpl::Math;

	F32ImageRef CircDiff::GenResult( F32ImageRef a_src, int a_nAprSiz )
	{
		Hcpl_ASSERT( 3 == a_src->GetNofChannels() );
		Hcpl_ASSERT( 1 == a_nAprSiz % 2 );

		FixedVector< int > shifArrX;
		FixedVector< int > shifArrY;
		{
			const int nRadius = a_nAprSiz / 2;

			CircleContPathRef cp1 = new CircleContPath( 0, 180, nRadius );

			shifArrX.SetCapacity( nRadius * 15 );
			shifArrY.SetCapacity( nRadius * 15 );

			do
			{
				F32Point pnt1 = cp1->GetCurrent();

				shifArrX.PushBack( (int)pnt1.x );
				shifArrY.PushBack( (int)pnt1.y );

			}while( cp1->MoveNext() );
		}



		F32ImageRef ret;

		const int nMarg = a_nAprSiz / 2;

		CvSize srcSiz = a_src->GetSize();
		//CvSize retSiz = cvSize( srcSiz.width - 2 * nMarg, srcSiz.height);
		CvSize retSiz = cvSize( srcSiz.width, srcSiz.height);

		ret = F32Image::Create( retSiz, 1 );

		{
			const float maxDif = sqrt( 3.0 * Sqr( 255 ) );

			FillBorder( ret->GetAt(0), (float)maxDif, 
				nMarg, nMarg, nMarg, nMarg);
		}

		const int nBgnX = nMarg;
		const int nLimX = retSiz.width - nMarg;

		const int nBgnY = nMarg;
		const int nLimY = retSiz.height - nMarg;

		const int nShiftSize = shifArrX.GetSize();


		for(int y = nBgnY; y < nLimY; y++)
		{
			for(int x = nBgnX; x < nLimX; x++)
			{
				float maxSqrDif = 0;

				for( int i=0; i < nShiftSize; i++ )
				{
					const int shiftX = shifArrX[ i ];
					const int shiftY = shifArrY[ i ];

					F32ColorVal * pC1 = (F32ColorVal *) a_src->GetPixAt( 
						x + shiftX, y + shiftY );

					F32ColorVal * pC2 = (F32ColorVal *) a_src->GetPixAt( 
						x - shiftX, y - shiftY );

					const float sqrDif = 
						Sqr( pC1->val0 - pC2->val0 ) +
						Sqr( pC1->val1 - pC2->val1 ) +
						Sqr( pC1->val2 - pC2->val2 );

					if( sqrDif > maxSqrDif )
						maxSqrDif = sqrDif;
				}

				float val = sqrt( maxSqrDif );
				*ret->GetPixAt( x, y ) = val;
			}
		}

		return ret;
	}

}