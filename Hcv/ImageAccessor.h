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

	//template<class T>
	//class MemAccessor_2D;

	template<class T_ImgElm, class T_AccElm, int const V_NofChannels>
	class ImageAccessor;


#define ImageAccessor_REF(T_ImgElm, T_AccElm, V_NofChannels) ObjRef< ImageAccessor< T_ImgElm, T_AccElm, V_NofChannels >>

	//typedef ImageAccessor< float, float, 3 > F32Image3C_FloatAcc;
	//typedef ObjRef< F32Image3C_FloatAcc > F32Image3C_FloatAcc_Ref;

	typedef ImageAccessor< Float, F32ColorVal, 3 > F32ImageAccessor3C;
	typedef ObjRef< F32ImageAccessor3C > F32ImageAccessor3C_Ref;

	//typedef ImageAccessor< Hcpl::Float, float, 1 > F32ImageAccessor1C;
	typedef ImageAccessor< Float, float, 1 > F32ImageAccessor1C;
	typedef ObjRef< F32ImageAccessor1C > F32ImageAccessor1C_Ref;

	typedef ImageAccessor< Int32, int, 1 > S32ImageAccessor1C;
	typedef ObjRef< S32ImageAccessor1C > S32ImageAccessor1C_Ref;



	template<class T_ImgElm, class T_AccElm, int const V_NofChannels>
	class ImageAccessor : FRM_Object
	{
	public:

		ImageAccessor(IMAGE_REF(T_ImgElm) a_srcImg)
		{
			m_isLocked = false;
			Init(a_srcImg);
		}

		ImageAccessor(OffsetCalc_2D_Ref a_offsetCalc)
		{
			m_isLocked = false;
			Init(a_offsetCalc);
		}

		OffsetCalc_2D_Ref GetOffsetCalc()
		{
			return GetMemAccessor()->GetOffsetCalc();
		}

		CvSize GetSize()
		{
			return cvSize(m_memAccessor->GetIndezSize_X(), m_memAccessor->GetIndezSize_Y());
		}

		bool IsLocked()
		{
			return m_isLocked;
		}

		void Lock()
		{
			m_isLocked = true;
		}

		void Unlock()
		{
			m_isLocked = false;
		}

		//ImageAccessor_REF(T_ImgElm, T_AccElm, V_NofChannels) CloneNew()
		//{
		//	ImageAccessor_REF(T_ImgElm, T_AccElm, V_NofChannels) ret =
		//		new ImageAccessor<T_ImgElm, T_AccElm, V_NofChannels>();

		//	ret->m_srcImg = m_srcImg;
		//	ret->m_memAccessor = this->m_memAccessor;

		//	return ret;
		//}

		ImageAccessor_REF(T_ImgElm, T_AccElm, V_NofChannels) CloneAccAndImage()
		{
			return Clone(true);
		}

		ImageAccessor_REF(T_ImgElm, T_AccElm, V_NofChannels) CloneAccessorOnly()
		{
			return Clone(false);
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
			Hcpl_ASSERT(a_srcImg->GetNofChannels() == V_NofChannels);

			OffsetCalc_2D_Ref offsetCalc = new OffsetCalc_2D(1, a_srcImg->GetSize().width, a_srcImg->GetSize().height);
			offsetCalc->Lock();

			a_pAccessor->Init((T_AccElm *)a_srcImg->GetDataPtr(), offsetCalc);
		}

		void SwitchXY()
		{
			m_memAccessor->Unlock();
			m_memAccessor->SwitchXY();
			m_memAccessor->Lock();
		}

	protected:

		ImageAccessor()
		{

		}

		void Init(IMAGE_REF(T_ImgElm) a_srcImg)
		{
			if (m_isLocked)
				throw "m_isLocked";

			m_srcImg = a_srcImg;
			m_memAccessor = new MemAccessor_2D<T_AccElm>();
			ImageAccessor::PrepareAccessorFromImage(m_srcImg, m_memAccessor);
		}

		void Init(OffsetCalc_2D_Ref a_offsetCalc)
		{
			if (m_isLocked)
				throw "m_isLocked";

			m_srcImg = F32Image::Create(
				cvSize(a_offsetCalc->GetOffsetCalc_X_Org()->GetIndexSize(),
				a_offsetCalc->GetOffsetCalc_Y_Org()->GetIndexSize()),
				//cvSize(a_offsetCalc->GetOffsetCalc_Y()->GetIndexSize(),
				//a_offsetCalc->GetOffsetCalc_X()->GetIndexSize()),
				V_NofChannels);

			m_memAccessor = new MemAccessor_2D<T_AccElm>(m_srcImg->GetDataPtr(), a_offsetCalc);
			//ImageAccessor::PrepareAccessorFromImage(m_srcImg, m_memAccessor);
		}

	protected:

		ImageAccessor_REF(T_ImgElm, T_AccElm, V_NofChannels) Clone(bool a_bCloneImage)
		{
			ImageAccessor_REF(T_ImgElm, T_AccElm, V_NofChannels) ret =
				new ImageAccessor<T_ImgElm, T_AccElm, V_NofChannels>();

			if (a_bCloneImage)
			{
				ret->m_srcImg = m_srcImg->Clone();
			}
			else
			{
				ret->m_srcImg = m_srcImg;
			}

			ret->m_memAccessor = m_memAccessor->Clone();
			ret->m_memAccessor->SetDataPtr((F32ColorVal *)ret->m_srcImg->GetDataPtr());
			ret->m_memAccessor->Lock();

			return ret;
		}

	protected:

		MemAccessor_2D_REF(T_AccElm) m_memAccessor;
		IMAGE_REF(T_ImgElm) m_srcImg;

		bool m_isLocked;
	};

}