#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\ImgGradMgr_2.h>



namespace Hcv
{
	using namespace Hcpl;
	//using namespace Hcpl::Math;

	//using namespace Hcv::Ns_ImgGradMgr;
	using namespace Ns_ImgGradMgr;

	ImgGradMgr_2::ImgGradMgr_2( IImgDataMgrRef a_src, int a_nAprSiz, int a_nofDirs ) :
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


	F32ImageRef ImgGradMgr_2::GetGradImgOfDir( const int a_nDir )
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



	void ImgGradMgr_2::Proceed()
	{
		CvSize srcSiz = m_src->GetSize();

		const int nSrcSiz1D = srcSiz.width * srcSiz.height;
		m_gradDataArr.SetSize( m_nofDirs * nSrcSiz1D );

		m_sdmColl = new ScanDirMgrColl( m_nofDirs, srcSiz );

		for( int i=0; i < m_sdmColl->GetNofMgrs(); i++ )
		{
			ScanDirMgr & rScanMgr = m_sdmColl->GetMgrAt( i );

			ProcessSubDir( rScanMgr, m_src, 		 
				m_nAprSiz, &m_gradDataArr[ i ], m_nofDirs );
		}

	}


	FixedVector< bool > & ImgGradMgr_2::GetIsPeakDataArr()
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

	F32ImageRef ImgGradMgr_2::GenIsPeakImg()
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

	void ImgGradMgr_2::PrepareDirPeaks( int a_nDir, int a_nAprSiz, int a_nStepCnt )
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

			//nMarg = (int) ( ( ratio * nMargOrg ) + 0.5555555 );
			nMarg = nMargOrg;
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







	F32ImageRef ImgGradMgr_2:: GetGradMagImg()
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



	F32ImageRef ImgGradMgr_2::GetGradAngleImg()
	{
		if( NULL == m_gradAngleImg )
		{
			PrepareTotGrad();
		}

		return m_gradAngleImg;
	}


	S32ImageRef ImgGradMgr_2::GetGradDirImg()
	{
		if( NULL == m_gradDirImg )
		{
			PrepareTotGrad();
		}

		return m_gradDirImg;
	}


	void ImgGradMgr_2::PrepareTotGrad()
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



	Accessor2D< ImgGradMgr_2::FwdBkdPoints > & ImgGradMgr_2::GetFwdBkdMatrix()
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

				ImgGradMgr_2::FwdBkdPoints & fbPnts = fbPntBuf[ i ];

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



	F32Point ImgGradMgr_2::GetPntOfIndex( int a_nIndex, CvSize a_siz )
	{
		F32Point retPnt;

		retPnt.x = a_nIndex % a_siz.width;
		retPnt.y = a_nIndex / a_siz.width;

		return retPnt;
	}



//yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy





	ScanLineItrRef ImgGradMgr_2::GenScanLineItr( F32Point a_pnt, int a_nDir )
	{
		CvSize srcSiz = m_src->GetSize();

		IndexCalc2D idx2D( srcSiz.width, srcSiz.height );

		const int nPI = idx2D.Calc( a_pnt.x, a_pnt.y );

		return GenScanLineItr( nPI, a_nDir );
	}

	ScanLineItrRef ImgGradMgr_2::GenScanLineItr( int a_nPI, int a_nDir )
	{
		ScanLineItrRef sli = new ScanLineItr();
		
		PrepareScanLineItr( sli, a_nPI, a_nDir );

		return sli;
	}

	void ImgGradMgr_2::PrepareScanLineItr( 
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

		int * pIOA_Head = sli->m_pIOA_Head;

		int nSSI = rSm.GetShiftStartArr()[ rPli.nLineNum ];

		int nIOL_Adj = rPli.nIOL - nSSI;

		//sli->m_nIOA_Size = rSm.Get_ioaSizeArr()[ rPli.nLineNum ];
		sli->m_nIOA_Size = rSm.Get_ioaSizeAt( rPli.nLineNum );

		//sli->m_nSI = rSm.GetShiftStartArr()[ rPli.nLineNum ];
		//sli->m_nLimI = rSm.GetLimArr()[ rPli.nLineNum ];
		//sli->m_nEI = rSm.GetLimArr()[ rPli.nLineNum ] - 1;

		//sli->m_pShiftArr = &rSm.GetShiftIndexArr();

		//	hthm try
		//sli->m_nCurI = rPli.nIOL;
		sli->m_nCurI = nIOL_Adj;		
		//sli->m_nCurI = 0;

		sli->m_nCPI = sli->m_pIOA_Head[ sli->m_nCurI ];

		sli->m_nDir = a_nDir;
	}

	

