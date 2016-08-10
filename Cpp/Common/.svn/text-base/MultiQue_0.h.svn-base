#pragma once

#pragma inline_depth( 255 )

#include <stdlib.h>


#include <Lib\Cpp\Common\Debug.h>
#include <Lib\Cpp\Common\FixedVector.h>



namespace Hcpl
{



	template<class T>
	class MultiQue : FRM_Object
	{
	protected:
		typedef struct _QueState
		{
			T * dataPtr;
			int nBgn;
			int nSize;
			int nMaxSiz;
		}QueState;

	public:

		void Init(int a_nofQues, int a_nTotalSiz)
		{
			m_qsVect.SetSize(a_nofQues);
			m_dataVect.SetSize(a_nTotalSiz);
			m_nInitIndex = 0;
		}

		void InitQue(int a_nIndex, int a_nSiz)
		{
			Hcpl_ASSERT(a_nIndex < m_qsVect.GetSize());
			Hcpl_ASSERT(a_nIndex == m_nInitIndex);

			QueState * pQs = &m_qsVect[a_nIndex];

			pQs->dataPtr = &m_dataVect[m_nSizCnt];
			pQs->nBgn = 0;
			pQs->nSize = 0;
			pQs->nMaxSiz = a_nSiz;

			m_nSizCnt += a_nSiz;
			m_nInitIndex++;

			Hcpl_ASSERT(m_nSizCnt <= m_dataVect.GetSize());
		}

		void PushVal(int a_nIndex, T a_val)
		{
			Hcpl_ASSERT(a_nIndex < m_qsVect.GetSize());

			QueState * pQs = m_qsVect[a_nIndex];
			Hcpl_ASSERT(pQs->nSize < pQs->nMaxSiz);

			pQs->dataPtr[ (pQs->nBgn + pQs->nSize) % pQs->nMaxSiz ] = a_val;
			pQs->nSize++;
		}

		T PopVal(int a_nIndex, T a_val)
		{
			Hcpl_ASSERT(a_nIndex < m_qsVect.GetSize());

			QueState * pQs = m_qsVect[a_nIndex];
			Hcpl_ASSERT(pQs->nSize > 0);

			int nOldBgn = pQs->nBgn;
			DoPop(pQs);

			return pQs->dataPtr[ nOldBgn ];
		}

		void Pop(int a_nIndex)
		{
			Hcpl_ASSERT(a_nIndex < m_qsVect.GetSize());

			QueState * pQs = m_qsVect[a_nIndex];
			Hcpl_ASSERT(pQs->nSize > 0);

			DoPop(pQs);
		}

		T GetVal(int a_nIndex)
		{
			Hcpl_ASSERT(a_nIndex < m_qsVect.GetSize());

			QueState * pQs = m_qsVect[a_nIndex];
			Hcpl_ASSERT(pQs->nSize > 0);

			return pQs->dataPtr[ pQs->nBgn ];
		}

		T * GetValPtr(int a_nIndex)
		{
			Hcpl_ASSERT(a_nIndex < m_qsVect.GetSize());

			QueState * pQs = m_qsVect[a_nIndex];
			Hcpl_ASSERT(pQs->nSize > 0);

			return &pQs->dataPtr[ pQs->nBgn ];
		}

	protected:

		void DoPop(QueState * a_pQs)
		{
			a_pQs->nBgn = (pQs->nBgn + 1) % pQs->nMaxSiz;
			a_pQs->nSize--;
		}

		//std::vector<T> m_dataVect;
		//std::vector<QueState> m_qsVect;		

		FixedVector<T> m_dataVect;
		FixedVector<QueState> m_qsVect;		

		int m_nInitIndex;
		int m_nSizCnt;
	};


}