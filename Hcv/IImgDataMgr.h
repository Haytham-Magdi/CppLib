#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Cpp\Math\mathLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>

#include <Lib\Hcv\IImgPopulation.h>

namespace Hcv
{
	//using namespace Hcpl::Math;


	class IImgDataMgr : FRM_Object
	{
	public:

		//virtual ~IImgDataMgr() = 0;

		virtual float CalcDif(int a_nIdx_1, int a_nIdx_2) = 0;

		virtual float Calc_UnitDif(int a_nIdx_1, int a_nIdx_2) = 0;

		virtual CvSize GetSize() = 0;

		virtual int GetDiameter() = 0;

		virtual F32ImageRef GetSrcImage() = 0;

		virtual F32ImageRef GetStandDiv_Mag_Image() = 0;

		virtual F32ImageRef Get_Mag_Image() = 0;

		virtual bool VerifyEdge( int a_nIdx_R1, int a_nIdx_Med1, 
			int a_nIdx_Edge, int a_nIdx_Med2, int a_nIdx_R2 ) = 0;

		//virtual IImgPopulationRef CreatePopulation() = 0;
		virtual IImgPopulationRef CreatePopulation() = 0;
	};


	typedef Hcpl::ObjRef< IImgDataMgr > IImgDataMgrRef;

}