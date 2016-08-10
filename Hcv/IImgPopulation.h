#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Cpp\Math\mathLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>

namespace Hcv
{
	//using namespace Hcpl::Math;


	class IImgPopulation : FRM_Object
	{
	public:

		//virtual ~IImgPopulation() { }



		virtual void AddElm(int a_nIdx) = 0;

		virtual void SubElm(int a_nIdx) = 0;

		virtual void AddRange(int * a_idxBuf, int a_nofElms) = 0;

		virtual void AddElm_Thick(int a_nIdx) = 0;

		virtual void SubElm_Thick(int a_nIdx) = 0;

		virtual void AddRange_Thick(int * a_idxBuf, int a_nofElms) = 0;


		virtual int GetNofElms() = 0;

		virtual void IncBy( IImgPopulation * a_pop ) = 0;

		virtual void Copy( IImgPopulation * a_pop ) = 0;


		virtual void Reset() = 0;

		virtual float CalcVariance() = 0;

		virtual float CalcStandDiv() = 0;
	};


	typedef Hcpl::ObjRef< IImgPopulation > IImgPopulationRef;

}