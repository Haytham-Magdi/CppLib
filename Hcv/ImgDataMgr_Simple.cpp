#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\ImgDataMgr_Simple.h>

namespace Hcv
{
	using namespace Hcpl;
	//using namespace Hcpl::Math;



	ImgDataMgr_Simple::ImgDataMgr_Simple(F32ImageRef a_srcImg, int a_nDiameter)
	{
		m_srcImg = a_srcImg;

		m_nDiameter = a_nDiameter;


		PrepareAll();
	}


	void ImgDataMgr_Simple::PrepareAll()
	{

		PrepareData_First();

		PrepareData_StandDiv();
	}



	void ImgDataMgr_Simple::PrepareData_StandDiv()
	{
		CvSize srcSiz = m_srcImg->GetSize();
		const int nSrcSiz1D = srcSiz.width * srcSiz.height;





		FixedVector<ImgDataElm_Simple> dataArr_1;
		dataArr_1.SetSize(m_dataArr.GetSize());

		for(int i=0; i < nSrcSiz1D; i++)
			dataArr_1[i].SetZeroVal();

		
		float nDiamSqr = m_nDiameter * m_nDiameter;


		FixedVector<ImgDataElm_Simple> dataArr_Mean;
		dataArr_Mean.SetSize(nSrcSiz1D);

		for(int i=0; i < nSrcSiz1D; i++)
			dataArr_Mean[i].SetZeroVal();

		{
			PrepareData_First_H(m_nDiameter, m_dataArr, dataArr_1);

			for(int i=0; i < nSrcSiz1D; i++)
			{
				//dataArr_1[i].DividSelfBy(m_nDiameter);
			}

			PrepareData_First_V(m_nDiameter, dataArr_1, dataArr_Mean);

			for(int i=0; i < nSrcSiz1D; i++)
			{
				dataArr_Mean[i].DividSelfBy(nDiamSqr);
				//dataArr_Mean[i].DividSelfBy(m_nDiameter);
			}
		}



		m_img_StandDiv_Mag = F32Image::Create( srcSiz, 1);

		float * imgBuff = m_img_StandDiv_Mag->GetPixAt(0, 0);
		

		for(int i=0; i < nSrcSiz1D; i++)
		{
			imgBuff[i] = m_dataArr[i].CalcMagSqr();
		}


		for(int i=0; i < nSrcSiz1D; i++)
		{
			if( imgBuff[i] < 0)
				i = i;
		}




		m_img_StandDiv_Mag = GenFastAvgImg(m_img_StandDiv_Mag, m_nDiameter);

		imgBuff = m_img_StandDiv_Mag->GetPixAt(0, 0);

		F32ImageRef errorImg = F32Image::Create( srcSiz, 1);
		float * errorBuf = errorImg->GetPixAt(0, 0);

		for(int i=0; i < nSrcSiz1D; i++)
		{
			errorBuf[i] = 0;
						
			if( imgBuff[i] < -1)
			{
				i = i;

				errorBuf[i] = 128;
			}
		}

		//ShowImage(m_img_StandDiv_Mag, "m_img_StandDiv_Mag_0");
		
		//ShowImage(errorImg, "errorImg");



		int x = 0;
		int y = 0;


		for(int i=0; i < nSrcSiz1D; i++)
		{
			//float selfSqr = dataArr_Mean[i].CalcMagSqr();

			float meanSqr = dataArr_Mean[i].CalcMagSqr();

			float locVar = imgBuff[i];

			float val = locVar - meanSqr;

			if( val < 0 )
			//if( val < -1 )
				//val = val;
				val = 0;

			//val *= 10;

			val = sqrt((float)val);

			imgBuff[i] = val;

			x = (x + 1) % srcSiz.width;

			if( 0 == x)
				y = (y + 1) % srcSiz.height;



		}


		//GenMeanImg();

		//ShowImage(m_img_StandDiv_Mag, "m_img_StandDiv_Mag");
		GlobalStuff::SetLinePathImg(m_img_StandDiv_Mag);
		GlobalStuff::ShowLinePathImg();
		//ShowImage(m_img_StandDiv_Mag, "LinePathImg");

		//for(int i=0; i < nSrcSiz1D; i++)
		//{
		//	dataArr_1[i].DividSelfBy(m_nDiameter);
		//	m_dataArr[i].Copy(dataArr_1[i]);
		//}


	}

	
	F32ImageRef ImgDataMgr_Simple::GenMeanImg()
	{
		return NULL;

		CvSize srcSiz = m_srcImg->GetSize();
		const int nSrcSiz1D = srcSiz.width * srcSiz.height;

		F32ImageRef retImg = F32Image::Create( srcSiz, 3);

		F32ColorVal * retBuff = (F32ColorVal *)retImg->GetPixAt(0, 0);

		for(int i=0; i < nSrcSiz1D; i++)
		{
			retBuff[i] = m_dataArr[i].Color;
		}

		ShowImage(retImg, "m_img_StandDiv_Mag");

		return retImg;
	}




