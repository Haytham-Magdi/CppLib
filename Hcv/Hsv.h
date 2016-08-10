#pragma once


#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\PixelTypes.h>
#include <Lib\Hcv\error.h>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>



namespace Hcv
{
	using namespace Hcpl;

	class Hsv
	{
	public :

		static void GetHsvOfRgb(Uint8 a_nRed, Uint8 a_nGrn, Uint8 a_nBlu,
			Uint8 * a_pHue, Uint8 * a_pSat, Uint8 * a_pVal);

		static void CalcHsvDiff(S16ImageRef a_srcP, 
			Hcv::S16ImageRef a_srcAN, S16ImageRef a_dst);
	};


}