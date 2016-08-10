#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\Signal1D.h>

#include <Lib\Hcv\DeltaSignal.h>

namespace Hcv
{
	using namespace Hcpl;
	//using namespace Hcpl::Math;

	Signal1DRef DeltaSignal::CreateAt( int a_nPos )
	{
		Signal1DBuilder sb1( 3, a_nPos - 1 );

		sb1.AddValue( 0 );
		sb1.AddValue( 100 );
		sb1.AddValue( 0 );
		
		return sb1.GetResult();
	}





}