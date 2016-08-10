#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\RegionSegmentor.h>

namespace Hcv
{
	using namespace Hcpl;
	using namespace Hcpl::Math;


	RegionSegmentor::RegionSegmentor(S16ImageRef a_src, float a_localRatio) : 
	m_nofLinks(0), m_rgnIndexCalc(a_src->GetWidth(), a_src->GetHeight()) 
	{
		m_src = a_src;

		m_localRatio = a_localRatio;
		m_meanRatio = 0.99 - a_localRatio;

		Hcpl_ASSERT(a_localRatio < 1);
		Hcpl_ASSERT((a_localRatio + m_meanRatio) <= 1);


		InitMaxDif();
		m_difQues.Init( GetMaxDif() + 1 );

		InitRgnInfoVect();
		InitDifQues();
	}

	void RegionSegmentor::InitRgnInfoVect(void)
	{
		m_rgnInfoVect.resize(m_src->GetWidth() * m_src->GetHeight());

		S16ChannelRef srcCh0 = m_src->GetAt(0);
		S16ChannelRef srcCh1 = m_src->GetAt(1);
		S16ChannelRef srcCh2 = m_src->GetAt(2);

		//IndexCalc2D indexCalc(m_src->GetWidth(), m_src->GetHeight());

		for(int y=0; y<srcCh0->GetHeight(); y++)
		{
			for(int x=0; x<srcCh0->GetWidth(); x++)
			{	
				//int nRgnIdx = indexCalc.Calc(x, y);
				//RgnInfo * pRgnInfo = &m_rgnInfoVect[nRgnIdx];
				RgnInfo * pRgnInfo = GetPointRgn(x, y);

				pRgnInfo->pActRgn = pRgnInfo;

				pRgnInfo->x = x;
				pRgnInfo->y = y;

				pRgnInfo->nofPixs = 1;

				pRgnInfo->nMeanCh0 = pRgnInfo->nLocalCh0 = 
					srcCh0->GetAt(x, y);
				pRgnInfo->nMeanCh1 = pRgnInfo->nLocalCh1 = 
					srcCh1->GetAt(x, y);
				pRgnInfo->nMeanCh2 = pRgnInfo->nLocalCh2 = 
					srcCh2->GetAt(x, y);
			}
		}

	}

	inline RegionSegmentor::RgnLink * RegionSegmentor::CreateLink(
		RgnInfo * a_pRgn, RgnLinkDir a_dir)
	{	
		static int dx[] = {1, 1, 0, -1};
		static int dy[] = {0, 1, 1, 1};
			
		RgnLink * pLink = &m_rgnLinkVect[m_nofLinks++];
		pLink->pPrev = pLink;
		pLink->pRgn1 = a_pRgn;
		pLink->pRgn2 = 	GetPointRgn(a_pRgn->x + dx[(int)a_dir], 
			a_pRgn->y + dy[(int)a_dir]);
		pLink->dir = a_dir;

		return pLink;
	}



