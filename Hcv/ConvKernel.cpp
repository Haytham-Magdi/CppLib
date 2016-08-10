#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\ConvKernel.h>

namespace Hcv
{
	using namespace Hcpl;

	void ConvKernel::PrepareMargins(ConvKernel * a_pCK)
	{
		ConvKernel * ck = a_pCK;

		ck->m_nLeftMarg = ck->m_anch.x;
		if(ck->m_nLeftMarg < 0)
			ck->m_nLeftMarg = 0;
		ck->m_nLeftMarg *= ck->m_nSpace + 1;

		ck->m_nRightMarg = ck->m_img->GetWidth() - 1 - ck->m_anch.x;
		if(ck->m_nRightMarg < 0)
			ck->m_nRightMarg = 0;
		ck->m_nRightMarg *= ck->m_nSpace + 1;

		ck->m_nTopMarg = ck->m_anch.y;
		if(ck->m_nTopMarg < 0)
			ck->m_nTopMarg = 0;
		ck->m_nTopMarg *= ck->m_nSpace + 1;

		ck->m_nBotomMarg = ck->m_img->GetHeight() - 1 - ck->m_anch.y;
		if(ck->m_nBotomMarg < 0)
			ck->m_nBotomMarg = 0;
		ck->m_nBotomMarg *= ck->m_nSpace + 1;
	}

	void ConvKernel::Prepare(void)
	{
		ConvKernel * ck = this;
		int nAprSiz = ck->m_img->GetSize().width;

		F32ChannelRef ch = ck->m_img->GetAt(0);

		Float sumP = 0, sumN = 0;

		int x, y;

		for(y=0; y<ch->GetHeight(); y++)
		{
			for(x=0; x<ch->GetWidth(); x++)
			{
				float val = ch->GetAt(x, y);

				if(val > 0)
					sumP += val;
				else
					sumN += val;
			}
		}

		Float maxAbsVal = fabs(sumP) > fabs(sumN) ? fabs(sumP) : fabs(sumN);

		for(y=0; y<ch->GetHeight(); y++)
		{
			for(x=0; x<ch->GetWidth(); x++)
			{
				ch->SetAt(x, y,
					ch->GetAt(x, y) / maxAbsVal);
			}
		}

		ck->m_anch = cvPoint(nAprSiz / 2, nAprSiz / 2);

		PrepareMargins(ck);
	}

	ConvKernel * ConvKernel::CreateSobel(int a_dx, int a_dy, 
		int a_aprSiz, int a_nSpace)
	{
		ConvKernel * ck = new ConvKernel(a_nSpace);

		ck->m_img = F32Image::Create( cvSize(a_aprSiz, a_aprSiz), 1);

		throw "Not complete implementation !";

		//HCV_CALL( cvTsCalcSobelKernel2D( a_dx, a_dy, a_aprSiz, 0, 
			//ck->m_img->GetMatPtr() ) );

		ck->Prepare();

		return ck;
	}

	ConvKernel * ConvKernel::CreateBlur(int a_aprSiz, int a_nSpace)
	{
		ConvKernel * ck = new ConvKernel(a_nSpace);

		ck->m_img = F32Image::Create( cvSize(a_aprSiz, a_aprSiz), 1);

		ck->m_img->SetAll(1);

		ck->Prepare();

		return ck;
	}



	ConvKernel * ConvKernel::CreateFilter1(int a_aprSiz, int a_nSpace)
	{
		ConvKernel * ck = new ConvKernel(a_nSpace);

		ck->m_img = F32Image::Create( cvSize(a_aprSiz, a_aprSiz), 1);

		ck->m_img->SetAll(0);

		const int cx = a_aprSiz / 2;
		const int cy = a_aprSiz / 2;

		for(int i=0; i < a_aprSiz; i++)
		{
			float * pix = ck->m_img->GetPixAt( i, cy );
			*pix = 1;
		}

		for(int i=0; i < a_aprSiz; i++)
		{
			float * pix = ck->m_img->GetPixAt( cx, i );
			*pix = 1;
		}

		for(int i=0; i < a_aprSiz; i++)
		{
			float * pix = ck->m_img->GetPixAt( i, a_aprSiz - 1 - i );
			*pix = 1;
		}

		ck->Prepare();

		return ck;
	}



