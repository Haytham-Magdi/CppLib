#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Cpp\Math\mathLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>

namespace Hcv
{
	//using namespace Hcpl::Math;


	class SlideMgr : FRM_Object
	{
	public:
		class SlideChainElm
		{
		public:
			int index;

			float val;
			float avgVal;

			SlideChainElm * pHead;
			SlideChainElm * pNext;
			SlideChainElm * pPrev;
			SlideChainElm * pTail;

			bool bHasNext;

			int nLastAction;
		};



	public:

		SlideMgr( F32ImageRef a_src, int a_nAprSiz , 
			FixedVector< SlideChainElm > * a_pSlideChainArr = NULL, 
			FixedVector< bool > * a_pIsRootArr = NULL );

		FixedVector< SlideChainElm > & GetSlideChainArr()
		{
			return *m_pSlideChainArr;
		}

		FixedVector< SlideChainElm * > & GetChainHeadPtrArr()
		{
			return m_chainHeadPtrArr;
		}

		

		FixedVector< bool > & GetIsRootArr()
		{
			return *m_pIsRootArr;
		}


		F32Point GetPosOfElm( SlideChainElm & a_elm )
		{
			return GetPosOfIndex( a_elm.index, this->m_src->GetSize().width );
		}


		static F32Point GetPosOfIndex( int a_nIndex, int a_nSizY )
		{
			F32Point pos;

			pos.y = a_nIndex / a_nSizY;
			pos.x = a_nIndex % a_nSizY;

			return pos;
		}


	protected:
		
		void InitSlideChainArr();
		void InitIsRootArr();
		

		void PrepareSlideChainArr();

	protected:

		F32ImageRef m_src;
		int m_nAprSiz;

		FixedVector< SlideChainElm > * m_pSlideChainArr; 
		FixedVector< SlideChainElm > m_slideChainArrLc; 

		FixedVector< SlideChainElm * > m_chainHeadPtrArr; 

		FixedVector< bool > * m_pIsRootArr;
		FixedVector< bool > m_isRootArrLc;
	};


	typedef Hcpl::ObjRef< SlideMgr > SlideMgrRef;

}