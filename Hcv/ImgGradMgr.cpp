#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\ImgGradMgr.h>

namespace Hcv
{
	using namespace Hcpl;
	//using namespace Hcpl::Math;
	
	using namespace Ns_ImgGradMgr;

	ImgGradMgr::ImgGradMgr( F32ImageRef a_src, const int a_nAprSiz, const int a_nofDirs ) :
		m_fwdBwdPntArr( NULL, a_src->GetSize().width, a_src->GetSize().height ),
			m_bFwdBkdDone( false )
	{
		m_src = a_src;

		m_gradMagImg = NULL;

		m_nofDirs = a_nofDirs;

		Hcpl_ASSERT( 0 == a_nofDirs % 4 );

		m_nAprSiz = a_nAprSiz;



		Proceed();
	}


	F32ImageRef ImgGradMgr::GetGradImgOfDir( const int a_nDir )
	{
		CvSize srcSiz = m_src->GetSize();

		const int nSrcSiz1D = srcSiz.width * srcSiz.height;

		F32ImageRef gradImg = F32Image::Create( srcSiz, 1 );
		float * gradImgBuf = gradImg->GetPixAt( 0, 0 );

		for( int i=0; i < nSrcSiz1D; i++ )
		{
			float * gradValColl = &m_gradDataArr[ i * m_nofDirs ];

			float val1 = gradValColl[ a_nDir ];

			gradImgBuf[ i ] = val1;
		}

		return gradImg;
	}



	void ImgGradMgr::Proceed()
	{
		CvSize srcSiz = m_src->GetSize();

		const int nSrcSiz1D = srcSiz.width * srcSiz.height;
		m_gradDataArr.SetSize( m_nofDirs * nSrcSiz1D );

		m_sdmColl = new ScanDirMgrColl( m_nofDirs, srcSiz );

		for( int i=0; i < m_sdmColl->GetNofMgrs(); i++ )
		{
			ScanDirMgr & rScanMgr = m_sdmColl->GetMgrAt( i );

			SubGradMgr::Process( rScanMgr, m_src, 		 
				m_nAprSiz, &m_gradDataArr[ i ], m_nofDirs );
		}

	}


	FixedVector< bool > & ImgGradMgr::GetIsPeakDataArr()
	{
		if( 0 == m_isPeakDataArr.GetSize() )
		{
			m_isPeakDataArr.SetSize( m_gradDataArr.GetSize() );

			for( int i=0; i < m_sdmColl->GetNofMgrs(); i++ )
			{
				//PrepareDirPeaks( i, m_nAprSiz, m_nofDirs );
				PrepareDirPeaks( i, m_nAprSiz * 4 + 1, m_nofDirs );
			}
		}

		return m_isPeakDataArr;
	}

	F32ImageRef ImgGradMgr::GenIsPeakImg()
	{
		GetIsPeakDataArr();

		CvSize srcSiz = m_src->GetSize();			
		const int nSrcSiz1D = srcSiz.width * srcSiz.height;

		F32ImageRef retImg = F32Image::Create( srcSiz, 3 );

		F32ColorVal * retColorBuf = (F32ColorVal *)retImg->GetPixAt( 0, 0 );

		Hcpl::Int32 * dirBuf = this->GetGradDirImg()->GetPixAt( 0, 0 );

		for( int i=0; i < nSrcSiz1D; i++ )
		{
			const int nDirC = dirBuf[ i ];

			const bool bVal = m_isPeakDataArr[ i * m_nofDirs + nDirC ];

			if( bVal )
				retColorBuf[ i ].AssignVal( 255, 255, 255 );
			else
				retColorBuf[ i ].AssignVal( 0, 0, 0 );
		}

		return retImg;
	}

