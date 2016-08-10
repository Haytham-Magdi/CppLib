#pragma once


#include <Lib\Cpp\Common\debug.h>
#include <Lib\Cpp\Common\Object.h>



namespace Hcpl
{
	
	
	template<class T>
	class ObjRef : virtual ObjectFriend
	{
	public:
		ObjRef()
		{
			m_pObj = NULL;
		}

		ObjRef(T * a_pObj)
		{
			m_pObj = a_pObj;
			AddObjectRef(m_pObj);
		}

		ObjRef(const ObjRef & a_objRef) 
		{
			m_pObj = a_objRef.m_pObj;
			AddObjectRef(m_pObj);
		}

		~ObjRef()
		{
			ReleaseObject(m_pObj);
		}

		void operator = (const ObjRef<T> & a_objRef) 
		{
			*this = a_objRef.m_pObj;
		}

		void operator = (T * a_pObj) 
		{
			AddObjectRef(a_pObj);
			ReleaseObject(m_pObj);
			m_pObj = a_pObj;            
		}


		T * operator -> (void)
		{
			return m_pObj;
		}

		operator T * (void)
		{
			return m_pObj;
		}

	protected:
		T * m_pObj;		
	};
	
}
