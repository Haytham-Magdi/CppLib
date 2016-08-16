#pragma once

#include <Lib\Cpp\Common\commonLib.h>
//#include <Lib\Cpp\Math\mathLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <Lib\Hcv\funcs1.h>
#include <vector>
//#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
//#include <queue>      


namespace Hcv
{
	using namespace Hcpl;
	//using namespace std;

	template<class T_ImgElm, class T_AccElm>
	static void PrepareAccessorFromImage(
		//Hcpl::ObjRef< Hcv::Image< T_ImgElm >> a_srcImg,
		Hcv::Image< T_ImgElm > * a_srcImg,
		int a_supposedNofChannels, MemAccessor_2D<T_AccElm> * a_pAccessor)
	{
		Hcpl_ASSERT(a_srcImg->GetNofChannels() == a_supposedNofChannels);

		a_pAccessor->Init((T_AccElm *)a_srcImg->GetPixAt(0, 0),
			1, a_srcImg->GetSize().width, a_srcImg->GetSize().height);
	}



	class Image_MemAccessor_2D : FRM_Object
	{
	public:

		 
		static void PrepareF32ColorValAccessor(F32ImageRef a_srcImg, MemAccessor_2D<F32ColorVal> * a_pAccessor)
		{
			Hcpl_ASSERT(3 == a_srcImg->GetNofChannels());

			a_pAccessor->Init((F32ColorVal *)a_srcImg->GetPixAt(0, 0),
				1, a_srcImg->GetSize().width, a_srcImg->GetSize().height);
		}

		static void PrepareFloatAccessor(F32ImageRef a_srcImg, MemAccessor_2D<float> * a_pAccessor)
		{
			Hcpl_ASSERT(1 == a_srcImg->GetNofChannels());

			a_pAccessor->Init((float *)a_srcImg->GetPixAt(0, 0),
				1, a_srcImg->GetSize().width, a_srcImg->GetSize().height);
		}

	};
}