	void ImgGradMgr::PrepareDirPeaks( int a_nDir, int a_nAprSiz, int a_nStepCnt )
	{
		GetGradDataArr();

		for( int k=0; k < m_gradDataArr.GetSize(); k++ )
		{
			//if( 0 != m_gradDataArr[ k ] )
			if( m_gradDataArr[ k ] > 5 )
				k = k;
		}

		ScanDirMgr & a_rScanMgr = m_sdmColl->GetMgrAt( a_nDir );

		float * inpHeadA = & m_gradDataArr[ a_nDir ];

		bool * retHeadA = & m_isPeakDataArr[ a_nDir ];

		FixedVector< int > & rShiftArr = a_rScanMgr.GetShiftIndexArr();

		FixedVector< int > & rBgnArr = a_rScanMgr.GetBgnIndexArr();

		FixedVector< int > & rStartArr = a_rScanMgr.GetShiftStartArr();

		FixedVector< int > & rLimArr = a_rScanMgr.GetLimArr();

		const int nofArrs = rBgnArr.GetSize();

		const int nMargOrg = a_nAprSiz / 2;

		int nMarg = -1;
		{
			F32Point dirPnt = a_rScanMgr.GetMainDirPnt();

			float distAct = dirPnt.CalcMag();

			float ratio = rShiftArr.GetSize() / distAct;

			nMarg = (int) ( ( ratio * nMargOrg ) + 0.5555555 );
		}

		const int nAprSizA = 2 * nMarg + 1;


		//const F32Point pntTest( 406, 89 );
		const F32Point pntTest( 430, 87 );
		
		CvSize srcSiz = m_src->GetSize();
		IndexCalc2D idx2D( srcSiz.width, srcSiz.height );

		const int nTestIdx = idx2D.Calc( pntTest.x, pntTest.y );



		for( int i=0; i < nofArrs; i++ )
		{
			const int nBPI = rBgnArr[ i ];
			const int nSI = rStartArr[ i ];
			const int nLimI = rLimArr[ i ];

			if( 430 == nBPI && 4 == a_nDir )
				i = i;

			{
				const int nMarg2 = nSI + nMarg;

				const int nLim2 = ( nMarg2 < nLimI ) ? nMarg2 : nLimI;

				for( int j = nSI; j < nLim2; j++ )
				{
					const int nCPI = nBPI + rShiftArr[ j ];

					retHeadA[ nCPI * a_nStepCnt ] = false;
				}
			}

			{
				int nBgn2 = nLimI - nMarg;

				if( nBgn2 < nSI )
					nBgn2 = nSI;

				for( int j = nBgn2; j < nLimI; j++ )
				{
					const int nCPI = nBPI + rShiftArr[ j ];

					retHeadA[ nCPI * a_nStepCnt ] = false;
				}
			}


			Hcpl::Int32 * dirBuf = this->GetGradDirImg()->GetPixAt( 0, 0 );


			for( int j = nSI + nMarg; j < nLimI - nMarg; j++ )
			{
				const int nCPIBef = nBPI + rShiftArr[ j - nMarg ];
				const int nCPI = nBPI + rShiftArr[ j ];
				const int nCPIAft = nBPI + rShiftArr[ j + nMarg ];

/*
				if( 4 != a_nDir )
				{
					retHeadA[ nCPI * a_nStepCnt ] = false;
					continue;
				}
*/

				if( nTestIdx == nCPI )
					j = j;

				const int nDirC = dirBuf[ nCPI ];

				if( nDirC != a_nDir )
				{
					retHeadA[ nCPI * a_nStepCnt ] = false;
					continue;
				}


				//const float valBef = inpHeadA[ ( j - nMarg ) * a_nStepCnt ];
				//const float valBef = inpHeadA[ ( nCPI - nMarg ) * a_nStepCnt ];				
				const float valBef = inpHeadA[ nCPIBef * a_nStepCnt ];				
				const float valCur = inpHeadA[ nCPI * a_nStepCnt ];
				const float valAft = inpHeadA[ nCPIAft * a_nStepCnt ];

				const float difBef = valCur - valBef;
				const float difAft = valCur - valAft;
				const float difMin = ( difBef < difAft ) ? difBef : difAft;

				//if( difMin < 7 )
				if( difMin < 2 )
				{
					retHeadA[ nCPI * a_nStepCnt ] = false;
					continue;
				}

				if( 0 != valBef ||
					0 != valCur ||
					0 != valAft )
					j = j;

				if( valCur < valBef || valCur < valAft )
				{
					retHeadA[ nCPI * a_nStepCnt ] = false;
					continue;
				}

				//if( valCur == valBef && valCur == valAft )
				if( valCur == valBef || valCur == valAft )
				{
					if( 0 != valCur )
						j = j;

					retHeadA[ nCPI * a_nStepCnt ] = false;
					continue;
				}
				else
					j = j;

				//( valCur > valBef || valCur > valAft )
				retHeadA[ nCPI * a_nStepCnt ] = true;

			}

		}

	}







