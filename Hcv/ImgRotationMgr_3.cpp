#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\ImgRotationMgr_3.h>


#define M_PI 3.14159265358979323846


namespace Hcv
{
	using namespace Hcpl;
	//using namespace Hcpl::Math;


	ImgRotationMgr_3::ImgRotationMgr_3(F32ImageRef a_srcImg, float a_angDig)
	{
		Hcpl_ASSERT(a_angDig >= 0);
		Hcpl_ASSERT(a_angDig <= 90);

		m_srcImg = a_srcImg;
		m_angDig = a_angDig;
		m_angRad = a_angDig * M_PI / 180;

		m_nScale = 1000;
		//m_nScale = 10000;
		m_nRound = 0.555555 * m_nScale;
		m_nRoundByMin = 0.999999 * m_nScale;

		m_nCos = cos(m_angRad) * m_nScale;
		m_nSin = sin(m_angRad) * m_nScale;

		m_nCos++;
		m_nSin++;

		int mag1 = Sqr(m_nCos) + Sqr(m_nSin);
		Hcpl_ASSERT(mag1 >= Sqr(m_nScale));

		// tmp only
		//m_nCos *= 1.3;
		//m_nSin *= 1.3;

		Prepare();
	}


	int ImgRotationMgr_3::AddRound(int a_num)
	{
		int nRet;

		if (a_num > 0)
		{
			nRet = (int)(a_num + m_nRound);
		}
		else
		{
			nRet = (int)(a_num - m_nRound);
		}

		nRet /= m_nScale;
		nRet *= m_nScale;

		return nRet;
	}


	int ImgRotationMgr_3::AddRoundByMin(int a_num)
	{
		int nRet;

		if (a_num > 0)
		{
			nRet = (int)(a_num + m_nRoundByMin);
		}
		else
		{
			nRet = (int)(a_num - m_nRoundByMin);
		}

		nRet /= m_nScale;
		nRet *= m_nScale;

		return nRet;
	}



