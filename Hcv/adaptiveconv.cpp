#include <Lib\Hcv\AdaptiveConv.h>

namespace Hcv
{
	using namespace Hcpl;

	AdaptiveConv * AdaptiveConv::Create(ConvKernelRef a_convKernel)
	{
		AdaptiveConv * ac = new AdaptiveConv();
		ac->m_convKernel = a_convKernel;

		return ac;
	}

	S16ImageRef AdaptiveConv::GenResult(S16ImageRef a_src)
	{
		S16ImageRef ret = S16Image::Create(
			a_src->GetSize(), a_src->GetNofChannels());		

		S16ChannelRef srcCh0 = a_src->GetAt(0);
		S16ChannelRef srcCh1 = a_src->GetAt(1);
		S16ChannelRef srcCh2 = a_src->GetAt(2);

		S16ChannelRef dstCh0 = ret->GetAt(0);
		S16ChannelRef dstCh1 = ret->GetAt(1);
		S16ChannelRef dstCh2 = ret->GetAt(2);
		
		F32ChannelRef kerChnl = GetConvKernelChannel( m_convKernel );
		int nKerInc = GetConvKernelSpace( m_convKernel ) + 1;

		int nBgnX = GetConvKernelLeftMarg( m_convKernel );
		int nLimX = srcCh0->GetWidth() - 
			GetConvKernelRightMarg( m_convKernel );
		int nKerLimX = kerChnl->GetWidth();

		int nBgnY = GetConvKernelTopMarg( m_convKernel );
		int nLimY = srcCh0->GetHeight() - 
			GetConvKernelBotomMarg( m_convKernel );
		int nKerLimY = kerChnl->GetHeight();

		CvPoint anch = GetConvKernelAnch( m_convKernel );

		
		for(int i=0; i<ret->GetNofChannels(); i++)
		{
			FillBorder(ret->GetAt(i), (Int16)0,
				GetConvKernelLeftMarg( m_convKernel ),
				GetConvKernelRightMarg( m_convKernel ),
				GetConvKernelTopMarg( m_convKernel ),
				GetConvKernelBotomMarg( m_convKernel ));
		}

		for(int y=nBgnY; y<nLimY; y++)
		{
			for(int x=nBgnX; x<nLimX; x++)
			{
				Int16 nOrgVal0 = srcCh0->GetAt(x, y);
				Int16 nOrgVal1 = srcCh1->GetAt(x, y);
				Int16 nOrgVal2 = srcCh2->GetAt(x, y);

				int nSrcBgnX = x - anch.x * nKerInc;
				int nSrcBgnY = y - anch.y * nKerInc;

				float sum0 = 0;
				float sum1 = 0;
				float sum2 = 0;

				int nWeightSum = 0;


				for(int ky = 0; ky<nKerLimY; ky++)
				{
					for(int kx = 0; kx<nKerLimX; kx++)
					{
						Int16 nVal0 = srcCh0->GetAt(nSrcBgnX + kx * nKerInc,
							nSrcBgnY + ky * nKerInc);
						Int16 nVal1 = srcCh1->GetAt(nSrcBgnX + kx * nKerInc,
							nSrcBgnY + ky * nKerInc);
						Int16 nVal2 = srcCh2->GetAt(nSrcBgnX + kx * nKerInc, 
							nSrcBgnY + ky * nKerInc);

						int nDif = (short)sqrt( (float) ( 
							Sqr(nOrgVal0 - nVal0) + 
							Sqr(nOrgVal1 - nVal1) + 
							Sqr(nOrgVal2 - nVal2) ) / 1 );

						int nDif2 = nDif;

						if( nDif >  20)
							nDif2 *= 1.5;
						if( nDif >  40)
							nDif2 *= 1.5;
						if( nDif >  60)
							nDif2 *= 1.5;
						if( nDif >  80)
							nDif2 *= 1.5;
						if( nDif >  100)
							nDif2 *= 1.5;

						if( nDif2 > 255 )
							nDif2 = 255;


						int nWeight = 255 - nDif2;
						nWeight *= kerChnl->GetAt(kx, ky);

						nWeightSum += nWeight;

						sum0 += nVal0 * nWeight;

						sum1 += nVal1 * nWeight;

						sum2 += nVal2 * nWeight;
					}
				}

/*				Hcpl_ASSERT((Int16)(sum0 / nWeightSum) <= 255);
				Hcpl_ASSERT((Int16)(sum1 / nWeightSum) <= 255);
				Hcpl_ASSERT((Int16)(sum2 / nWeightSum) <= 255);*/

				dstCh0->SetAt(x, y, (Int16)(sum0 / nWeightSum));
				dstCh1->SetAt(x, y, (Int16)(sum1 / nWeightSum));
				dstCh2->SetAt(x, y, (Int16)(sum2 / nWeightSum));
			}
		}




		return ret;
	}

}