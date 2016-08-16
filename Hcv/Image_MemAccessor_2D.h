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

//#include <Lib\cpp\MemAccessor_2D.h>



namespace Hcv
{
	using namespace Hcpl;


	template<class T_ImgElm, class T_AccElm, int const V_SupposedNofChannels>
	class Image_MemAccessor_2D : FRM_Object
	{
	public:

		Image_MemAccessor_2D(IMAGE_REF(T_ImgElm) a_srcImg)
		{
			m_srcImg = a_srcImg;
			Image_MemAccessor_2D::PrepareAccessorFromImage( m_srcImg, &m_memAccessor);
		}

		MemAccessor_2D<T_AccElm> &  GetMemAccessor()
		{
			return m_memAccessor;
		}

		static void PrepareAccessorFromImage( 
			Hcv::Image< T_ImgElm > * a_srcImg,
			MemAccessor_2D<T_AccElm> * a_pAccessor)
		{
			Hcpl_ASSERT(a_srcImg->GetNofChannels() == V_SupposedNofChannels);

			a_pAccessor->Init((T_AccElm *)a_srcImg->GetPixAt(0, 0),
				1, a_srcImg->GetSize().width, a_srcImg->GetSize().height);
		}


	protected:

		MemAccessor_2D<T_AccElm> m_memAccessor;
		IMAGE_REF(T_ImgElm) m_srcImg;

		//static void PrepareF32ColorValAccessor(F32ImageRef a_srcImg, MemAccessor_2D<F32ColorVal> * a_pAccessor)
		//{
		//	Hcpl_ASSERT(3 == a_srcImg->GetNofChannels());

		//	a_pAccessor->Init((F32ColorVal *)a_srcImg->GetPixAt(0, 0),
		//		1, a_srcImg->GetSize().width, a_srcImg->GetSize().height);
		//}

		//static void PrepareFloatAccessor(F32ImageRef a_srcImg, MemAccessor_2D<float> * a_pAccessor)
		//{
		//	Hcpl_ASSERT(1 == a_srcImg->GetNofChannels());

		//	a_pAccessor->Init((float *)a_srcImg->GetPixAt(0, 0),
		//		1, a_srcImg->GetSize().width, a_srcImg->GetSize().height);
		//}

	};

	//typedef Hcv::Image_MemAccessor_2D<float, float> F32Image3C_F32ColorVal_MemAccessor_2D;
	//typedef Hcv::Image_MemAccessor_2D< float, float, 3 > F32Image3C_F32ColorVal_MemAccessor_2D;


	//typedef Hcv::Image_MemAccessor_2D< Hcpl::Float, F32ColorVal, 3 > F32Image3C_F32ColorVal_MemAccessor_2D;
	//typedef Hcv::Image_MemAccessor_2D< Hcpl::Float, float, 1 > F32Image_Float1C_MemAccessor_2D;

	//typedef Hcv::Image_MemAccessor_2D< Hcpl::Int32, int, 1 > S32Image1C_Int_MemAccessor_2D;


}