	F32ImageRef ImgGradMgr:: GetGradMagImg()
	{
		if( NULL == m_gradMagImg )
		{
			PrepareTotGrad();

/*

			CvSize srcSiz = m_src->GetSize();
			const int nSrcSiz1D = srcSiz.width * srcSiz.height;

			m_gradMagImg = F32Image::Create( srcSiz, 1 );

			float * magBuf = m_gradMagImg->GetPixAt( 0, 0 );

			for( int i=0; i < nSrcSiz1D; i++ )
			{
				float * gradValColl = &m_gradDataArr[ i * m_nofDirs ];

				float maxVal = 0;

				for( int j=0; j < m_nofDirs; j++ )
				{
					float val = gradValColl[ j ];

					if( val > maxVal )
						maxVal = val;
				}

				magBuf[ i ] = maxVal;
			}

*/


		}

		return m_gradMagImg;
	}



	F32ImageRef ImgGradMgr::GetGradAngleImg()
	{
		if( NULL == m_gradAngleImg )
		{
			PrepareTotGrad();
		}

		return m_gradAngleImg;
	}


	S32ImageRef ImgGradMgr::GetGradDirImg()
	{
		if( NULL == m_gradDirImg )
		{
			PrepareTotGrad();
		}

		return m_gradDirImg;
	}


	void ImgGradMgr::PrepareTotGrad()
	{

		CvSize srcSiz = m_src->GetSize();
		const int nSrcSiz1D = srcSiz.width * srcSiz.height;

		m_gradMagImg = F32Image::Create( srcSiz, 1 );
		float * magBuf = m_gradMagImg->GetPixAt( 0, 0 );

		m_gradAngleImg = F32Image::Create( srcSiz, 1 );
		float * anfgleBuf = m_gradAngleImg->GetPixAt( 0, 0 );

		m_gradDirImg = S32Image::Create( srcSiz, 1 );
		Hcpl::Int32 * dirBuf = m_gradDirImg->GetPixAt( 0, 0 );


		FixedVector< float > subGradArr2;
		subGradArr2.SetSize( m_nofDirs );

		const int valSiz = m_nofDirs;
		const int valHahfSiz = valSiz / 2;
		const int valQtrSiz = valSiz / 4;


		FixedVector< float > radArr;
		radArr.SetSize( m_nofDirs * 2 );
		{
			for( int i=0; i < m_sdmColl->GetNofMgrs(); i++ )
			{
				int j = i + m_sdmColl->GetNofMgrs();

				float val = m_sdmColl->GetMgrAt( i ).GetAngleByRad();

				radArr[ i ] = val;
				radArr[ j ] = val;
			}

		}


		IndexCalc2D index2D( srcSiz.width, srcSiz.height );
		//F32Point testPnt( 125, 213 );
		//F32Point testPnt( 124, 220 );
		//F32Point testPnt( 263, 285 );
		F32Point testPnt( 575, 344 );
		const int nTestIdx = index2D.Calc( testPnt.x, testPnt.y );

		for( int i=0; i < nSrcSiz1D; i++ )
		{
			if( nTestIdx == i )
				i = i;

			float * gradValColl = &m_gradDataArr[ i * m_nofDirs ];

			float maxVal = 0;

			int nMaxIdx = 0;

			// Cross elemination
			{

				//for( int i=0; i < valQtrSiz; i++ )
				for( int i=0; i < valHahfSiz; i++ )
				{
					//int k = i + valQtrSiz;
					int k = i + valHahfSiz;
					
					float valI = gradValColl[ i ];
					float valK = gradValColl[ k ];

					if( valI > maxVal )
						maxVal = valI;

					if( valK > maxVal )
						maxVal = valK;

					float valMin = ( valI < valK ) ? valI : valK;



					//int nMaxIK = ( valI > valK ) ? i : k;

					//subGradArr2[ i ] = valI - valMin;
					//subGradArr2[ k ] = valK - valMin;

					valI -= valMin;
					valK -= valMin;

					int nMaxIK = ( valI > valK ) ? i : k;

					subGradArr2[ i ] = valI;
					subGradArr2[ k ] = valK;

					if( subGradArr2[ nMaxIK ] > subGradArr2[ nMaxIdx ] )
						nMaxIdx = nMaxIK;
				}

				//for( int i = valHahfSiz; i < valSiz; i++ )
				//{
				//	subGradArr2[ i ] = subGradArr2[ i - valHahfSiz ];
				//}


			}



			
			int nCenterIdx;			
			float centerRad;

			// Circ center of mass
			{
				int nBgn = nMaxIdx + valSiz - valQtrSiz;
				int nLim = nMaxIdx + valSiz + valQtrSiz;

				float valSum = 0;
				float valByIdxSum = 0;
				float radByValSum = 0;

				float oldRad1 = -1;
				float oldRad2 = -1;

				for( int i = nBgn; i < nLim; i++ )
				{
					const int nActI = i % valSiz;

					float val = subGradArr2[ nActI ];
					
					float rad = radArr[ nActI ];


					//if( nActI < i )
						//rad += 2 * M_PI;

					if( rad < oldRad1 )
						rad += 2 * M_PI;
						//rad = rad;

					oldRad2 = oldRad1;
					oldRad1 = rad;

					valSum += val;
					valByIdxSum += i * val;
					radByValSum  += rad * val;
				}

				if( valSum > 0.01 )
				{
					nCenterIdx = (int) ( ( valByIdxSum / valSum ) + 0.55555 );
					centerRad = radByValSum / valSum;
				}
				else
				{
					nCenterIdx = 0;
					centerRad = 0;
				}

				nCenterIdx = nCenterIdx % valSiz;
				//int nCentOpIdx = ( nCenterIdx + valHahfSiz ) % valSiz;


				//for( int i=0; i < valSiz; i++ )
					//m_valArr[ i ] = 0;

				//m_valArr[ nCenterIdx ] = m_valArr[ nCentOpIdx ] = 130;
			}

			{
				float magVal = subGradArr2[ nCenterIdx ];
				//float magVal = subGradArr2[ nMaxIdx ];			

				if(	magVal < maxVal * 0.5 && maxVal > 100 )
					magVal = magVal;

				magBuf[ i ] = magVal;
				//magBuf[ i ] = maxVal;
			}

			anfgleBuf[ i ] = centerRad;

			dirBuf[ i ] = nCenterIdx;
		}

	}



