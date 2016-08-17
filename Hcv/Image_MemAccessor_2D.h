#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <Lib\Hcv\funcs1.h>
#include <vector>
#include <Lib\Hcv\Image.h>



namespace Hcv
{
	using namespace Hcpl;

	#define Image_MemAccessor_2D_REF(T_ImgElm, T_AccElm, V_SupposedNofChannels) Hcpl::ObjRef< Image_MemAccessor_2D< T_ImgElm, T_AccElm, V_SupposedNofChannels >>

	template<class T_ImgElm, class T_AccElm, int const V_SupposedNofChannels>
	class Image_MemAccessor_2D : FRM_Object
	{
	public:

		Image_MemAccessor_2D(IMAGE_REF(T_ImgElm) a_srcImg)
		{
			m_srcImg = a_srcImg;
			Image_MemAccessor_2D::PrepareAccessorFromImage( m_srcImg, &m_memAccessor);
		}

		void Init(IMAGE_REF(T_ImgElm) a_srcImg)
		{
			m_srcImg = a_srcImg;
			Image_MemAccessor_2D::PrepareAccessorFromImage(m_srcImg, &m_memAccessor);
		}

		//void CopyTo(Image_MemAccessor_2D<T_ImgElm, T_AccElm, V_SupposedNofChannels> * pDest)
		//{
		//	*pDest = *this;
		//}

		Image_MemAccessor_2D_REF(T_ImgElm, T_AccElm, V_SupposedNofChannels) CloneNew()
		{
			Image_MemAccessor_2D_REF(T_ImgElm, T_AccElm, V_SupposedNofChannels) ret =
				new Image_MemAccessor_2D<T_ImgElm, T_AccElm, V_SupposedNofChannels>();

			ret->m_srcImg = m_srcImg->CloneNew();
			ret->m_memAccessor->Init(ret->m_srcImg);
			m_memAccessor->GetOffsetCalc().CopyTo(&ret->m_memAccessor->GetOffsetCalc());
			
			//m_memAccessor.CopyTo(&ret->m_memAccessor);

			return ret;
			//throw "Not Implemented";
		}

		MemAccessor_2D<T_AccElm> &  GetMemAccessor()
		{
			return m_memAccessor;
		}

		IMAGE_REF(T_ImgElm) GetSrcImg()
		{
			return m_srcImg;
		}

		static void PrepareAccessorFromImage( 
			Image<T_ImgElm> * a_srcImg,
			MemAccessor_2D<T_AccElm> * a_pAccessor)
		{
			Hcpl_ASSERT(a_srcImg->GetNofChannels() == V_SupposedNofChannels);

			a_pAccessor->Init((T_AccElm *)a_srcImg->GetPixAt(0, 0),
				1, a_srcImg->GetSize().width, a_srcImg->GetSize().height);
		}

	//protected:

	protected:

		MemAccessor_2D<T_AccElm> m_memAccessor;
		IMAGE_REF(T_ImgElm) m_srcImg;
	};

	typedef Image_MemAccessor_2D< float, float, 3 > F32Image3C_Float_MemAccessor_2D;
	typedef Hcpl::ObjRef< F32Image3C_Float_MemAccessor_2D > F32Image3C_Float_MemAccessor_2DRef;

	typedef Image_MemAccessor_2D< Hcpl::Float, F32ColorVal, 3 > F32Image3C_F32ColorVal_MemAccessor_2D;
	typedef Hcpl::ObjRef< F32Image3C_F32ColorVal_MemAccessor_2D > F32Image3C_F32ColorVal_MemAccessor_2DRef;

	typedef Image_MemAccessor_2D< Hcpl::Float, float, 1 > F32Image_Float1C_MemAccessor_2D;
	typedef Hcpl::ObjRef< F32Image_Float1C_MemAccessor_2D > F32Image_Float1C_MemAccessor_2DRef;
	
	typedef Image_MemAccessor_2D< Hcpl::Int32, int, 1 > S32Image1C_Int_MemAccessor_2D;
	typedef Hcpl::ObjRef< S32Image1C_Int_MemAccessor_2D > S32Image1C_Int_MemAccessor_2DRef;

}