#pragma once

#pragma inline_depth( 255 )

#include <stdlib.h>


#include <Lib\Cpp\Common\Debug.h>
#include <Lib\Cpp\Common\OffsetCalc_2D.h>


namespace Hcpl
{
	//MemAccessor_1D

	template<class T>
	class MemAccessor_2D : FRM_Object
	{
	public:

		MemAccessor_2D()
		{
			m_isLocked = false;
		}

		MemAccessor_2D(T * a_data, OffsetCalc_2D_Ref a_offsetCalc)
		{
			m_isLocked = false;
			Init(a_data, a_offsetCalc);
		}

		bool IsLocked()
		{
			return m_isLocked;
		}

		void LockForever()
		{
			m_isLocked = true;
		}

		//void CopyTo(MemAccessor_2D<T> a_pDest)
		//{
		//	*pDest = *this;
		//}

		void Init(T * a_data, OffsetCalc_2D_Ref a_offsetCalc)
		{
			m_data = a_data;
			m_offsetCalc = a_offsetCalc;
		}

		OffsetCalc_2D_Ref GetOffsetCalc()
		{
			return m_offsetCalc;
		}

		T * GetData()
		{
			return m_data;
		}

	protected:

		T * m_data;
		OffsetCalc_2D_Ref m_offsetCalc;
		bool m_isLocked;
	};
	
#define MemAccessor_2D_REF(T) Hcpl::ObjRef< Hcv::MemAccessor_2D< T > >

	//typedef Hcpl::ObjRef< OffsetCalc_2D > OffsetCalc_2D_Ref;
}