	ScanLineItrRef ImgGradMgr_2::GenScanLineItr_FromLine( 
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





//ttttttttttttttttttttttttttttttttttt


	Signal1DRef ImgGradMgr_2::GenLineSignal( F32Point a_pnt, int a_nDir )
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




//-----------------------------------------


	void ImgGradMgr_2::ShowLineImages()
	{
		CvSize srcSiz = m_src->GetSize();

		const int nSrcSiz1D = srcSiz.width * srcSiz.height;

		int a_nAprSiz = m_nAprSiz;

		//m_sdmColl = new ScanDirMgrColl( m_nofDirs, srcSiz );

		for( int k=0; k < m_sdmColl->GetNofMgrs(); k++ )
		//for( int k=0; k < 1; k++ )
		{
			F32ImageRef ret = F32Image::Create( srcSiz, 3 );
			F32ColorVal * colorBuf = (F32ColorVal *)ret->GetPixAt( 0, 0 );

			F32ImageRef ret_1 = F32Image::Create( srcSiz, 3 );
			F32ColorVal * colorBuf_1 = (F32ColorVal *)ret_1->GetPixAt( 0, 0 );

			F32ImageRef ret_2 = F32Image::Create( srcSiz, 3 );
			F32ColorVal * colorBuf_2 = (F32ColorVal *)ret_2->GetPixAt( 0, 0 );

			ScanDirMgr & a_rScanMgr = m_sdmColl->GetMgrAt( k );

			FixedVector< int > & rBgnIndexArr = a_rScanMgr.GetBgnIndexArr();

			FixedVector< int > & rLimArr = a_rScanMgr.GetLimArr(); 

			FixedVector< int > & rShiftStartArr = a_rScanMgr.GetShiftStartArr(); 

			FixedVector< int > & rShiftIndexArr = a_rScanMgr.GetShiftIndexArr(); 

			const int nMargOrg = a_nAprSiz / 2;

			for( int i=0; i < rBgnIndexArr.GetSize(); i++ )
			{
				Hcpl::Int32 nBgnIdx = rBgnIndexArr[ i ];

				Hcpl::Int32 nCurIdx = nBgnIdx;

				const int nLim = rLimArr[ i ];

				const int nStart = rShiftStartArr[ i ];			


				for( int j = nStart; j < nLim; j++ )
				{
					nCurIdx = nBgnIdx + rShiftIndexArr[ j ];

					{
						//float val = ( i < 255 ) ? i : 255;
						float val = (( i / 1 ) % 2 ) * 255;
						//float val = (( i / 10 ) % 2 ) * 255;
						//float val = (( i / 70 ) % 2 ) * 255;

						colorBuf[ nCurIdx ].AssignVal( val, val, val );
					}

					{
						float val = (( i / 10 ) % 2 ) * 255;
						//float val = (( i / 70 ) % 2 ) * 255;

						colorBuf_1[ nCurIdx ].AssignVal( val, val, val );
					}
				}

			}

			{
				F32Point p1( 0, 0 );
				F32Point p2( 12, 8 );

				ret = GenClipedImg( ret, p1, p2 );

				ret = GenUpSampledImage( ret, 30, 30 );
				ret = GenImageWithGrid( ret, 30, 30 );				
			}

			{
				char sImgName[200];
				sprintf( sImgName, "Dir %d Lines", k );

				ShowImage( ret, sImgName );
			}

			{
				char sImgName[200];
				sprintf( sImgName, "Dir %d Lines a", k );

				ShowImage( ret_1, sImgName );
			}


		}



	}


	void ImgGradMgr_2::ProcessSubDir( ScanDirMgr & a_rScanMgr, IImgDataMgrRef a_srcImg, 
		int a_nAprSiz, float * a_retHead, int a_nStepCnt )
	{
		Hcpl_ASSERT( 1 == a_nAprSiz % 2 );

		CvSize srcSiz = a_srcImg->GetSize();

		{
			CvSize mgrSiz = a_rScanMgr.GetCvSize();

			Hcpl_ASSERT( mgrSiz.width == srcSiz.width &&
				mgrSiz.height == srcSiz.height );
		}



//--		F32ColorVal * srcColorArr = (F32ColorVal *) a_srcImg->GetPixAt( 0, 0 );

		FixedVector< int > & rBgnIndexArr = a_rScanMgr.GetBgnIndexArr();

		//FixedVector< F32Point > & rBgnPntArr = a_rScanMgr.GetBgnPntArr();

		FixedVector< int > & rLimArr = a_rScanMgr.GetLimArr(); 

		FixedVector< int > & rShiftStartArr = a_rScanMgr.GetShiftStartArr(); 

		

		FixedVector< int > & rShiftIndexArr = a_rScanMgr.GetShiftIndexArr(); 
		FixedVector< F32Point > & rShiftArr = a_rScanMgr.GetShiftArr(); 



		const int nMargOrg = a_nAprSiz / 2;


		int nMarg = -1;
		{
			F32Point dirPnt = a_rScanMgr.GetMainDirPnt();

			float distAct = dirPnt.CalcMag();

			float ratio = rShiftIndexArr.GetSize() / distAct;

			//nMarg = (int) ( ( ratio * nMargOrg ) + 0.5555555 );
			nMarg = nMargOrg;
		}

		const int nAprSizA = 2 * nMarg + 1;




		//Hcpl::Int32 * nextIdxArr = (Hcpl::Int32 *)a_rScanMgr.GetNextMapImg()->GetPixAt( 0, 0 );
		//int * nextIdxArr = (int *)a_rScanMgr.GetNextMapImg()->GetPixAt( 0, 0 );

		//Accessor2D< Hcpl::Int32 > ac1( &nextIdxArr[ 0 ], srcSiz.width, srcSiz.height );

		//const int nTestIdx = ac1.GetIndex1D( 323, 125 );
		const int nTestIdx = 124;

		int nCnt = 0;
		int nInrCnt = 0;

		F32Point testPnt;

		F32Point testBgnPnt( 124, 0 );

		FixedDeque< Hcpl::Int32 > indexDeque( nAprSizA );

		IndexCalc2D index2D( srcSiz.width, srcSiz.height );

		for( int i=0; i < rBgnIndexArr.GetSize(); i++ )
		{
			//Hcpl::Int32 nCurIdx = rBgnIndexArr[ i ];
			Hcpl::Int32 nBgnIdx = rBgnIndexArr[ i ];

			//F32Point rBgnPnt = rBgnPntArr[ i ];

			Hcpl::Int32 nCurIdx = nBgnIdx;

			indexDeque.ResetSize();


			//F32Point curPnt0 = GetPntOfIndex( nCurIdx, srcSiz );

			//if( F32Point::Compare( testBgnPnt, curPnt0 ) )
				//curPnt0 = curPnt0;


			const int nLim = rLimArr[ i ];

			const int nStart = rShiftStartArr[ i ];			

			{
				const int nMarg2 = nStart + nMarg;

				const int nLim2 = ( nMarg2 < nLim ) ? nMarg2 : nLim;

				for( int j = nStart; j < nLim2; j++ )
				{
					nCurIdx = nBgnIdx + rShiftIndexArr[ j ];

					a_retHead[ nCurIdx * a_nStepCnt ] = 0;
				}
			}

			{
				int nBgn2 = nLim - nMarg;

				if( nBgn2 < nStart )
					nBgn2 = nStart;

				for( int j = nBgn2; j < nLim; j++ )
				{
					nCurIdx = nBgnIdx + rShiftIndexArr[ j ];

					a_retHead[ nCurIdx * a_nStepCnt ] = 0;
				}
			}

			//while( true )
			for( int j = nStart; j < nLim; j++ )
			{
				nCurIdx = nBgnIdx + rShiftIndexArr[ j ];

				//F32Point resPnt = F32Point::Add(
					//rBgnPnt, rShiftArr[ j ] );

				//if( -1 == nCurIdx )
					//break;

				//F32Point curPnt = GetPntOfIndex( nCurIdx, srcSiz );

				//if( F32Point::Compare( testBgnPnt, curPnt ) )
					//curPnt = curPnt;

/*
				if( nTestIdx == nCurIdx )
				{
					//testPnt = a_rScanMgr.GetBgnPntArr()[ i ];

					testPnt = GetPntOfIndex( nCurIdx, srcSiz );

					nCurIdx = nCurIdx;
				}
*/

				nCnt++;

				indexDeque.PushBack( nCurIdx );

				if( indexDeque.GetSize() == nAprSizA )
				{
					nInrCnt++;

					const int nLastIdx = indexDeque.GetBack();
					const int nFirstIdx = indexDeque.GetFront();

//--					F32ColorVal * pLastColor = &srcColorArr[ nLastIdx ];
//--					F32ColorVal * pFirstColor = &srcColorArr[ nFirstIdx ];


					//if( 353882 == nLastIdx &&
					//	363610 == nFirstIdx )
					if( 363596 == nLastIdx &&
						344170 == nFirstIdx )
					{
						nCnt = nCnt;
					}


					//if( 353882 == nFirstIdx &&
					//	363610 == nLastIdx )
					if( 363596 == nFirstIdx &&
						344170 == nLastIdx )
					{
						nCnt = nCnt;
					}


					//float difMag = F32ColorVal::Sub( *pLastColor, *pFirstColor ).CalcMag();
					float difMag = a_srcImg->CalcDif( nLastIdx, nFirstIdx );

					//float colorDif_R1_R2 = m_src->CalcDif( 344170, 363596 );

					//	Hthm Tmp
					//if( a_rScanMgr.GetID() != GlobalStuff::ScanDir )
						//difMag = 0;



					//float unitDif = CalcColorUnitDif( *pLastColor, *pFirstColor );

					//difMag *= ( 1 + unitDif );
					//difMag *= ( 0.6 + unitDif );

					//if( difMag > 440 )
						//difMag = 440;

					const int nMedIdx = indexDeque[ nMarg ];

					if( 353883 == nMedIdx )
						i = i;


					a_retHead[ nMedIdx * a_nStepCnt ] = difMag;

					indexDeque.PopFront();
				}

				//Hcpl::Int32 nNextIdx = nextIdxArr[ nCurIdx ];
				//Hcpl::Int32 nNextIdx = nextIdxArr[ nCurIdx ];

				//nCurIdx = nNextIdx;
			}





		}

		nCnt = nCnt;

	}








//-----------------------------------------






}