	void RegionSegmentor::InitDifQues(void)
	{
		const int nSrcWidth = m_src->GetWidth();
		const int nSrcHeight = m_src->GetHeight();

		m_rgnLinkVect.resize( (4 * nSrcWidth * nSrcHeight) -
			nSrcHeight -	//	RC
			2 * (nSrcHeight + nSrcWidth - 1) -	//	RB + LB
			nSrcWidth	//	CB
			);

		RgnLink * pLink;

		for(int y=0; y<nSrcHeight-1; y++)
		{
			for(int x=1; x<nSrcWidth-1; x++)
			{	
				//	RC, RB, CB, LB
				
				RgnInfo * pRgn = GetPointRgn(x, y);

				pLink = CreateLink(pRgn, RC);
				m_difQues.PushPtr(CalcLinkDif(pLink), pLink);

				pLink = CreateLink(pRgn, RB);
				m_difQues.PushPtr(CalcLinkDif(pLink), pLink);

				pLink = CreateLink(pRgn, CB);
				m_difQues.PushPtr(CalcLinkDif(pLink), pLink);				

				pLink = CreateLink(pRgn, LB);
				m_difQues.PushPtr(CalcLinkDif(pLink), pLink);
			}
		}

		for(int y=0, x=0; y<nSrcHeight-1; y++)
		{
			//	RC, RB, CB

			RgnInfo * pRgn = GetPointRgn(x, y);

			pLink = CreateLink(pRgn, RC);
			m_difQues.PushPtr(CalcLinkDif(pLink), pLink);

			pLink = CreateLink(pRgn, RB);
			m_difQues.PushPtr(CalcLinkDif(pLink), pLink);

			pLink = CreateLink(pRgn, CB);
			m_difQues.PushPtr(CalcLinkDif(pLink), pLink);
		}

		for(int y=nSrcHeight-1, x=0; x<nSrcWidth-1; x++)
		{	
			// RC

			RgnInfo * pRgn = GetPointRgn(x, y);

			pLink = CreateLink(pRgn, RC);
			m_difQues.PushPtr(CalcLinkDif(pLink), pLink);
		}

		for(int y=0, x=nSrcWidth-1; y<nSrcHeight-1; y++)
		{
			//	CB, LB

			RgnInfo * pRgn = GetPointRgn(x, y);

			pLink = CreateLink(pRgn, CB);
			m_difQues.PushPtr(CalcLinkDif(pLink), pLink);

			pLink = CreateLink(pRgn, LB);
			m_difQues.PushPtr(CalcLinkDif(pLink), pLink);
		}

	}

	S16ImageRef RegionSegmentor::GenDifImg(bool a_bRC, bool a_bRB,
		bool a_bCB, bool a_bLB, int a_nMinDif, int a_nMaxDif)
	{
		if(a_nMaxDif < 0)
			a_nMaxDif = GetMaxDif();

		int nWCnt = 0;
		int nBCnt = 0;

		S16ImageRef ret = S16Image::Create(m_src->GetSize(), 1);
		ret->SetAll(0);

		S16ChannelRef retCh0 = ret->GetAt(0);

		bool dirFlags[] = {false, false, false, false};

		if(a_bRC)
			dirFlags[RC] = true;
		if(a_bRB)
			dirFlags[RB] = true;
		if(a_bCB)
			dirFlags[CB] = true;
		if(a_bLB)
			dirFlags[LB] = true;


		ListQue< RgnLink > tmpQue;

		for(int i=0; i<m_difQues.GetNofQues(); i++)
		{
			RgnLink * pLink = m_difQues.PopPtr(i);

			while(NULL != pLink)
			{
				if( ! dirFlags[ pLink->dir ] )
					goto Continue;

				if( i < a_nMinDif ||
					i > a_nMaxDif)
				{
					nBCnt++;
					goto Continue;
				}

				int x = pLink->pRgn1->x;
				int y = pLink->pRgn1->y;

				//retCh0->SetAt(x, y, i+ 100);
				retCh0->SetAt(x, y, 255);
				nWCnt++;

Continue:
				tmpQue.PushPtr(pLink);
				pLink = m_difQues.PopPtr(i);
			}

			m_difQues.PushQue(i, &tmpQue);
		}

		return ret;
	}

	HistoGramRef RegionSegmentor::GenDifHisto(bool a_bRC, bool a_bRB,
		bool a_bCB, bool a_bLB)
	{
		HistoGramRef hist = new HistoGram( GetMaxDif() + 1 );
		std::vector<Int32> & rHistVect = *hist->GetVectPtr();

		bool dirFlags[] = {false, false, false, false};

		if(a_bRC)
			dirFlags[RC] = true;
		if(a_bRB)
			dirFlags[RB] = true;
		if(a_bCB)
			dirFlags[CB] = true;
		if(a_bLB)
			dirFlags[LB] = true;


		ListQue< RgnLink > tmpQue;

		for(int i=0; i<m_difQues.GetNofQues(); i++)
		{
			RgnLink * pLink = m_difQues.PopPtr(i);

			while(NULL != pLink)
			{
				if( ! dirFlags[ pLink->dir ] )
					continue;

				rHistVect[ i ]++;

				tmpQue.PushPtr(pLink);
				pLink = m_difQues.PopPtr(i);
			}

			m_difQues.PushQue(i, &tmpQue);
		}

		return hist;
	}