	void ImgDataMgr_Simple::PrepareData_First()
	{
		//ShowImage(m_srcImg, "m_srcImg");

		CvSize srcSiz = m_srcImg->GetSize();
		const int nSrcSiz1D = srcSiz.width * srcSiz.height;

		m_dataArr.SetSize(nSrcSiz1D);

		F32ColorVal * srcBuff = (F32ColorVal *)m_srcImg->GetPixAt(0, 0);

		for(int i=0; i < nSrcSiz1D; i++)
		{
			m_dataArr[i].Color = srcBuff[i];
			//m_dataArr[i].SetZeroVal();
		}

		
		FixedVector<ImgDataElm_Simple> dataArr_1;
		dataArr_1.SetSize(m_dataArr.GetSize());

		PrepareData_First_H(m_nDiameter, m_dataArr, dataArr_1);

		PrepareData_First_V(m_nDiameter, dataArr_1, m_dataArr);

		float nDiamSqr = m_nDiameter * m_nDiameter;

		for(int i=0; i < nSrcSiz1D; i++)
		{
			m_dataArr[i].DividSelfBy(nDiamSqr);
		}

		//for(int i=0; i < nSrcSiz1D; i++)
		//{
		//	dataArr_1[i].DividSelfBy(m_nDiameter);
		//	m_dataArr[i].Copy(dataArr_1[i]);
		//}


	}



	void ImgDataMgr_Simple::ZeroBorder(FixedVector<ImgDataElm_Simple> & a_dataArr,
		int a_nMargX, int a_nMargY)
	{
		CvSize srcSiz = m_srcImg->GetSize();

		IndexCalc2D idx_Calc(srcSiz.width, srcSiz.height);

			

		int x, y;

		//const int nIdx_Test = 53718;
		const int nIdx_Test = 386640;


		for(y=0; y < a_nMargY; y++)
			for(x=0; x < srcSiz.width; x++)
			{
				int nIdx = idx_Calc.Calc(x, y);

				if(nIdx_Test == nIdx)
					nIdx = nIdx;

				a_dataArr[ nIdx ].SetZeroVal();
			}

		for(y = srcSiz.height-1-a_nMargY; y < srcSiz.height; y++)
			for(x=0; x < srcSiz.width; x++)
			{
				int nIdx = idx_Calc.Calc(x, y);

				if(nIdx_Test == nIdx)
					nIdx = nIdx;

				a_dataArr[ nIdx ].SetZeroVal();
			}

		for(x=0; x < a_nMargX; x++)
			for(y=0; y < srcSiz.height; y++)
			{
				int nIdx = idx_Calc.Calc(x, y);

				if(nIdx_Test == nIdx)
					nIdx = nIdx;

				a_dataArr[ nIdx ].SetZeroVal();
			}


		for(x = srcSiz.width - 1 - a_nMargX; x < srcSiz.width; x++)
			for(y=0; y < srcSiz.height; y++)
			{
				int nIdx = idx_Calc.Calc(x, y);

				if(nIdx_Test == nIdx)
					nIdx = nIdx;

				a_dataArr[ nIdx ].SetZeroVal();
			}


	}



	void ImgDataMgr_Simple::PrepareData_First_H(int a_nAprSiz,
			FixedVector<ImgDataElm_Simple> & a_srcArr,
			FixedVector<ImgDataElm_Simple> & a_destArr)
	{

		Hcpl::Debug::Assert( 1 == a_nAprSiz % 2 );

		F32ImageRef ret;

		const int nMarg = a_nAprSiz / 2;

		CvSize retSiz = m_srcImg->GetSize();

		IndexCalc2D idx_Calc(retSiz.width, retSiz.height);

		//ret = F32Image::Create(retSiz, a_src->GetNofChannels());

		const int nBgnX = nMarg + 1;
		const int nLimX = retSiz.width - nMarg;

		const int nBgnY = 0;
		const int nLimY = retSiz.height;

		//for(int nCh=0; nCh < a_src->GetNofChannels(); nCh++)
		{
			//F32ChannelRef srcCh = a_src->GetAt(nCh);
			//F32ChannelRef retCh = ret->GetAt(nCh);

			//FillBorder(retCh, (float)0, nMarg, nMarg, 0, 0);

			int nMarg = a_nAprSiz / 2;

			ZeroBorder(a_destArr, nMarg, nMarg);

			for(int y = nBgnY; y < nLimY; y++)
			{
				//float sum = 0;

				ImgDataElm_Simple sumElm;

				sumElm.SetZeroVal();

				for(int nSrcX=0; nSrcX < a_nAprSiz; nSrcX++)
				{
					//sum += srcCh->GetAt( nSrcX, y );

					sumElm.IncBy( a_srcArr[ idx_Calc.Calc( nSrcX, y ) ] );
				}
				//retCh->SetAt( nMarg, y, sum / a_nAprSiz );
				a_destArr[ idx_Calc.Calc( nMarg, y ) ].Copy(sumElm);

				int nSrcXSub = 0;
				int nSrcXAdd = a_nAprSiz;

				for(int x = nBgnX; x < nLimX; x++, nSrcXSub++, nSrcXAdd++)
				{
					//sum -= srcCh->GetAt( nSrcXSub, y);
					//sum += srcCh->GetAt( nSrcXAdd, y);

					sumElm.DecBy( a_srcArr[ idx_Calc.Calc( nSrcXSub, y ) ] );
					sumElm.IncBy( a_srcArr[ idx_Calc.Calc( nSrcXAdd, y ) ] );

					//retCh->SetAt( x, y, sum / a_nAprSiz );
					a_destArr[ idx_Calc.Calc( x, y ) ].Copy(sumElm);
				}
			}
		}


	}



