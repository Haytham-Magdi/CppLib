#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\PixelTypes.h>
#include <Lib\Hcv\error.h>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>

#include <Lib\Hcv\Hsv.h>


namespace Hcv
{
	using namespace Hcpl;


	void Hsv::GetHsvOfRgb(Hcpl::Uint8 a_nRed, Hcpl::Uint8 a_nGrn, Hcpl::Uint8 a_nBlu, 
		Hcpl::Uint8 *a_pHue, Hcpl::Uint8 *a_pSat, Hcpl::Uint8 *a_pVal)
	{
		Uint8 nMax = 0;
		if(a_nRed > nMax)
			nMax = a_nRed;
		if(a_nGrn > nMax)
			nMax = a_nGrn;
		if(a_nBlu > nMax)
			nMax = a_nBlu;

		Uint8 nMin = 255;
		if(a_nRed < nMin)
			nMin = a_nRed;
		if(a_nGrn < nMin)
			nMin = a_nGrn;
		if(a_nBlu < nMin)
			nMin = a_nBlu;


		int nHue;

		if(0 == (nMax - nMin))
			nHue = 0;
		else
		{
			if(nMax == a_nRed)
				nHue = 60 * (a_nGrn-a_nBlu) / (nMax-nMin);
			else if(nMax == a_nGrn)
				nHue = 2 * 60 + 60 * (a_nBlu-a_nRed) / (nMax-nMin);
			else	//	(nMax == a_nBlu)
				nHue = 4 * 60 + 60 * (a_nRed-a_nGrn) / (nMax-nMin);

			if(nHue < 0)
				nHue += 360;

			nHue = nHue * 256 / 360;
			*a_pHue = nHue;
		}



		Uint8 nSat = 0;
		if(0 !=	nMax)	
			nSat = (Uint8)( (nMax-nMin) * 255 / nMax); 
		//Uint8 nSat = (Uint8)((ULONG)(nMax-nMin)*255/nMax); 
		*a_pSat = nSat;


		*a_pVal = nMax;
	}


	void Hsv::CalcHsvDiff(Hcv::S16ImageRef a_srcP, Hcv::S16ImageRef a_srcAN,
		Hcv::S16ImageRef a_dst)
	{
		// HTODO : validate identical sizes

		S16ChannelRef bluChP = a_srcP->GetAt(0);
		S16ChannelRef grnChP = a_srcP->GetAt(1);
		S16ChannelRef redChP = a_srcP->GetAt(2);

		S16ChannelRef bluChAN = a_srcAN->GetAt(0);
		S16ChannelRef grnChAN = a_srcAN->GetAt(1);
		S16ChannelRef redChAN = a_srcAN->GetAt(2);

		S16ChannelRef hueCh = a_dst->GetAt(0);
		S16ChannelRef satCh = a_dst->GetAt(1);
		S16ChannelRef valCh = a_dst->GetAt(2);

		for(int y=0; y<a_srcP->GetHeight(); y++)
			for(int x=0; x<a_srcP->GetWidth(); x++)
			{
				Uint8 nRed, nGrn, nBlu;

				nRed = (Uint8) redChP->GetAt(x, y);
				nGrn = (Uint8) grnChP->GetAt(x, y);
				nBlu = (Uint8) bluChP->GetAt(x, y);

				Uint8 nHueP, nSatP, nValP;

				Hsv::GetHsvOfRgb(nRed, nGrn, nBlu, &nHueP, &nSatP, &nValP);

				nRed = (Uint8) redChAN->GetAt(x, y);
				nGrn = (Uint8) grnChAN->GetAt(x, y);
				nBlu = (Uint8) bluChAN->GetAt(x, y);

				Uint8 nHueAN, nSatAN, nValAN;

				Hsv::GetHsvOfRgb(nRed, nGrn, nBlu, &nHueAN, &nSatAN, &nValAN);

				
				Uint8 nMinSat = 255;
				if(nSatP < nMinSat)
					nMinSat = nSatP;
				if(nSatAN < nMinSat)
					nMinSat = nSatAN;

				Uint8 nHueD = nHueP - nHueAN;
				if(nHueD > 127)
					nHueD = 255 - nHueD;
				//nHueD *= 2;
				Uint8 nMaxHueD = 43;
				if(nHueD >= nMaxHueD)
					nHueD = 255;
				else
					nHueD = nHueD * 255 / nMaxHueD;
				if(nMinSat < (30))
					nHueD = nHueD * nMinSat / 256;


				Uint8 nSatD = abs( nSatP - nSatAN );
				Uint8 nMaxSatD = 120;
				if(nSatD >= nMaxSatD)
					nSatD = 255;
				else
					nSatD = nSatD * 255 / nMaxSatD;


				Uint8 nValD = abs( nValP - nValAN );
				Uint8 nMaxValD = 120;
				if(nValD >= nMaxValD)
					nValD = 255;
				else
					nValD = nValD * 255 / nMaxValD;

				hueCh->SetAt(x, y, nValD);
//				hueCh->SetAt(x, y, 0);
				satCh->SetAt(x, y, nSatD);
				valCh->SetAt(x, y, nHueD);

			}
	}
}