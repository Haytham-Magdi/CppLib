#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\ImgDataElm_Simple.h>

namespace Hcv
{
	using namespace Hcpl;
	//using namespace Hcpl::Math;



	float ImgDataElm_Simple::CalcMag()
	{
		float val = this->CalcMagSqr();

		val = sqrt( val );

		return val;

		//return sqrt( (float)this->CalcMag() );
	}



	//float ImgDataElm_Simple::Get_FarUnitDif()


}