	void RegionSegmentor::Segment(int a_nMergDif)
	{
		ListQue< RgnLink > tmpQue;
		
		//for(int j=0, nLim = m_difQues.GetNofQues(); j<2; j++, nLim = a_nMergDif + 1)
		//for(int j=0, nLim = a_nMergDif + 1; j<2; j++, nLim = a_nMergDif + 1)
		for(int nMergDif = 1; nMergDif<a_nMergDif; nMergDif += 1)
		{
			//for(int i=0; i<m_difQues.GetNofQues(); i++)
			for(int i=0; i<a_nMergDif; i++)				
			{
				RgnLink * pLink = m_difQues.PopPtr(i);

				while(NULL != pLink)
				{
					short nDif = CalcLinkDif(pLink);

					if(nDif <= nMergDif)
					{
						MergLink(pLink);
					}
					else if(nDif == i)
					{
						tmpQue.PushPtr(pLink);
					}
					else
					{
						m_difQues.PushPtr(nDif, pLink);
					}				

					pLink = m_difQues.PopPtr(i);
				}

				m_difQues.PushQue(i, &tmpQue);
			}

		}
	}

	short RegionSegmentor::CalcRgnDif(RgnInfo * a_pRgn1, RgnInfo * a_pRgn2)
	{
		UpdateActRgn(a_pRgn1);
		UpdateActRgn(a_pRgn2);

		float nDif0 = 
			(m_meanRatio * a_pRgn1->pActRgn->nMeanCh0 + 
			m_localRatio * a_pRgn1->nLocalCh0) -
			(m_meanRatio * a_pRgn2->pActRgn->nMeanCh0 + 
			m_localRatio * a_pRgn2->nLocalCh0);

		float nDif1 = 
			(m_meanRatio * a_pRgn1->pActRgn->nMeanCh1 + 
			m_localRatio * a_pRgn1->nLocalCh1) -
			(m_meanRatio * a_pRgn2->pActRgn->nMeanCh1 + 
			m_localRatio * a_pRgn2->nLocalCh1);

		float nDif2 = 
			(m_meanRatio * a_pRgn1->pActRgn->nMeanCh2 + 
			m_localRatio * a_pRgn1->nLocalCh2) -
			(m_meanRatio * a_pRgn2->pActRgn->nMeanCh2 + 
			m_localRatio * a_pRgn2->nLocalCh2);

/*		nDif0 *= 0.51;
		nDif1 *= 1.44;
		nDif2 *= 1.048;*/

		double res = 

		sqrt( (float) ( 
			Sqr(nDif0 * DIFFACT) + Sqr(nDif1 * DIFFACT) + Sqr(nDif2 * DIFFACT) ));
			//+ 0.9);
			//+ 0.0);			

		if( res > 0 && res < 1 
			)
			res = res;

		double res1 = res + 0.9;
		short res2 = (short) res;

		//if( res >= 0 && res < 1 
		if( res2 == 0
//			&& nDif0 == 0 && nDif1 == 0 && nDif2 == 0 )
			&& (nDif0 != 0 || nDif1 != 0 || nDif2 != 0 ))
			//&& res != 0.0 )
			res1 = res1;


		return res2;

/*		return (short)sqrt( 
			m_meanRatio * (
			0.51 * Sqr(a_pRgn1->pActRgn->nMeanCh0 - a_pRgn2->pActRgn->nMeanCh0) +
			1.44 * Sqr(a_pRgn1->pActRgn->nMeanCh1 - a_pRgn2->pActRgn->nMeanCh1) +
			1.048 * Sqr(a_pRgn1->pActRgn->nMeanCh2 - a_pRgn2->pActRgn->nMeanCh2)) +

			m_localRatio * (
			0.51 * Sqr(a_pRgn1->nLocalCh0 - a_pRgn2->nLocalCh0) +
			1.44 * Sqr(a_pRgn1->nLocalCh1 - a_pRgn2->nLocalCh1) +
			1.048 * Sqr(a_pRgn1->nLocalCh2 - a_pRgn2->nLocalCh2)) );*/
	}

