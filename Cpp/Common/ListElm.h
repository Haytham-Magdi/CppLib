#pragma once

#pragma inline_depth( 255 )

#include <stdlib.h>


#include <Lib\Cpp\Common\Debug.h>
//#include <Lib\Cpp\Common\ListQue.h>


namespace Hcpl
{
	template<class T>
	class ListElm : FRM_Object
	{
	protected:

	public:

		ListElm()
		{
			m_pNext = NULL;
			m_pPrev = NULL;
		}

	public:

		ListElm< T > * GetNext()
		{
			return m_pNext;
		}

		ListElm< T > * GetPrev()
		{
			return m_pNext;
		}

		ListElm< T > * GetFarNext()
		{
			ListElm< T > * pNext = m_pNext;
			ListElm< T > * pOldNext = pNext;

			while( NULL != pNext )
			{
				pOldNext = pNext;

				pNext = pNext->m_pNext;
			}

			return pOldNext;
		}

		ListElm< T > * GetFarPrev()
		{
			ListElm< T > * pPrev = m_pPrev;
			ListElm< T > * pOldPrev = pPrev;

			while( NULL != pPrev )
			{
				pOldPrev = pPrev;

				pPrev = pPrev->m_pPrev;
			}

			return pOldPrev;
		}

		void AddNext( ListElm< T > * a_pElm )
		{
			Hcpl_ASSERT( NULL == m_pNext );
			Hcpl_ASSERT( NULL == a_pElm->m_pPrev );

			m_pNext = a_pElm;
			a_pElm->m_pPrev = this;
		}

		void AddPrev( ListElm< T > * a_pElm )
		{
			Hcpl_ASSERT( NULL == m_pPrev );
			Hcpl_ASSERT( NULL == a_pElm->m_pNext );

			m_pPrev = a_pElm;
			a_pElm->m_pNext = this;
		}

/*
		void InsertNext( ListElm< T > * a_pElm )
		{
			erwerwer
				BreakNext()
					BreakPrev()

			Hcpl_ASSERT( NULL == m_pNext );
			Hcpl_ASSERT( NULL == a_pElm->m_pPrev );

			m_pNext = a_pElm;
			a_pElm->m_pPrev = this;
		}
*/

		void BreakNext()
		{
			if( NULL != m_pNext )
			{
				m_pNext->m_pPrev = NULL;
				m_pNext = NULL;
			}
		}

		void BreakPrev()
		{
			if( NULL != m_pPrev )
			{
				m_pPrev->m_pNext = NULL;
				m_pPrev = NULL;
			}
		}

		void ConnectNextToPrev()
		{
			ListElm< T > * pNext = m_pNext;
			ListElm< T > * pPrev = m_pPrev;

			BreakNext();
			BreakPrev();

			if( NULL != pNext &&
				NULL != pPrev )
			{
				pPrev->AddNext( pNext );
			}
		}


		T Content;

	protected:

		ListElm< T > * m_pNext;
		ListElm< T > * m_pPrev;
	};



}