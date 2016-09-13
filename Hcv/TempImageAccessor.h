#pragma once

#include <Lib\Cpp\Common\commonLib.h>
//#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
//#include <Lib\Hcv\funcs1.h>
#include <vector>
//#include <Lib\Hcv\Image.h>

//MemAccessor_2D


namespace Hcv
{
	using namespace Hcpl;

	template<class T>
	class TempImageAccessor;


#define TempImageAccessor_REF(T) ObjRef< TempImageAccessor< T >>

	template<class T>
	class TempImageAccessor : FRM_Object
	{
	public:

		//TempImageAccessor(MemAccessor_2D_REF(T) a_memAccessor)
		TempImageAccessor(OffsetCalc_2D_Ref a_offsetCalc)
		{
			//int nSize_X = a_memAccessor->GetOffsetCalc()->GetOffsetCalc_X()->GetIndexSize();
			//int nSize_Y = a_memAccessor->GetOffsetCalc()->GetOffsetCalc_Y()->GetIndexSize();
			int nSize_X = a_offsetCalc->GetOffsetCalc_X_Org()->GetIndexSize();
			int nSize_Y = a_offsetCalc->GetOffsetCalc_Y_Org()->GetIndexSize();
			int nSize_1D = nSize_X * nSize_Y;

			m_allocVect = new FixedVector<T>();
			m_allocVect->SetSize(nSize_1D);

			OffsetCalc_2D_Ref offsetCalc = new OffsetCalc_2D(1, nSize_X, nSize_Y);
			if (a_offsetCalc->GetOffsetCalc_X_Org()->GetIndexSize() != a_offsetCalc->GetOffsetCalc_X()->GetIndexSize())
			{
				offsetCalc->SwitchXY();
			}

			m_memAccessor = new MemAccessor_2D<T>(m_allocVect->GetHeadPtr(), offsetCalc);
			m_memAccessor->Lock();
		}

		MemAccessor_2D_REF(T) GetMemAccessor()
		{
			return m_memAccessor;
		}

		void SwitchXY()
		{
			m_memAccessor->Unlock();
			m_memAccessor->SwitchXY();
			m_memAccessor->Lock();
		}

		TempImageAccessor_REF(T) CloneAccAndData()
		{
			return Clone(true);
		}

		TempImageAccessor_REF(T) CloneAccessorOnly()
		{
			return Clone(false);
		}

	protected:

		TempImageAccessor_REF(T) Clone(bool a_bCloneData)
		{
			TempImageAccessor_REF(T) ret = new TempImageAccessor<T>();

			if (a_bCloneData)
			{
				ret->m_allocVect = new FixedVector<T>();
				ret->m_allocVect->SetSize(m_allocVect->GetSize());

				memcpy(ret->m_allocVect->GetHeadPtr(), m_allocVect->GetHeadPtr(), m_allocVect->GetSize() * sizeof(T));
			}
			else
			{
				ret->m_allocVect = m_allocVect;
			}

			ret->m_memAccessor = m_memAccessor->Clone();
			ret->m_memAccessor->SetDataPtr(ret->m_allocVect->GetHeadPtr());
			ret->m_memAccessor->Lock();

			return ret;
		}


	protected:

		TempImageAccessor()
		{

		}

	protected:

		MemAccessor_2D_REF(T) m_memAccessor;
		FixedVector_REF(T) m_allocVect;
	};

}