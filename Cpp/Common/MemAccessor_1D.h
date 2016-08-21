#pragma once

#pragma inline_depth( 255 )

#include <stdlib.h>


#include <Lib\Cpp\Common\Debug.h>
#include <Lib\Cpp\Common\OffsetCalc_1D.h>
#include <Lib\Cpp\Common\PtrIterator.h>


namespace Hcpl
{
#define MemAccessor_1D_REF(T) Hcpl::ObjRef< MemAccessor_1D< T > >

	template<class T>
	class MemAccessor_1D : FRM_Object
	{
	public:

		MemAccessor_1D()
		{
			m_isLocked = false;
		}

		MemAccessor_1D(T * a_data, OffsetCalc_1D_Ref a_offsetCalc)
		{
			m_isLocked = false;
			Init(a_data, a_offsetCalc);
		}

		MemAccessor_1D_REF(T) Clone()
		{
			MemAccessor_1D_REF(T) pRet = new MemAccessor_1D<T>();

			pRet->m_data = m_data;

			pRet->m_offsetCalc = m_offsetCalc->Clone();
			pRet->m_offsetCalc->Lock();

			pRet->m_isLocked = false;
			return pRet;
		}

		PtrIterator<T> GenPtrIterator(int a_nAftBgn, int a_nBefEnd)
		{
			T * ptr_P2 = &(this->GetDataPtr())[m_offsetCalc->GetOffsetPart1()];
			
			T * ptr_Bgn = &ptr_P2[a_nAftBgn];
			T * ptr_Lim = &ptr_P2[m_offsetCalc->CalcPart2(m_offsetCalc->GetMaxNofSteps() - a_nBefEnd)];

			PtrIterator<T> ret(ptr_Bgn, ptr_Lim, m_offsetCalc->GetActualStepSize());
			//PtrIterator<T> ret(NULL, NULL, m_offsetCalc->GetActualStepSize());

			return ret;
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

		void Init(T * a_data, OffsetCalc_1D_Ref a_offsetCalc)
		{
			if (m_isLocked)
				throw "m_isLocked";

			m_data = a_data;

			m_offsetCalc = a_offsetCalc->Clone();
			m_offsetCalc->Lock();
		}

		OffsetCalc_1D_Ref GetOffsetCalc()
		{
			return m_offsetCalc;
		}

		T * GetDataPtr()
		{
			return m_data;
		}

		void SetDataPtr(T * a_data)
		{
			if (m_isLocked)
				throw "m_isLocked";

			m_data = a_data;
		}

	protected:

		T * m_data;
		OffsetCalc_1D_Ref m_offsetCalc;
		bool m_isLocked;
	};
	
}