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


	RegionSegmentor::RegionSegmentor(S16ImageRef a_src)
	{
		m_nMaxDif = (short)sqrt( (float) ( 3 * Sqr(255)) );

		m_src = a_src;

		m_difRC = S16Image::Create(m_src->GetSize(), 1);
		m_difRB = S16Image::Create(m_src->GetSize(), 1);
		m_difCB = S16Image::Create(m_src->GetSize(), 1);
		m_difLB = S16Image::Create(m_src->GetSize(), 1);

		int nQueSizVectSiz = m_nMaxDif;

		m_queSizVectRC.resize(nQueSizVectSiz);
		m_queSizVectRB.resize(nQueSizVectSiz);
		m_queSizVectCB.resize(nQueSizVectSiz);
		m_queSizVectLB.resize(nQueSizVectSiz);

		ResetQueSizeVects();
	}

	void RegionSegmentor::ResetQueSizeVects(void)
	{
		for(int i=0; i<m_queSizVectRC.size(); i++)
			m_queSizVectRC[i] = 0;
		for(int i=0; i<m_queSizVectRB.size(); i++)
			m_queSizVectRB[i] = 0;
		for(int i=0; i<m_queSizVectCB.size(); i++)
			m_queSizVectCB[i] = 0;
		for(int i=0; i<m_queSizVectLB.size(); i++)
			m_queSizVectLB[i] = 0;
	}

	void RegionSegmentor::CalcDiffs(void)
	{
		short nMaxVal = m_nMaxDif;


		S16ChannelRef srcCh0 = m_src->GetAt(0);
		S16ChannelRef srcCh1 = m_src->GetAt(1);
		S16ChannelRef srcCh2 = m_src->GetAt(2);

		S16ChannelRef difChRC = m_difRC->GetAt(0);
		S16ChannelRef difChRB = m_difRB->GetAt(0);
		S16ChannelRef difChCB = m_difCB->GetAt(0);
		S16ChannelRef difChLB = m_difLB->GetAt(0);		

		{
			FillBorder(difChRC, (Int16)nMaxVal, 1);
			FillBorder(difChRB, (Int16)nMaxVal, 1);
			FillBorder(difChCB, (Int16)nMaxVal, 1);
			FillBorder(difChLB, (Int16)nMaxVal, 1);
		}

		for(int y=0; y<srcCh0->GetHeight()-1; y++)
		{
			for(int x=1; x<srcCh0->GetWidth()-1; x++)
			{		
				short nVal0 = srcCh0->GetAt(x, y);
				short nVal1 = srcCh1->GetAt(x, y);
				short nVal2 = srcCh2->GetAt(x, y);

				short nVal;

				nVal = (short)sqrt( (float) ( 
					Sqr(nVal0 - srcCh0->GetAt(x + 1, y)) +
					Sqr(nVal1 - srcCh1->GetAt(x + 1, y)) +
					Sqr(nVal2 - srcCh2->GetAt(x + 1, y)) ) );
				difChRC->SetAt(x, y, nVal);
				m_queSizVectRC[nVal]++;

				nVal = (short)sqrt( (float) ( 
					Sqr(nVal0 - srcCh0->GetAt(x + 1, y + 1)) +
					Sqr(nVal1 - srcCh1->GetAt(x + 1, y + 1)) +
					Sqr(nVal2 - srcCh2->GetAt(x + 1, y + 1)) ) );
				difChRB->SetAt(x, y, nVal);
				m_queSizVectRB[nVal]++;

				nVal = (short)sqrt( (float) ( 
					Sqr(nVal0 - srcCh0->GetAt(x, y + 1)) +
					Sqr(nVal1 - srcCh1->GetAt(x, y + 1)) +
					Sqr(nVal2 - srcCh2->GetAt(x, y + 1)) ) );
				difChCB->SetAt(x, y, nVal);
				m_queSizVectCB[nVal]++;

				nVal = (short)sqrt( (float) ( 
					Sqr(nVal0 - srcCh0->GetAt(x - 1, y + 1)) +
					Sqr(nVal1 - srcCh1->GetAt(x - 1, y + 1)) +
					Sqr(nVal2 - srcCh2->GetAt(x - 1, y + 1)) ) );
				difChLB->SetAt(x, y, nVal);
				m_queSizVectLB[nVal]++;
			}
		}

		for(int y=0, x=0; y<srcCh0->GetHeight()-1; y++)
		{
			short nVal0 = srcCh0->GetAt(x, y);
			short nVal1 = srcCh1->GetAt(x, y);
			short nVal2 = srcCh2->GetAt(x, y);

			short nVal;

			nVal = (short)sqrt( (float) ( 
				Sqr(nVal0 - srcCh0->GetAt(x + 1, y)) +
				Sqr(nVal1 - srcCh1->GetAt(x + 1, y)) +
				Sqr(nVal2 - srcCh2->GetAt(x + 1, y)) ) );
			difChRC->SetAt(x, y, nVal);
			m_queSizVectRC[nVal]++;

			nVal = (short)sqrt( (float) ( 
				Sqr(nVal0 - srcCh0->GetAt(x + 1, y + 1)) +
				Sqr(nVal1 - srcCh1->GetAt(x + 1, y + 1)) +
				Sqr(nVal2 - srcCh2->GetAt(x + 1, y + 1)) ) );
			difChRB->SetAt(x, y, nVal);
			m_queSizVectRB[nVal]++;

			nVal = (short)sqrt( (float) ( 
				Sqr(nVal0 - srcCh0->GetAt(x, y + 1)) +
				Sqr(nVal1 - srcCh1->GetAt(x, y + 1)) +
				Sqr(nVal2 - srcCh2->GetAt(x, y + 1)) ) );
			difChCB->SetAt(x, y, nVal);
			m_queSizVectCB[nVal]++;
		}

		for(int y=srcCh0->GetHeight()-1, x=0; x<srcCh0->GetWidth()-1; x++)
		{		
			short nVal0 = srcCh0->GetAt(x, y);
			short nVal1 = srcCh1->GetAt(x, y);
			short nVal2 = srcCh2->GetAt(x, y);

			short nVal;

			nVal = (short)sqrt( (float) ( 
				Sqr(nVal0 - srcCh0->GetAt(x + 1, y)) +
				Sqr(nVal1 - srcCh1->GetAt(x + 1, y)) +
				Sqr(nVal2 - srcCh2->GetAt(x + 1, y)) ) );
			difChRC->SetAt(x, y, nVal);
			m_queSizVectRC[nVal]++;
		}

		for(int y=0, x=srcCh0->GetWidth()-1; y<srcCh0->GetHeight()-1; y++)
		{
			short nVal0 = srcCh0->GetAt(x, y);
			short nVal1 = srcCh1->GetAt(x, y);
			short nVal2 = srcCh2->GetAt(x, y);

			short nVal;

			nVal = (short)sqrt( (float) ( 
				Sqr(nVal0 - srcCh0->GetAt(x, y + 1)) +
				Sqr(nVal1 - srcCh1->GetAt(x, y + 1)) +
				Sqr(nVal2 - srcCh2->GetAt(x, y + 1)) ) );
			difChCB->SetAt(x, y, nVal);
			m_queSizVectCB[nVal]++;

			nVal = (short)sqrt( (float) ( 
				Sqr(nVal0 - srcCh0->GetAt(x - 1, y + 1)) +
				Sqr(nVal1 - srcCh1->GetAt(x - 1, y + 1)) +
				Sqr(nVal2 - srcCh2->GetAt(x - 1, y + 1)) ) );
			difChLB->SetAt(x, y, nVal);
			m_queSizVectLB[nVal]++;
		}

	}

}