	void ConvKernel::Process(S16ImageRef a_src, S16ImageRef a_dst)
	{
		//	HTODO : Validate src, dst Identical

		S16ChannelRef chnl = a_dst->GetAt(0);
		F32ChannelRef kerChnl = m_img->GetAt(0);

		int nKerInc = m_nSpace + 1;

		int nBgnX = m_nLeftMarg;
		int nLimX = chnl->GetWidth() - m_nRightMarg;
		int nKerLimX = kerChnl->GetWidth();

		int nBgnY = m_nTopMarg;
		int nLimY = chnl->GetHeight() - m_nBotomMarg;
		int nKerLimY = kerChnl->GetHeight();

		for(int nCh = 0; nCh < a_dst->GetNofChannels(); nCh++)
		{
			S16ChannelRef srcCh = a_src->GetAt(nCh); 
			S16ChannelRef dstCh = a_dst->GetAt(nCh); 

			FillBorder(dstCh, (Int16)0,
				m_nLeftMarg, m_nRightMarg, m_nTopMarg, m_nBotomMarg);

			for(int y=nBgnY; y<nLimY; y++)
			{
				for(int x=nBgnX; x<nLimX; x++)
				{
					int nSrcBgnX = x - m_anch.x * nKerInc;
					int nSrcBgnY = y - m_anch.y * nKerInc;

					float sum = 0;

					for(int ky = 0; ky<nKerLimY; ky++)
					{
						for(int kx = 0; kx<nKerLimX; kx++)
						{
							sum += srcCh->GetAt(
								nSrcBgnX + kx * nKerInc, 
								nSrcBgnY + ky * nKerInc) *
								kerChnl->GetAt(kx, ky);
						}
					}

					dstCh->SetAt(x, y, (Int16)sum);
				}
			}

		}

	}



	void ConvKernel::Process(F32ImageRef a_src, F32ImageRef a_dst)
	{
		//	HTODO : Validate src, dst Identical

		F32ChannelRef chnl = a_dst->GetAt(0);
		F32ChannelRef kerChnl = m_img->GetAt(0);

		int nKerInc = m_nSpace + 1;

		int nBgnX = m_nLeftMarg;
		int nLimX = chnl->GetWidth() - m_nRightMarg;
		int nKerLimX = kerChnl->GetWidth();

		int nBgnY = m_nTopMarg;
		int nLimY = chnl->GetHeight() - m_nBotomMarg;
		int nKerLimY = kerChnl->GetHeight();

		for(int nCh = 0; nCh < a_dst->GetNofChannels(); nCh++)
		{
			F32ChannelRef srcCh = a_src->GetAt(nCh); 
			F32ChannelRef dstCh = a_dst->GetAt(nCh); 

			FillBorder(dstCh, (float)0,
				m_nLeftMarg, m_nRightMarg, m_nTopMarg, m_nBotomMarg);

			for(int y=nBgnY; y<nLimY; y++)
			{
				for(int x=nBgnX; x<nLimX; x++)
				{
					int nSrcBgnX = x - m_anch.x * nKerInc;
					int nSrcBgnY = y - m_anch.y * nKerInc;

					float sum = 0;

					for(int ky = 0; ky<nKerLimY; ky++)
					{
						for(int kx = 0; kx<nKerLimX; kx++)
						{
							sum += srcCh->GetAt(
								nSrcBgnX + kx * nKerInc, 
								nSrcBgnY + ky * nKerInc) *
								kerChnl->GetAt(kx, ky);
						}
					}

					dstCh->SetAt(x, y, (Int16)sum);
				}
			}

		}

	}


	ConvKernel * ConvKernel::CreatePositiveInst(void)
	{
		int nBgnX, nLimX, nBgnY, nLimY;
		int x, y;

		F32ChannelRef chnl1 = m_img->GetAt(0);

		nBgnX = -1;
		nBgnY = -1;
		for(y=0; y<chnl1->GetHeight(); y++)
		{
			for(x=0; x<chnl1->GetWidth(); x++)
			{
				float val = chnl1->GetAt(x, y);

				if(val <= 0)
					continue;

				if(-1 == nBgnX)
					nBgnX = x;

				if(-1 == nBgnY)
					nBgnY = y;
			}
		}

		if(-1 == nBgnX)
			return NULL;

		nLimX = -1;
		nLimY = -1;
		for(y=chnl1->GetHeight()-1; y>=0; y--)
		{
			for(x=chnl1->GetWidth()-1; x>=0; x--)
			{
				float val = chnl1->GetAt(x, y);

				if(val <= 0)
					continue;

				if(-1 == nLimX)
					nLimX = x + 1;

				if(-1 == nLimY)
					nLimY = y + 1;
			}
		}

		ConvKernel * ck = new ConvKernel(m_nSpace);

		ck->m_img = F32Image::Create( 
			cvSize(nLimX - nBgnX, nLimY - nBgnY), 1);

		F32ChannelRef chnl2 = ck->m_img->GetAt(0);

		for(y=0; y<chnl2->GetHeight(); y++)
		{
			for(x=0; x<chnl2->GetWidth(); x++)
			{
				float val = chnl1->GetAt(
					nBgnX + x, nBgnY + y);

				if(val <= 0)
					val = 0;

				chnl2->SetAt(x, y, val);
			}
		}

		ck->m_anch.x = m_anch.x - nBgnX;
		ck->m_anch.y = m_anch.y - nBgnY;

		PrepareMargins(ck);

		return ck;
	}