	Accessor2D< ImgGradMgr::FwdBkdPoints > & ImgGradMgr::GetFwdBkdMatrix()
	{
		if( ! m_bFwdBkdDone )
		{
			CvSize srcSiz = m_src->GetSize();
			const int nSrcSiz1D = srcSiz.width * srcSiz.height;

			Hcpl::Int32 * dirBuf = GetGradDirImg()->GetPixAt( 0, 0 );

			FwdBkdPoints * fbPntBuf = m_fwdBwdPntArr.GetPtrAt( 0, 0 );


			IndexCalc2D index2D( srcSiz.width, srcSiz.height );

			//F32Point testPnt( 125, 213 );
			F32Point testPnt( 124, 220 );
			const int nTestIdx = index2D.Calc( testPnt.x, testPnt.y );

			for( int i=0; i < nSrcSiz1D; i++ )
			{
				if( nTestIdx == i )
					i = i;



				F32Point srcPnt = GetPntOfIndex( i, srcSiz );

				ImgGradMgr::FwdBkdPoints & fbPnts = fbPntBuf[ i ];

				int nDir = dirBuf[ i ];

				ScanDirMgr & rScanMgr = m_sdmColl->GetMgrAt( nDir );

				float rad1 = rScanMgr.GetAngleByRad();

				F32Point difPnt = rScanMgr.GetDifPnt();

				fbPnts.FwdPnt = F32Point::Add( srcPnt, difPnt );
				fbPnts.BkdPnt = F32Point::Sub( srcPnt, difPnt );
			}


			m_bFwdBkdDone = true;
		}

		return m_fwdBwdPntArr;
	}



	F32Point ImgGradMgr::GetPntOfIndex( int a_nIndex, CvSize a_siz )
	{
		F32Point retPnt;

		retPnt.x = a_nIndex % a_siz.width;
		retPnt.y = a_nIndex / a_siz.width;

		return retPnt;
	}



//yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy





	ScanLineItrRef ImgGradMgr::GenScanLineItr( F32Point a_pnt, int a_nDir )
	{
		CvSize srcSiz = m_src->GetSize();

		IndexCalc2D idx2D( srcSiz.width, srcSiz.height );

		const int nPI = idx2D.Calc( a_pnt.x, a_pnt.y );

		return GenScanLineItr( nPI, a_nDir );
	}

	ScanLineItrRef ImgGradMgr::GenScanLineItr( int a_nPI, int a_nDir )
	{
		ScanLineItrRef sli = new ScanLineItr();
		
		PrepareScanLineItr( sli, a_nPI, a_nDir );

		return sli;
	}