	void ImgRotationMgr_3::Prepare()
	{
		CvSize srcSiz = m_srcImg->GetSize();

		const int nScaled_SrcWidth = srcSiz.width * m_nScale;
		const int nScaled_SrcHeight = srcSiz.height * m_nScale;

		CvPoint bgnPnt;

		int nSafeMarg = 0;
		//int nSafeMarg = 20;

		{
			//int nofLinesBef = AddRound(
			int nofLinesBef = AddRoundByMin(
				m_nSin * (srcSiz.width + nSafeMarg));

			nofLinesBef /= m_nScale;

			bgnPnt.x = nofLinesBef * m_nSin;
			bgnPnt.y = -nofLinesBef * m_nCos;

			int nofLinesAft = AddRoundByMin(
				m_nCos * (srcSiz.height + nSafeMarg));

			nofLinesAft /= m_nScale;

			//m_resSiz.height = nofLinesBef + nofLinesAft;
			m_resSiz.height = nofLinesBef + nofLinesAft + 1;
		}

		{
			int nofLinesBef = m_nCos * (srcSiz.width + nSafeMarg);
			nofLinesBef = AddRoundByMin(nofLinesBef);

			nofLinesBef /= m_nScale;


			int nofLinesAft = m_nSin * (srcSiz.height + nSafeMarg);
			nofLinesAft = AddRoundByMin(nofLinesAft);

			nofLinesAft /= m_nScale;

			m_resSiz.width = nofLinesBef + nofLinesAft + 1;
		}

		//{
		//	int nofLinesBef = m_nCos * (srcSiz.width + nSafeMarg);

		//	int nofLinesAft = m_nSin * (srcSiz.height + nSafeMarg);

		//	//m_resSiz.width = AddRound(
		//	m_resSiz.width = AddRoundByMin(
		//		nofLinesBef + nofLinesAft );

		//	m_resSiz.width /= m_nScale;
		//}

		m_resToSrcMapImg = S32Image::Create(m_resSiz, 1);
		int * resToSrcBuf = (int *)m_resToSrcMapImg->GetPixAt(0, 0);

		//m_resToSrcMapImg_X_Scaled = S32Image::Create(m_resSiz, 1);
		//int * resToSrcBuf_X_Scaled = (int *)m_resToSrcMapImg_X_Scaled->GetPixAt(0, 0);

		//m_resToSrcMapImg_Y_Scaled = S32Image::Create(m_resSiz, 1);
		//int * resToSrcBuf_Y_Scaled = (int *)m_resToSrcMapImg_Y_Scaled->GetPixAt(0, 0);

		m_srcToResMapImg = S32Image::Create(srcSiz, 1);
		int *  srcToResBuf = (int *)m_srcToResMapImg->GetPixAt(0, 0);

		S32ImageRef srcMinDistImg = S32Image::Create(srcSiz, 1);
		int *  srcMinDistBuf = (int *)srcMinDistImg->GetPixAt(0, 0);

		m_resImg = F32Image::Create(m_resSiz, 3);
		F32ColorVal * resBuf = (F32ColorVal *)m_resImg->GetPixAt(0, 0);

		F32ColorVal * srcBuf = (F32ColorVal *)m_srcImg->GetPixAt(0, 0);

		const int nGreatDist = 10000000;
		srcMinDistImg->SetAll(nGreatDist);

		int nMappedSrcCnt = 0;





		IndexCalc2D idxCalc_Src(srcSiz.width, srcSiz.height);

		IndexCalc2D idxCalc_Res(m_resSiz.width, m_resSiz.height);

		//CvPoint curPnt = bgnPnt;

		FixedVector< CvPoint > srcPntArr(100);

		for (int y = 0; y < m_resSiz.height; y++)
		{
			CvPoint curPnt_Y;

			curPnt_Y.x = bgnPnt.x - y * m_nSin;
			curPnt_Y.y = bgnPnt.y + y * m_nCos;

			for (int x = 0; x < m_resSiz.width; x++)
			{
				int nIdx_Res = idxCalc_Res.Calc(x, y);

				CvPoint curPnt_X;

				curPnt_X.x = curPnt_Y.x + x * m_nCos;
				curPnt_X.y = curPnt_Y.y + x * m_nSin;

				//resToSrcBuf_X_Scaled[idxCalc_Res.Calc(x, y)] =

				//{
				int nX1, nX2, nY1, nY2;

				nY1 = (curPnt_X.y / m_nScale) * m_nScale;

				nX1 = (curPnt_X.x / m_nScale) * m_nScale;

				if (!(nY1 >= 0 && nY1 < nScaled_SrcHeight))
					goto SrcToResEnd;

				if (!(nX1 >= 0 && nX1 < nScaled_SrcWidth))
					goto SrcToResEnd;

				srcPntArr.ResetSize();

				srcPntArr.PushBack(cvPoint(nX1, nY1));

				nY2 = AddRoundByMin(curPnt_X.y);

				nX2 = AddRoundByMin(curPnt_X.x);

				if (nY2 < 0 || nY2 >= nScaled_SrcHeight)
					nY2 = nY1;

				if (nX2 < 0 || nX2 >= nScaled_SrcWidth)
					nX2 = nX1;

				srcPntArr.PushBack(cvPoint(nX2, nY1));
				srcPntArr.PushBack(cvPoint(nX2, nY2));
				srcPntArr.PushBack(cvPoint(nX1, nY2));

			SrcToResEnd:

				for (int i = 0; i < srcPntArr.GetSize(); i++)
				{
					CvPoint & rSrcPnt = srcPntArr[i];

					int nIdx_Src = idxCalc_Src.Calc(
						rSrcPnt.x / m_nScale, rSrcPnt.y / m_nScale);

					int nOldDist = srcMinDistBuf[nIdx_Src];

					if (nGreatDist == nOldDist)
						nMappedSrcCnt++;

					int nNewDist = abs(curPnt_X.x - rSrcPnt.x) +
						abs(curPnt_X.y - rSrcPnt.y);

					if (nNewDist < nOldDist)
					{
						srcMinDistBuf[nIdx_Src] = nNewDist;
						srcToResBuf[nIdx_Src] = nIdx_Res;
					}

				}
				//}


				bool bInImg = true;

				int nX_Src = AddRound(curPnt_X.x);
				nX_Src /= m_nScale;

				if (!(nX_Src >= 0 && nX_Src < srcSiz.width))
					bInImg = false;

				int nY_Src = AddRound(curPnt_X.y);
				nY_Src /= m_nScale;

				if (!(nY_Src >= 0 && nY_Src < srcSiz.height))
					bInImg = false;

				int nIdx_Src = -1;

				if (bInImg)
				{
					nIdx_Src = idxCalc_Src.Calc(nX_Src, nY_Src);
				}

				resToSrcBuf[nIdx_Res] = nIdx_Src;




				//	PrepareResImg
				//if (bInImg)
				{
					F32ColorVal & rColor_Res = resBuf[nIdx_Res];

					int nIdx_Src = resToSrcBuf[nIdx_Res];


					if (nIdx_Src >= 0)
					{
						//Hcpl_ASSERT(nX1 >= 0);
						//Hcpl_ASSERT(nY1 >= 0);

						//Hcpl_ASSERT(nX2 >= 0);
						//Hcpl_ASSERT(nY2 >= 0);

						F32ColorVal & rColor_Src_X1_Y1 = srcBuf[idxCalc_Src.Calc(nX1 / m_nScale, nY1 / m_nScale)];
						F32ColorVal & rColor_Src_X1_Y2 = srcBuf[idxCalc_Src.Calc(nX1 / m_nScale, nY2 / m_nScale)];
						F32ColorVal & rColor_Src_X2_Y1 = srcBuf[idxCalc_Src.Calc(nX2 / m_nScale, nY1 / m_nScale)];
						F32ColorVal & rColor_Src_X2_Y2 = srcBuf[idxCalc_Src.Calc(nX2 / m_nScale, nY2 / m_nScale)];

						//int nCur_X = (nX1 == nX2) ? nX1 : curPnt_X.x;
						int nWt_X1 = (nX1 == nX2) ? m_nScale : abs(curPnt_X.x - nX2);
						Hcpl_ASSERT(nWt_X1 <= m_nScale);

						F32ColorVal rColor_Src_X_Y1 = F32ColorVal::Add(
							rColor_Src_X1_Y1.MultBy(nWt_X1),
							rColor_Src_X2_Y1.MultBy(m_nScale - nWt_X1)
							).DividBy(m_nScale);

						F32ColorVal rColor_Src_X_Y2 = F32ColorVal::Add(
							rColor_Src_X1_Y2.MultBy(nWt_X1),
							rColor_Src_X2_Y2.MultBy(m_nScale - nWt_X1)
							).DividBy(m_nScale);

						//int nCur_Y = (nY1 == nY2) ? nY1 : curPnt_X.y;
						int nWt_Y1 = (nY1 == nY2) ? m_nScale : abs(curPnt_X.y - nY2);
						Hcpl_ASSERT(nWt_Y1 <= m_nScale);

						rColor_Res = F32ColorVal::Add(
							rColor_Src_X_Y1.MultBy(nWt_Y1),
							rColor_Src_X_Y2.MultBy(m_nScale - nWt_Y1)
							).DividBy(m_nScale);


						//F32ColorVal rColor_Src_X1_Y2 = srcBuf[idxCalc_Src.Calc(nX1, nY2)];

						//F32ColorVal & rColor_Src = srcBuf[nIdx_Src];
						//rColor_Res.AssignVal(rColor_Src);
						
						//rColor_Res.AssignVal(rColor_Src_X1_Y1);
						//rColor_Res.AssignVal(rColor_Src_X2_Y2);
					}
					else
					{
						//rColor_Res.AssignVal( 0, 120, 0 );
						rColor_Res.AssignVal(0, 0, 0);
					}

				}







			}
		}

		int nSrcSiz1D = srcSiz.width * srcSiz.height;

		Hcpl_ASSERT(nMappedSrcCnt == nSrcSiz1D);



		{
			m_lineLimit_H_Arr.SetSize(m_resSiz.height);
			m_lineLimit_V_Arr.SetSize(m_resSiz.width);

			for (int y = 0; y < m_resSiz.height; y++)
			{
				LineLimit & rLimit_H = m_lineLimit_H_Arr[y];

				//rLimit_H.nBgnIdx = 100000000;
				rLimit_H.nBgnIdx = nGreatDist;
				rLimit_H.nEndIdx = -nGreatDist;

				for (int x = 0; x < m_resSiz.width; x++)
				{
					int i = idxCalc_Res.Calc(x, y);

					int nSrcIdx = resToSrcBuf[i];

					if (nSrcIdx < 0)
						continue;

					if (rLimit_H.nEndIdx < 0)
					{
						rLimit_H.nBgnIdx = i;
					}

					rLimit_H.nEndIdx = i;
				}

				if (rLimit_H.nEndIdx < 0)
				{
					rLimit_H.nBgnIdx = m_resSiz.height / 2;
					rLimit_H.nEndIdx = rLimit_H.nBgnIdx - 1;
				}

			}


			for (int x = 0; x < m_resSiz.width; x++)
			{
				LineLimit & rLimit_V = m_lineLimit_V_Arr[x];

				//rLimit_H.nBgnIdx = 100000000;
				rLimit_V.nBgnIdx = nGreatDist;
				rLimit_V.nEndIdx = -nGreatDist;

				for (int y = 0; y < m_resSiz.height; y++)
				{
					int i = idxCalc_Res.Calc(x, y);

					int nSrcIdx = resToSrcBuf[i];

					if (nSrcIdx < 0)
						continue;

					if (rLimit_V.nEndIdx < 0)
					{
						rLimit_V.nBgnIdx = i;
					}

					rLimit_V.nEndIdx = i;
				}

				if (rLimit_V.nEndIdx < 0)
				{
					rLimit_V.nBgnIdx = m_resSiz.width / 2;
					rLimit_V.nEndIdx = rLimit_V.nBgnIdx - m_resSiz.width;
				}

			}


		}

		PrepareImageItrMgr();


		//PrepareResImg();

		//ShowImage( m_resImg, "m_resImg" );

	}


