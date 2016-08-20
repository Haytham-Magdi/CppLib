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

#define Image_MemAccessor_2D_REF(T_ImgElm, T_AccElm, V_SupposedNofChannels) ObjRef< Image_MemAccessor_2D< T_ImgElm, T_AccElm, V_SupposedNofChannels >>

	template<class T_ImgElm, class T_AccElm, int const V_SupposedNofChannels>
	class Image_MemAccessor_2D : FRM_Object
	{
	public:

		Image_MemAccessor_2D(IMAGE_REF(T_ImgElm) a_srcImg)
		{
			m_isLocked = false;
			Init(a_srcImg);
		}

		void Init(IMAGE_REF(T_ImgElm) a_srcImg)
		{
			if (m_isLocked)
				throw "m_isLocked";

			m_srcImg = a_srcImg;
			m_memAccessor = new MemAccessor_2D<T_AccElm>();
			Image_MemAccessor_2D::PrepareAccessorFromImage(m_srcImg, m_memAccessor);
		}

		static Image_MemAccessor_2D_REF(T_ImgElm, T_AccElm, V_SupposedNofChannels) SelfOrClone_Unlocked(
			Image_MemAccessor_2D_REF(T_ImgElm, T_AccElm, V_SupposedNofChannels) a_arg)
		{
			return a_arg->IsLocked() ? a_arg->CloneUnlocked() : a_arg;
		}

		static Image_MemAccessor_2D_REF(T_ImgElm, T_AccElm, V_SupposedNofChannels) SelfOrClone_Locked(
			Image_MemAccessor_2D_REF(T_ImgElm, T_AccElm, V_SupposedNofChannels) a_arg)
		{
			Image_MemAccessor_2D_REF(T_ImgElm, T_AccElm, V_SupposedNofChannels) ret = a_arg;

			if (!ret->IsLocked())
			{
				ret = ret->CloneUnlocked();
				ret->LockForever();
			}

			return ret;
		}

		bool IsLocked()
		{
			return m_isLocked;
		}

		void LockForever()
		{
			if (m_isLocked)
				return;

			m_isLocked = true;
		}

		Image_MemAccessor_2D_REF(T_ImgElm, T_AccElm, V_SupposedNofChannels) CloneNew()
		{
			Image_MemAccessor_2D_REF(T_ImgElm, T_AccElm, V_SupposedNofChannels) ret =
				new Image_MemAccessor_2D<T_ImgElm, T_AccElm, V_SupposedNofChannels>();

			ret->m_srcImg = m_srcImg;
			ret->m_memAccessor = this->m_memAccessor;

			return ret;
		}

		MemAccessor_2D_REF(T_AccElm) GetMemAccessor()
		{
			return m_memAccessor;
		}

		IMAGE_REF(T_ImgElm) GetSrcImg()
		{
			return m_srcImg;
		}

		void SetSrcImg(IMAGE_REF(T_ImgElm) a_srcImg)
		{
			if (m_isLocked)
				throw "m_isLocked";

			m_srcImg = a_srcImg;
		}

		static void PrepareAccessorFromImage(
			IMAGE_REF(T_ImgElm) a_srcImg,
			MemAccessor_2D_REF(T_AccElm) a_pAccessor)
		{
			Hcpl_ASSERT(a_srcImg->GetNofChannels() == V_SupposedNofChannels);

			OffsetCalc_2D_Ref calc = new OffsetCalc_2D(1, a_srcImg->GetSize().width, a_srcImg->GetSize().height);
			calc->LockForever();

			a_pAccessor->Init((T_AccElm *)a_srcImg->GetPixAt(0, 0), calc);
		}

	protected:

		MemAccessor_2D_REF(T_AccElm) m_memAccessor;
		IMAGE_REF(T_ImgElm) m_srcImg;

		bool m_isLocked;
	};

	//typedef Image_MemAccessor_2D< float, float, 3 > F32Image3C_FloatAcc;
	//typedef ObjRef< F32Image3C_FloatAcc > F32Image3C_FloatAcc_Ref;

	typedef Image_MemAccessor_2D< Float, F32ColorVal, 3 > F32Image3C_F32ColorVal_MemAccessor_2D;
	typedef ObjRef< F32Image3C_F32ColorVal_MemAccessor_2D > F32Image3C_F32ColorVal_MemAccessor_2D_Ref;

	//typedef Image_MemAccessor_2D< Hcpl::Float, float, 1 > F32Image_Float1C_MemAccessor_2D;
	typedef Image_MemAccessor_2D< Float, float, 1 > F32Image_Float1C_MemAccessor_2D;
	typedef ObjRef< F32Image_Float1C_MemAccessor_2D > F32Image_Float1C_MemAccessor_2D_Ref;

	typedef Image_MemAccessor_2D< Int32, int, 1 > S32Image1C_Int_MemAccessor_2D;
	typedef ObjRef< S32Image1C_Int_MemAccessor_2D > S32Image1C_Int_MemAccessor_2D_Ref;

}