	void ImgDataMgr_Simple::PrepareData_First_V(int a_nAprSiz,
			FixedVector<ImgDataElm_Simple> & a_srcArr,
			FixedVector<ImgDataElm_Simple> & a_destArr)
	{
		Hcpl::Debug::Assert( 1 == a_nAprSiz % 2 );

		F32ImageRef ret;

		const int nMarg = a_nAprSiz / 2;

		//CvSize retSiz = cvSize( srcSiz.width, srcSiz.height);
		CvSize retSiz = m_srcImg->GetSize();

		IndexCalc2D idx_Calc(retSiz.width, retSiz.height);


		//ret = F32Image::Create(retSiz, a_src->GetNofChannels());

		const int nBgnY = nMarg + 1;
		const int nLimY = retSiz.height - nMarg;

		const int nBgnX = 0;
		const int nLimX = retSiz.width;

		//for(int nCh=0; nCh < a_src->GetNofChannels(); nCh++)
		{
			//F32ChannelRef srcCh = a_src->GetAt(nCh);
			//F32ChannelRef retCh = ret->GetAt(nCh);

			//FillBorder(retCh, (float)0, 0, 0, nMarg, nMarg );

			int nMarg = a_nAprSiz / 2;

			ZeroBorder(a_destArr, nMarg, nMarg);

			for(int x = nBgnX; x < nLimX; x++)
			{
				//float sum = 0;

				ImgDataElm_Simple sumElm;

				sumElm.SetZeroVal();


				for(int nSrcY=0; nSrcY < a_nAprSiz; nSrcY++)
				{
					//sum += srcCh->GetAt( x, nSrcY );
				
					sumElm.IncBy( a_srcArr[ idx_Calc.Calc( x, nSrcY ) ] );
				}
				//retCh->SetAt( x, nMarg, sum / a_nAprSiz );
				a_destArr[ idx_Calc.Calc( x, nMarg ) ].Copy(sumElm);

				int nSrcYSub = 0;
				int nSrcYAdd = a_nAprSiz;

				for(int y = nBgnY; y < nLimY; y++, nSrcYSub++, nSrcYAdd++)
				{
					//sum -= srcCh->GetAt( x, nSrcYSub);
					//sum += srcCh->GetAt( x, nSrcYAdd);

					sumElm.DecBy( a_srcArr[ idx_Calc.Calc( x, nSrcYSub ) ] );
					sumElm.IncBy( a_srcArr[ idx_Calc.Calc( x, nSrcYAdd ) ] );

					//retCh->SetAt( x, y, sum / a_nAprSiz );
					a_destArr[ idx_Calc.Calc( x, y ) ].Copy(sumElm);
				}
			}
		}



	}


	float ImgDataMgr_Simple::CalcDif(int a_nIdx_1, int a_nIdx_2)
	{
		//F32ColorVal * pC1 = &m_dataArr[a_nIdx_1].Color;

		//F32ColorVal * pC2 = &m_dataArr[a_nIdx_2].Color;

		return ImgDataElm_Simple::CalcDif(
			m_dataArr[a_nIdx_1], m_dataArr[a_nIdx_2]);

		//return F32ColorVal::Sub(*pC1, *pC2).CalcMag();
	}


	float ImgDataMgr_Simple::Calc_UnitDif(int a_nIdx_1, int a_nIdx_2)
	{
		return ImgDataElm_Simple::Calc_UnitDif(
			m_dataArr[a_nIdx_1], m_dataArr[a_nIdx_2]);
	}





}