#pragma once

#pragma inline_depth( 255 )

#include <stdlib.h>


#include <Lib\Cpp\Common\Debug.h>
#include <Lib\Cpp\Common\OffsetCalc_1D.h>


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

		MemAccessor_1D(T * a_data, OffsetCalc_1D_Ref a_offsetCalc_Locked)
		{
			m_isLocked = false;
			Init(a_data, a_offsetCalc);
		}

		MemAccessor_1D<T> * CloneUnlocked()
		{
			MemAccessor_1D<T> * pRet = new MemAccessor_1D<T>();

			pRet->m_data = m_data;
			pRet->m_offsetCalc = m_offsetCalc;

			pRet->m_isLocked = false;
			return pRet;
		}

		static MemAccessor_1D_REF(T) SelfOrClone_Unlocked(MemAccessor_1D_REF(T) a_arg)
		{
			throw "Not working!";
			return a_arg->IsLocked() ? a_arg->CloneUnlocked() : a_arg;
		}

		static MemAccessor_1D_REF(T) SelfOrClone_Locked(MemAccessor_1D_REF(T) a_arg)
		{
			MemAccessor_1D_REF(T) ret = a_arg;

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

		void Init(T * a_data, OffsetCalc_1D_Ref a_offsetCalc_Locked)
		{
			if (m_isLocked)
				throw "m_isLocked";

			if (!a_offsetCalc_Locked->IsLocked())
				throw "!a_offsetCalc_Locked->IsLocked()";

			m_data = a_data;

			m_offsetCalc = a_offsetCalc_Locked;
			m_offsetCalc->LockForever();
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