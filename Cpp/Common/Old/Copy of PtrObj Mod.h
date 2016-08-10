#pragma once



//#include <Lib\Cpp\Common\common.h>
#include <Lib\Cpp\Common\object.h>
#include <Lib\Cpp\Common\killers.h>


namespace Hcpl
{




	template<class T>
	class PtrRef : virtual ObjectFriend 
	{

	private:

		class PtrObj : FRM_Object
		{
		public:

			PtrObj()
			{
				Init();
			}

			PtrObj(void * a_ptr)
			{
				Init();
				m_ptr = a_ptr;		
			}

			PtrObj(ObjRef<PtrObj> & a_parent)
			{
				Init();
				m_parent = a_parent;		
			}

			virtual ~PtrObj(void)
			{
				if( NULL != m_pKillerFunc)
					m_pKillerFunc();
			}

		protected:
			void Init()
			{
				m_ptr = NULL;
				m_parent = NULL;
				m_pKillerFunc = NULL;
			}

		protected:
			void * m_ptr;
			ObjRef<PtrObj> m_parent;
			void (* m_pKillerFunc) ();
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
			*this = a_ptrRef.m_pObj;
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
			ReleaseObject(m_pObj);
			m_pObj = a_ptrObj;
			AddObjectRef(m_pObj);
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

	protected:
		PtrObj<T> * m_pObj;		

	};



}