	//void ImgRotationMgr_3::PrepareProvArr()
	void ImgRotationMgr_3::PrepareImageItrMgr()
	{
		m_imageItrMgr = new ImageItrMgr(m_resSiz,
			m_lineLimit_H_Arr, m_lineLimit_V_Arr);
	}



	//void ImgRotationMgr_3::PrepareResImg()
	//{
	//	int * resToSrcBuf = (int *)m_resToSrcMapImg->GetPixAt(0, 0);

	//	m_resImg = F32Image::Create(m_resSiz, 3);

	//	F32ColorVal * resBuf = (F32ColorVal *)m_resImg->GetPixAt(0, 0);

	//	CvSize srcSiz = m_srcImg->GetSize();

	//	//int nSrcSiz1D = srcSiz.width * srcSiz.height;

	//	F32ColorVal * srcBuf = (F32ColorVal *)m_srcImg->GetPixAt(0, 0);

	//	int nResSiz1D = m_resSiz.width * m_resSiz.height;

	//	for (int i = 0; i < nResSiz1D; i++)
	//	{
	//		F32ColorVal & rColor_Res = resBuf[i];


	//		int nIdx_Src = resToSrcBuf[i];

	//		if (nIdx_Src >= 0)
	//		{
	//			F32ColorVal & rColor_Src = srcBuf[nIdx_Src];

	//			rColor_Res.AssignVal(rColor_Src);
	//		}
	//		else
	//		{
	//			//rColor_Res.AssignVal( 0, 120, 0 );
	//			rColor_Res.AssignVal(0, 0, 0);
	//		}
	//	}

	//}



}