	ConvKernel * ConvKernel::CreateNegativeInst(void)
	{
		int nBgnX, nLimX, nBgnY, nLimY;
		int x, y;

		F32ChannelRef chnl1 = m_img->GetAt(0);

		nBgnX = -1;
		nBgnY = -1;
		for(y=0; y<chnl1->GetHeight(); y++)
		{
			for(x=0; x<chnl1->GetWidth(); x++)
			{
				float val = chnl1->GetAt(x, y);

				if(val >= 0)
					continue;

				if(-1 == nBgnX)
					nBgnX = x;

				if(-1 == nBgnY)
					nBgnY = y;
			}
		}

		if(-1 == nBgnX)
			return NULL;

		nLimX = -1;
		nLimY = -1;
		for(y=chnl1->GetHeight()-1; y>=0; y--)
		{
			for(x=chnl1->GetWidth()-1; x>=0; x--)
			{
				float val = chnl1->GetAt(x, y);

				if(val >= 0)
					continue;

				if(-1 == nLimX)
					nLimX = x + 1;

				if(-1 == nLimY)
					nLimY = y + 1;
			}
		}

		ConvKernel * ck = new ConvKernel(m_nSpace);

		ck->m_img = F32Image::Create( 
			cvSize(nLimX - nBgnX, nLimY - nBgnY), 1);

		F32ChannelRef chnl2 = ck->m_img->GetAt(0);

		for(y=0; y<chnl2->GetHeight(); y++)
		{
			for(x=0; x<chnl2->GetWidth(); x++)
			{
				float val = chnl1->GetAt(
					nBgnX + x, nBgnY + y);

				if(val >= 0)
					val = 0;

				chnl2->SetAt(x, y, val);
			}
		}

		ck->m_anch.x = m_anch.x - nBgnX;
		ck->m_anch.y = m_anch.y - nBgnY;

		PrepareMargins(ck);

		return ck;
	}



	ConvKernel * ConvKernel::CreateAbsNegativeInst(void)
	{
		int nBgnX, nLimX, nBgnY, nLimY;
		int x, y;

		F32ChannelRef chnl1 = m_img->GetAt(0);

		nBgnX = -1;
		nBgnY = -1;
		for(y=0; y<chnl1->GetHeight(); y++)
		{
			for(x=0; x<chnl1->GetWidth(); x++)
			{
				float val = chnl1->GetAt(x, y);

				if(val >= 0)
					continue;

				if(-1 == nBgnX)
					nBgnX = x;

				if(-1 == nBgnY)
					nBgnY = y;
			}
		}

		if(-1 == nBgnX)
			return NULL;

		nLimX = -1;
		nLimY = -1;
		for(y=chnl1->GetHeight()-1; y>=0; y--)
		{
			for(x=chnl1->GetWidth()-1; x>=0; x--)
			{
				float val = chnl1->GetAt(x, y);

				if(val >= 0)
					continue;

				if(-1 == nLimX)
					nLimX = x + 1;

				if(-1 == nLimY)
					nLimY = y + 1;
			}
		}

		ConvKernel * ck = new ConvKernel(m_nSpace);

		ck->m_img = F32Image::Create( 
			cvSize(nLimX - nBgnX, nLimY - nBgnY), 1);

		F32ChannelRef chnl2 = ck->m_img->GetAt(0);

		for(y=0; y<chnl2->GetHeight(); y++)
		{
			for(x=0; x<chnl2->GetWidth(); x++)
			{
				float val = chnl1->GetAt(
					nBgnX + x, nBgnY + y);

				if(val >= 0)
					val = 0;

				chnl2->SetAt(x, y, - val);
			}
		}

		ck->m_anch.x = m_anch.x - nBgnX;
		ck->m_anch.y = m_anch.y - nBgnY;

		PrepareMargins(ck);

		return ck;
	}

}