	void RegionSegmentor::MergRgns(RgnInfo * a_pActRgn1, RgnInfo * a_pActRgn2)
	{
		Hcpl_ASSERT(a_pActRgn1->pActRgn == a_pActRgn1);
		Hcpl_ASSERT(a_pActRgn2->pActRgn == a_pActRgn2);

		if(a_pActRgn1 == a_pActRgn2)
			return;

		RgnInfo * pMaster, * pSlave;

		if(a_pActRgn1 == a_pActRgn2)
		{
			return;
		}
		else if(a_pActRgn1 < a_pActRgn2)
		{
			pMaster = a_pActRgn1;
			pSlave = a_pActRgn2;
		}
		else
		{
			pMaster = a_pActRgn2;
			pSlave = a_pActRgn1;
		}

		int nNewNofPixs = pMaster->nofPixs + pSlave->nofPixs;

		float nNewMeanCh0 = (pMaster->nofPixs * pMaster->nMeanCh0 +
			pSlave->nofPixs * pSlave->nMeanCh0) / nNewNofPixs;
		float nNewMeanCh1 = (pMaster->nofPixs * pMaster->nMeanCh1 +
			pSlave->nofPixs * pSlave->nMeanCh1) / nNewNofPixs;
		float nNewMeanCh2 = (pMaster->nofPixs * pMaster->nMeanCh2 +
			pSlave->nofPixs * pSlave->nMeanCh2) / nNewNofPixs;

		pMaster->nofPixs = nNewNofPixs;

		pMaster->nMeanCh0 = nNewMeanCh0;
		pMaster->nMeanCh1 = nNewMeanCh1;
		pMaster->nMeanCh2 = nNewMeanCh2;

		pSlave->pActRgn = pMaster;
	}

	void RegionSegmentor::UpdateActRgn(RgnInfo * a_pRgn)
	{
		RgnInfo * pActRgn = a_pRgn;

		static std::vector<RgnInfo *> rgnVect(100);
		int nofRgns = 0;

		while(pActRgn != pActRgn->pActRgn)
		{
			rgnVect[nofRgns++] = pActRgn;
			pActRgn = pActRgn->pActRgn;
		}

		for(int i=0; i<nofRgns; i++)
			rgnVect[i]->pActRgn = pActRgn;
	}

	S16ImageRef RegionSegmentor::GenSegmentedImage(void)
	{
		S16ImageRef ret = S16Image::Create(m_src->GetSize(), 3);

		S16ChannelRef retCh0 = ret->GetAt(0);
		S16ChannelRef retCh1 = ret->GetAt(1);
		S16ChannelRef retCh2 = ret->GetAt(2);

		for(int y=0; y<retCh0->GetHeight(); y++)
		{
			for(int x=0; x<retCh0->GetWidth(); x++)
			{	
				RgnInfo * pRgn = GetPointRgn(x, y);
				UpdateActRgn(pRgn);

				retCh0->SetAt(x, y, (short)pRgn->pActRgn->nMeanCh0);
				retCh1->SetAt(x, y, (short)pRgn->pActRgn->nMeanCh1);
				retCh2->SetAt(x, y, (short)pRgn->pActRgn->nMeanCh2);
			}
		}

		return ret;
	}

}