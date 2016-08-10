#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\PixelTypes.h>
#include <Lib\Hcv\error.h>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\InitLib.h>


namespace Hcv
{
	bool g_bDone = false;

	void InitLib(void)
	{
		if(g_bDone)
			return;

/*		std::vector<char> * pCv;

		for(int i=0; i<100; i++)
		{
			pCv = new std::vector<char>();
			pCv->resize(1000000);
		}*/

/*		std::vector<char> cv;
		cv.resize(100000000);
		cv.*/

/*		cv.resize(100000000);
		cv.resize(100000000);
		cv.clear();
		cv.resize(100000000);*/

		g_bDone = true;
	}


}