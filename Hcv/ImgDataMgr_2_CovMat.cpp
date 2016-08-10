#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\ImgDataMgr_2_CovMat.h>

namespace Hcv
{
	using namespace Hcpl;
	//using namespace Hcpl::Math;


	IImgDataMgr_2 * ImgDataMgr_2_CovMat_Factory::CreateImgDataMgr( 
			F32ImageRef a_srcImg, int a_nAprSiz,
			IImgCoreSrcRef a_dataSrc, ImgRotationMgrRef a_rot )
	{
		IImgDataMgr_2 * pIdm = new ImgDataMgr_2_CovMat(
			a_srcImg, a_nAprSiz );

		return pIdm;		
	}


	ImgDataMgr_2_CovMat::ImgDataMgr_2_CovMat( 
		F32ImageRef a_srcImg, int a_nAprSiz )
	{
		m_srcImg = a_srcImg;
		m_nAprSiz = a_nAprSiz;

		PrepareData();
	}

	
	void ImgDataMgr_2_CovMat::PrepareData()
	{
		//ThrowHcvException();

		CvSize srcSiz = m_srcImg->GetSize();
		const int nSrcSiz1D = srcSiz.width * srcSiz.height;

		m_dataArr.SetSize(nSrcSiz1D);


		F32ColorVal * srcBuff = (F32ColorVal *)m_srcImg->GetPixAt(0, 0);

		for(int i=0; i < nSrcSiz1D; i++)
		{
			F32ColorVal & c1 = srcBuff[i];

			m_dataArr[i].SetFrom_F32ColorVal( c1 );
		}

		
		FixedVector<ImgDataElm_CovMat> dataArr_1;
		dataArr_1.SetSize(m_dataArr.GetSize());

		PrepareData_First_H( m_nAprSiz, m_srcImg->GetSize(),
			m_dataArr, dataArr_1);

		PrepareData_First_V( m_nAprSiz, m_srcImg->GetSize(),
			dataArr_1, m_dataArr);

		float nDiamSqr = m_nAprSiz * m_nAprSiz;

		for(int i=0; i < nSrcSiz1D; i++)
		{
			m_dataArr[i].DividSelfBy(nDiamSqr);
			m_dataArr[i].PrepareStanDev();
		}


		ImgDataElm_CovMat * pElm = & m_dataArr[ 0 ];

		int nElmSiz = sizeof( ImgDataElm_CovMat );
		
	}



	void ImgDataMgr_2_CovMat::PrepareData_First_H(
		int a_nAprSiz, CvSize a_imgSiz,
			FixedVector<ImgDataElm_CovMat> & a_srcArr,
			FixedVector<ImgDataElm_CovMat> & a_destArr)
	{

		Hcpl::Debug::Assert( 1 == a_nAprSiz % 2 );

		F32ImageRef ret;

		const int nMarg = a_nAprSiz / 2;

		CvSize retSiz = a_imgSiz;

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

			ZeroBorder(a_destArr, a_imgSiz, nMarg, nMarg);

			for(int y = nBgnY; y < nLimY; y++)
			{
				//float sum = 0;

				ImgDataElm_CovMat sumElm;

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



			{
				int x, y;

				for(x = 0; x < nMarg; x++)
				{
					int nSrcX = nMarg;

					for(y = 0; y < retSiz.height; y++)
						a_destArr[ idx_Calc.Calc(x, y) ] = 
							a_destArr[ idx_Calc.Calc(nSrcX, y) ];
				}
				
				for(x = retSiz.width - nMarg; x < retSiz.width; x++)
				{
					int nSrcX = retSiz.width - nMarg - 1;

					for(y = 0; y < retSiz.height; y++)
						a_destArr[ idx_Calc.Calc(x, y) ] =
							a_destArr[ idx_Calc.Calc(nSrcX, y) ];
				}

			}




		}


	}



	void ImgDataMgr_2_CovMat::PrepareData_First_V(
			int a_nAprSiz, CvSize a_imgSiz,
			FixedVector<ImgDataElm_CovMat> & a_srcArr,
			FixedVector<ImgDataElm_CovMat> & a_destArr)
	{
		Hcpl::Debug::Assert( 1 == a_nAprSiz % 2 );

		F32ImageRef ret;

		const int nMarg = a_nAprSiz / 2;

		//CvSize retSiz = cvSize( srcSiz.width, srcSiz.height);
		CvSize retSiz = a_imgSiz;

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

			ZeroBorder(a_destArr, a_imgSiz,
				nMarg, nMarg);

			for(int x = nBgnX; x < nLimX; x++)
			{
				//float sum = 0;

				ImgDataElm_CovMat sumElm;

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


			{
				int x, y;

				for(y = 0; y < nMarg; y++)		
				{
					int nSrcY = nMarg;

					for(x = 0; x < retSiz.width; x++)
						a_destArr[ idx_Calc.Calc(x, y) ] =   
							a_destArr[ idx_Calc.Calc(x, nSrcY) ];
				}

				for(y = retSiz.height - nMarg; y < retSiz.height; y++)
				{
					int nSrcY = retSiz.height - nMarg - 1;

					for(x = 0; x < retSiz.width; x++)
						a_destArr[ idx_Calc.Calc(x, y) ] =  
							a_destArr[ idx_Calc.Calc(x, nSrcY) ];
				}


			}


		}

	}




	void ImgDataMgr_2_CovMat::ZeroBorder(
		FixedVector<ImgDataElm_CovMat> & a_dataArr,
		CvSize a_imgSiz,
		int a_nMargX, int a_nMargY)
	{
		CvSize srcSiz = a_imgSiz;

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





}