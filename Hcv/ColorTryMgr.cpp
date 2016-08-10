#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\ColorTryMgr.h>

namespace Hcv
{
	using namespace Hcpl;
	//using namespace Hcpl::Math;


	void ColorTryMgr::Proceed()
	{
		F32ColorVal c1;
		//c1.AssignVal( 255, 0, 0 );
		c1.AssignVal( 255, 255, 0 );

		F32ColorVal c2;
		//c2.AssignVal( 0, 255, 0 );
		//c2.AssignVal( 0, 0, 255 );
		//c2.AssignVal( 0, 255, 255 );
		//c2.AssignVal( 255, 0, 255 );
		c2.AssignVal( 255, 255, 255 );

		float dist = F32ColorVal::Sub(
			c1.GetUnitVect(), c2.GetUnitVect() ).CalcMag();


	}



	float ColorTryMgr::CalcMag()
	{
		float val = this->CalcMagSqr();

		val = sqrt( val );

		return val;

		//return sqrt( (float)this->CalcMag() );
	}



	//float ColorTryMgr::Get_FarUnitDif()


}