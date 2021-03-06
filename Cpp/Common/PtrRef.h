#pragma once



//#include <Lib\Cpp\Common\common.h>
#include <Lib\Cpp\Common\object.h>
#include <Lib\Cpp\Common\killers.h>


namespace Hcpl
{
	class Dummy {};		
	
	
	
	template<class T>
		class PtrRef : virtual ObjectFriend 
	{
		
		
		
		
	public:
		
		
		template<class T>
			class PtrObj : FRM_IObject_IFC
		{
	public:
		
		PtrObj(T * a_ptr)
		{
			m_ptr = a_ptr;		
			m_nRefCnt = 0;
		}
		
		virtual ~PtrObj(void)
		{
		}
		
		
		
		T * GetPtr(void)
		{
			return m_ptr;
		}
		
		
	private:
		virtual void AddRef(void)
		{
			m_nRefCnt++;
		}
		
		virtual void Release(void)
		{
			m_nRefCnt--;
			Hcpl_ASSERT(m_nRefCnt >= 0);
			
			if(0 == m_nRefCnt)
			{
				KillObj(m_ptr);
				delete this;				
			}
		}
		
		
	private:
		int m_nRefCnt;
		T * m_ptr;
		};
		
		
		
		
		
		
		
		///////////////////////////////////////////
		
		
		
		
		
		
	public:
		PtrRef()
		{
			m_pObj = NULL;
		}
		
		PtrRef(T * a_ptr)
		{
			m_pObj = new PtrObj<T>(a_ptr);
			AddObjectRef(m_pObj);
		}
		
		PtrRef(const PtrRef & a_ptrRef) 
		{
			m_pObj = a_ptrRef.m_pObj;
			AddObjectRef(m_pObj);
		}
		
		~PtrRef()
		{
			ReleaseObject(m_pObj);
		}
		
		void operator = (const PtrRef<T> & a_ptrRef) 
		{
			*this = a_ptrRef.m_pObj;
		}
		
		void operator = (PtrObj<T> * a_ptrObj) 
		{
			AddObjectRef(a_ptrObj);
			ReleaseObject(m_pObj);
			m_pObj = a_ptrObj;
		}

		void operator = (Dummy * a_pDmy) 
		{
			Hcpl_ASSERT(false);
		}

		void operator = (T * a_ptr) 
		{
			ReleaseObject(m_pObj);
			m_pObj = new PtrObj<T>(a_ptr);
			AddObjectRef(m_pObj);
		}
		
		
		T * operator -> (void)
		{
			return m_pObj->GetPtr();
		}

		operator T * (void)
		{
			return m_pObj->GetPtr();
		}
		
		operator Dummy * (void)
		{
			return NULL;
		}



	protected:
		PtrObj<T> * m_pObj;		
		
	};
	
	
	template<class T2, class T1>
		inline PtrRef<T2> GenRef(PtrRef<T1> a_t1)
	{
		PtrRef<T2> t2 = (T2 *) a_t1;
		return t2;
	}

	
}