	void ImgGradMgr::PrepareScanLineItr( 
		ScanLineItrRef a_sli, int a_nPI, int a_nDir )
	{
		ScanLineItrRef sli = a_sli;

		ScanDirMgr & rSm = m_sdmColl->GetMgrAt( a_nDir );

		ScanDirMgr::PixLineInfo & rPli = rSm.GetPixLineInfoArr()[ a_nPI ];

		sli->m_srcSiz = m_src->GetSize();

		//sli->m_nBPI = rSm.GetBgnIndexArr()[ rPli.nLineNum ];

		sli->m_nLineNum = rPli.nLineNum;

		//sli->m_pIOA_Head = rSm.Get_ioaHeadArr()[ rPli.nLineNum ];
		sli->m_pIOA_Head = rSm.Get_ioaHeadAt( rPli.nLineNum );

		//sli->m_nIOA_Size = rSm.Get_ioaSizeArr()[ rPli.nLineNum ];
		sli->m_nIOA_Size = rSm.Get_ioaSizeAt( rPli.nLineNum );

		//sli->m_nSI = rSm.GetShiftStartArr()[ rPli.nLineNum ];
		//sli->m_nLimI = rSm.GetLimArr()[ rPli.nLineNum ];
		//sli->m_nEI = rSm.GetLimArr()[ rPli.nLineNum ] - 1;

		//sli->m_pShiftArr = &rSm.GetShiftIndexArr();

		//sli->m_nCurI = rPli.nIOL;
		sli->m_nCurI = 0;

		sli->m_nCPI = sli->m_pIOA_Head[ sli->m_nCurI ];

		sli->m_nDir = a_nDir;
	}

	

	ScanLineItrRef ImgGradMgr::GenScanLineItr_FromLine( 
		int a_nLineNum, int a_nDir, int a_nCurI )
	{
		ScanLineItrRef sli = new ScanLineItr();

		sli->m_nLineNum = a_nLineNum;

		ScanDirMgr & rSm = m_sdmColl->GetMgrAt( a_nDir );

		//ScanDirMgr::PixLineInfo & rPli = rSm.GetPixLineInfoArr()[ a_nPI ];

		sli->m_srcSiz = m_src->GetSize();

		//sli->m_nBPI = rSm.GetBgnIndexArr()[ a_nLineNum ];

		//sli->m_pIOA_Head = rSm.Get_ioaHeadArr()[ a_nLineNum ];
		sli->m_pIOA_Head = rSm.Get_ioaHeadAt( a_nLineNum );

		//sli->m_nIOA_Size = rSm.Get_ioaSizeArr()[ a_nLineNum ];
		sli->m_nIOA_Size = rSm.Get_ioaSizeAt( a_nLineNum );


		//sli->m_nSI = rSm.GetShiftStartArr()[ a_nLineNum ];

		//sli->m_nEI = rSm.GetLimArr()[ a_nLineNum ] - 1;

		//sli->m_pShiftArr = &rSm.GetShiftIndexArr();

		sli->m_nCurI = a_nCurI;

		sli->m_nCPI = sli->m_pIOA_Head[ sli->m_nCurI ];

		sli->m_nDir = a_nDir;

		return sli;
	}

	bool ScanLineItr::MoveNext()
	{
		int a = 5;

		//if( m_nCurI == m_nIOA_Size - 1 )
		if( m_nCurI >= m_nIOA_Size - 1 )
			return false;

		m_nCurI++;

		m_nCPI = m_pIOA_Head[ m_nCurI ];

		return true;
	}

	bool ScanLineItr::MovePrev()
	{
		//if( 0 == m_nCurI )
		if( 0 >= m_nCurI )
			return false;

		m_nCurI--;

		m_nCPI = m_pIOA_Head[ m_nCurI ];

		return true;
	}

	void ScanLineItr::GotoFirst()
	{
		m_nCurI = 0;

		m_nCPI = m_pIOA_Head[ m_nCurI ];
	}

	void ScanLineItr::GotoLast()
	{
		m_nCurI = m_nIOA_Size - 1;

		m_nCPI = m_pIOA_Head[ m_nCurI ];
	}





//ttttttttttttttttttttttttttttttttttt


	Signal1DRef ImgGradMgr::GenLineSignal( F32Point a_pnt, int a_nDir )
	{
		Signal1DBuilder sb1;

		//ScanLineItrRef sli = GenScanLineItr( a_pnt, a_nDir );
		ScanLineItrRef sli = GenScanLineItr( a_pnt, a_nDir );

		sli->GotoFirst();

		do
		{
			 const int nCPI = sli->GetCurPI();

			 const int nValIdx = a_nDir + nCPI * m_nofDirs;

			 float val = m_gradDataArr[ nValIdx ];

			 sb1.AddValue( val );
		}
		while( sli->MoveNext() );

		return sb1.GetResult();
	}




}