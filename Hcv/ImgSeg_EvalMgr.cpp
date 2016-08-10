#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\ImgSeg_EvalMgr.h>



namespace Hcv
{
	using namespace Hcpl;
	//using namespace Hcpl::Math;


	ImgSeg_EvalMgr::ImgSeg_EvalMgr()
	{
		m_humanSegMgr_Arr.SetCapacity(2000);		

		m_avgIndexVal = -1;
		m_maxIndexVal = -1;
	}


	void ImgSeg_EvalMgr::Set_HumanSegMgr_Coll( CString a_collPath )
	{
		CString sPath = a_collPath;
		CFileFind hFile;
		BOOL bFound = hFile.FindFile( sPath + _T("\\*.*") );

		while ( bFound )   
		{
			bFound = hFile.FindNextFile();

			//CString sName = hFile.GetFileName();
			CString sPath_1 = hFile.GetFilePath();
			//CString sTitle = hFile.GetFileTitle();

			if( ( ! hFile.IsDots()) && ( ! hFile.IsDirectory() ) )		
			{
				this->Add_HumanSegMgr( new ImgSegDataMgr( 
					sPath_1.GetBuffer() ) );
			}
		}


	}


	void ImgSeg_EvalMgr::Save_ExpectedIndex_Text()
	{
		//FixedVector< double > imgExpIndex_Arr;
		FixedVector< HumanImg_ExpectedVal > imgIndexInfo_Arr;
				

		{
			CString sSrcFilePath = 
				"E:\\HthmWork\\Lib\\BSDS_300_2\\exp_Rand_Index.dat";

			CFile file1;

			if( ! file1.Open(sSrcFilePath, CFile::modeRead) )
				ThrowHcvException();

			LONGLONG nFileLen = file1.GetLength();

			 
			imgIndexInfo_Arr.SetSize( (int)( nFileLen / sizeof( HumanImg_ExpectedVal ) ) );

			file1.Read( & imgIndexInfo_Arr[ 0 ],
				nFileLen );

			file1.Close();
		}



		
		{
			char sOut_Buf[ 100000 ];
			sOut_Buf[ 0 ] = 0;

			int nOutBuf_Cnt = 0;

			for( int i=0; i < imgIndexInfo_Arr.GetSize(); i++ )
			{
				nOutBuf_Cnt += sprintf( & sOut_Buf[ nOutBuf_Cnt ], 
					//"%f\r\n", imgExpIndex_Arr[ i ] );
					"%d\t%f\r\n", imgIndexInfo_Arr[ i ].nImgNum,
					imgIndexInfo_Arr[ i ].expectedVal );
			}
			


			CString sFilePath = 
				"E:\\HthmWork\\Lib\\BSDS_300_2\\exp_Rand_Index_Dsp.txt";

			CFile file1;

			if( ! file1.Open(sFilePath, CFile::modeCreate | CFile::modeWrite) )
				ThrowHcvException();
			
			file1.Write( sOut_Buf, nOutBuf_Cnt );

			file1.Close();
		}
		



	}


	//double ImgSeg_EvalMgr::Calc_ExpectedIndex()
	void ImgSeg_EvalMgr::Calc_ExpectedIndex()
	{
		//char sOut_Buf[ 100000 ];
		//sOut_Buf[ 0 ] = 0;

		//int nOutBuf_Cnt = 0;

		//nOutBuf_Cnt += sprintf( & sOut_Buf[ nOutBuf_Cnt ], "%d\r\n", 7 );
		//nOutBuf_Cnt += sprintf( & sOut_Buf[ nOutBuf_Cnt ], "%d\r\n", 7 );
		//nOutBuf_Cnt += sprintf( & sOut_Buf[ nOutBuf_Cnt ], "%d\r\n", 7 );
		//nOutBuf_Cnt += sprintf( & sOut_Buf[ nOutBuf_Cnt ], "%d\r\n", 7 );
		//nOutBuf_Cnt += sprintf( & sOut_Buf[ nOutBuf_Cnt ], "%d\r\n", 7 );

		
		FixedVector< CvPoint > ij_Arr;

		ij_Arr.SetSize( 5000000 );


		CvSize imgSiz = cvSize( 481, 321 );

		int nImgSiz_1D = imgSiz.width * imgSiz.height;


		//int nMaxR = -1000000;
		//int nMinR = 1000000;

		MaxFinder< int > maxFnd;
		MinFinder< int > minFnd;

		

		for( int i=0; i < ij_Arr.GetSize(); i++ )
		{
			CvPoint & rP1 = ij_Arr[ i ];

			rP1.x = ( rand() + rand() * RAND_MAX ) % 
				nImgSiz_1D;

			maxFnd.AddValue( rP1.x );
			minFnd.AddValue( rP1.x );


			//rP1.y = rand() % nImgSiz_1D;
			rP1.y = ( rand() + rand() * RAND_MAX ) % 
				nImgSiz_1D;

			maxFnd.AddValue( rP1.y );
			minFnd.AddValue( rP1.y );
			
		}


		if( false )
		{
			F32ImageRef img1 = F32Image::Create( imgSiz, 3 );
			img1->SetAll( 0 );

			F32ColorVal * img1_ColorBuf = (F32ColorVal *)img1->GetPixAt( 0, 0 );

			for( int i=0; i < ij_Arr.GetSize(); i++ )
			{
				CvPoint & rP1 = ij_Arr[ i ];

				F32ColorVal * pI = & img1_ColorBuf[ rP1.x ];
				pI->val1 = 180;

				F32ColorVal * pJ = & img1_ColorBuf[ rP1.y ];
				pJ->val2 = 180;
			}

			ShowImage( img1, "exp_Index_Img" );
		}


//////////////////////////////////////////////////////////////////


		FixedVector< CString > imgDir_Arr( 1000 );

		{
			CString sRootDirPath(
				"E:\\HthmWork\\Lib\\BSDS_300_2\\Img_Human_1");

			CString sPath = sRootDirPath;
			CFileFind hFile;
			BOOL bFound = hFile.FindFile( sPath + _T("\\*.*") );

			while ( bFound )   
			{
				bFound = hFile.FindNextFile();

				CString sPath_1 = hFile.GetFilePath();

				if( ( ! hFile.IsDots()) && ( hFile.IsDirectory() ) )		
				{
					imgDir_Arr.PushBack( sPath_1 );
				}
			}

		}


		FixedVector< double > pij_D_Arr;
		pij_D_Arr.SetSize( ij_Arr.GetSize() );

		for( int i=0; i < pij_D_Arr.GetSize(); i++ )
			pij_D_Arr[ i ] = 0;

		int nImg_D_Cnt = 0;
		

		FixedVector< double > pij_Arr;
		pij_Arr.SetSize( ij_Arr.GetSize() );

		FixedVector< ImgSegDataMgrRef > humanSegMgr_Arr(3000);

		//imgDir_Arr.SetSize( 5 );

		for( int p=0; p < imgDir_Arr.GetSize(); p++ )
		//for( int p=0; p < 5; p++ )
		{
			int nImgCnt = 0;		

			CString sDirPath(
				//"E:\\HthmWork\\Lib\\BSDS_300_2\\Img_Human_1\\2092");
				imgDir_Arr[ p ] );

			for( int i=0; i < pij_Arr.GetSize(); i++ )
				pij_Arr[ i ] = 0;

			humanSegMgr_Arr.ResetSize();
			
			CString sPath = sDirPath;
			CFileFind hFile;
			BOOL bFound = hFile.FindFile( sPath + _T("\\*.*") );

			while ( bFound )   
			{
				bFound = hFile.FindNextFile();

				CString sPath_1 = hFile.GetFilePath();

				if( ( ! hFile.IsDots()) && ( ! hFile.IsDirectory() ) )		
				{
					humanSegMgr_Arr.PushBack( new ImgSegDataMgr( 
						sPath_1.GetBuffer() ) );
				}
			}

			for( int k=0; k < humanSegMgr_Arr.GetSize(); k++ )
			{
				FixedVector< int > & rPixArr = 
					humanSegMgr_Arr[ k ]->GetPixArr();

				for( int i=0; i < ij_Arr.GetSize(); i++ )
				{
					CvPoint & rP1 = ij_Arr[ i ];

					if( rPixArr[ rP1.x ] == rPixArr[ rP1.y ] )
						pij_Arr[ i ]++;
				}
			}

			nImgCnt += humanSegMgr_Arr.GetSize();

			for( int i=0; i < ij_Arr.GetSize(); i++ )
			{
				CvPoint & rP1 = ij_Arr[ i ];

				pij_D_Arr[ i ] += pij_Arr[ i ];
			}

			nImg_D_Cnt += nImgCnt;
		}

//uuuu


		FixedVector< HumanImg_ExpectedVal > imgIndexInfo_Arr;
		imgIndexInfo_Arr.SetSize( imgDir_Arr.GetSize() );

		//FixedVector< double > imgExpIndex_Arr;
		//imgExpIndex_Arr.SetSize( imgDir_Arr.GetSize() );


		for( int p=0; p < imgDir_Arr.GetSize(); p++ )
		//for( int p=0; p < 5; p++ )
		{
			int nImgCnt = 0;		

			CString sDirPath(
				//"E:\\HthmWork\\Lib\\BSDS_300_2\\Img_Human_1\\2092");
				imgDir_Arr[ p ] );

			for( int i=0; i < pij_Arr.GetSize(); i++ )
				pij_Arr[ i ] = 0;

			humanSegMgr_Arr.ResetSize();
			
			CString sPath = sDirPath;
			CFileFind hFile;
			BOOL bFound = hFile.FindFile( sPath + _T("\\*.*") );

			while ( bFound )   
			{
				bFound = hFile.FindNextFile();

				CString sPath_1 = hFile.GetFilePath();

				if( ( ! hFile.IsDots()) && ( ! hFile.IsDirectory() ) )		
				{
					humanSegMgr_Arr.PushBack( new ImgSegDataMgr( 
						sPath_1.GetBuffer() ) );
				}
			}

			for( int k=0; k < humanSegMgr_Arr.GetSize(); k++ )
			{
				FixedVector< int > & rPixArr = 
					humanSegMgr_Arr[ k ]->GetPixArr();

				for( int i=0; i < ij_Arr.GetSize(); i++ )
				{
					CvPoint & rP1 = ij_Arr[ i ];

					if( rPixArr[ rP1.x ] == rPixArr[ rP1.y ] )
						pij_Arr[ i ]++;
				}
			}

			nImgCnt = humanSegMgr_Arr.GetSize();

			double expIndex = 0;

			for( int i=0; i < ij_Arr.GetSize(); i++ )
			{
				double pij = pij_Arr[ i ] / nImgCnt;
				double pij_D = pij_D_Arr[ i ] / nImg_D_Cnt;

				expIndex += pij * pij_D +
					( 1 - pij ) * ( 1 - pij_D );
			}

			expIndex /= ij_Arr.GetSize();

			
			//imgExpIndex_Arr[ p ] = expIndex;
			imgIndexInfo_Arr[ p ].expectedVal = expIndex;

			{
				FilePathMgr fpm( imgDir_Arr[ p ].GetBuffer() );

				FixedVector< char > title_Cont;

				fpm.GetFileTitle( title_Cont );

				int nImgNum = atoi( title_Cont.GetHeadPtr() );

				Hcpl_ASSERT( nImgNum > 0 );

				imgIndexInfo_Arr[ p ].nImgNum = nImgNum;
			}
			


			//for( int i=0; i < ij_Arr.GetSize(); i++ )
			//{
			//	CvPoint & rP1 = ij_Arr[ i ];

			//	pij_D_Arr[ i ] += pij_Arr[ i ];
			//}

			//nImg_D_Cnt += nImgCnt;
		}


		{
			CString sFilePath = 
				"E:\\HthmWork\\Lib\\BSDS_300_2\\exp_Rand_Index.dat";

			CFile file1;

			if( ! file1.Open(sFilePath, CFile::modeCreate | CFile::modeWrite) )
				ThrowHcvException();

			//file1.Write( & imgExpIndex_Arr[ 0 ], 
				//imgExpIndex_Arr.GetSize() * sizeof(double) );

			file1.Write( & imgIndexInfo_Arr[ 0 ], 				
				imgIndexInfo_Arr.GetSize() * sizeof( HumanImg_ExpectedVal ) );

			

			file1.Close();
		}
		






		//return -1;
		return;
	}



	void ImgSeg_EvalMgr::Calc_ExpectedIndex_2()
	{
		ImgSeg_EvalMgrRef evalMgr = new ImgSeg_EvalMgr();


//////////////////////////////////////////////////////////////////


		FixedVector< CString > imgDir_Arr( 1000 );

		{
			CString sRootDirPath(
				"E:\\HthmWork\\Lib\\BSDS_300_2\\Img_Human_1");

			CString sPath = sRootDirPath;
			CFileFind hFile;
			BOOL bFound = hFile.FindFile( sPath + _T("\\*.*") );

			while ( bFound )   
			{
				bFound = hFile.FindNextFile();

				CString sPath_1 = hFile.GetFilePath();

				if( ( ! hFile.IsDots()) && ( hFile.IsDirectory() ) )		
				{
					imgDir_Arr.PushBack( sPath_1 );
				}
			}

		}


		int nImg_D_Cnt = 0;
		

		FixedVector< ImgSegDataMgrRef > humanSegMgr_Arr(3000);

		FixedVector< CString > humanSeg_Path_Arr(3000);

		//imgDir_Arr.SetSize( 5 );

		for( int p=0; p < imgDir_Arr.GetSize(); p++ )
		//for( int p=0; p < 5; p++ )
		{
			int nImgCnt = 0;		

			CString sDirPath(
				//"E:\\HthmWork\\Lib\\BSDS_300_2\\Img_Human_1\\2092");
				imgDir_Arr[ p ] );


			humanSeg_Path_Arr.ResetSize();
			
			CString sPath = sDirPath;
			CFileFind hFile;
			BOOL bFound = hFile.FindFile( sPath + _T("\\*.*") );

			while ( bFound )   
			{
				bFound = hFile.FindNextFile();

				CString sPath_1 = hFile.GetFilePath();

				if( ( ! hFile.IsDots()) && ( ! hFile.IsDirectory() ) )		
				{
					humanSeg_Path_Arr.PushBack( sPath_1 );
				}
			}

			float sum_Avg = 0;

			float sum_Max = 0;

			for( int i=0; i < humanSeg_Path_Arr.GetSize(); i++ )
			{
				evalMgr->Reset();

				CString sAppSegPath = humanSeg_Path_Arr[ i ];

				evalMgr->Set_AppSegMgr( new ImgSegDataMgr( 
							sAppSegPath.GetBuffer() ) );

				evalMgr->Set_HumanSegMgr_Coll( sDirPath );

				evalMgr->Proceed();

				sum_Avg += evalMgr->GetAvgIndexVal();

				sum_Max += evalMgr->GetMaxIndexVal();
			}


			nImgCnt += humanSeg_Path_Arr.GetSize();

			sum_Avg /= nImgCnt;
			sum_Max /= nImgCnt;

			//nImg_D_Cnt += nImgCnt;
		}

//uuuu

		return;


		FixedVector< HumanImg_ExpectedVal > imgIndexInfo_Arr;
		imgIndexInfo_Arr.SetSize( imgDir_Arr.GetSize() );

		//FixedVector< double > imgExpIndex_Arr;
		//imgExpIndex_Arr.SetSize( imgDir_Arr.GetSize() );


		for( int p=0; p < imgDir_Arr.GetSize(); p++ )
		//for( int p=0; p < 5; p++ )
		{
			int nImgCnt = 0;		

			CString sDirPath(
				//"E:\\HthmWork\\Lib\\BSDS_300_2\\Img_Human_1\\2092");
				imgDir_Arr[ p ] );




			humanSegMgr_Arr.ResetSize();
			
			CString sPath = sDirPath;
			CFileFind hFile;
			BOOL bFound = hFile.FindFile( sPath + _T("\\*.*") );

			while ( bFound )   
			{
				bFound = hFile.FindNextFile();

				CString sPath_1 = hFile.GetFilePath();

				if( ( ! hFile.IsDots()) && ( ! hFile.IsDirectory() ) )		
				{
					humanSegMgr_Arr.PushBack( new ImgSegDataMgr( 
						sPath_1.GetBuffer() ) );
				}
			}

			for( int k=0; k < humanSegMgr_Arr.GetSize(); k++ )
			{
				CString sAppSegPath(
					"E:\\HthmWork\\Lib\\BSDS_300_2\\Img_Human_1\\2092\\2092_1103.seg2");

				evalMgr->Set_AppSegMgr( new ImgSegDataMgr( 
							sAppSegPath.GetBuffer() ) );

				CString sDirPath(
					"E:\\HthmWork\\Lib\\BSDS_300_2\\Img_Human_1\\2092");

				evalMgr->Reset();

				evalMgr->Set_HumanSegMgr_Coll( sDirPath );

				evalMgr->Proceed();
			}

			nImgCnt = humanSegMgr_Arr.GetSize();

			double expIndex = 0;


			
			//imgExpIndex_Arr[ p ] = expIndex;
			imgIndexInfo_Arr[ p ].expectedVal = expIndex;

			{
				FilePathMgr fpm( imgDir_Arr[ p ].GetBuffer() );

				FixedVector< char > title_Cont;

				fpm.GetFileTitle( title_Cont );

				int nImgNum = atoi( title_Cont.GetHeadPtr() );

				Hcpl_ASSERT( nImgNum > 0 );

				imgIndexInfo_Arr[ p ].nImgNum = nImgNum;
			}
			

		}


		{
			CString sFilePath = 
				"E:\\HthmWork\\Lib\\BSDS_300_2\\exp_Rand_Index.dat";

			CFile file1;

			if( ! file1.Open(sFilePath, CFile::modeCreate | CFile::modeWrite) )
				ThrowHcvException();

			//file1.Write( & imgExpIndex_Arr[ 0 ], 
				//imgExpIndex_Arr.GetSize() * sizeof(double) );

			file1.Write( & imgIndexInfo_Arr[ 0 ], 				
				imgIndexInfo_Arr.GetSize() * sizeof( HumanImg_ExpectedVal ) );

			

			file1.Close();
		}
		






		//return -1;
		return;
	}



	//void ImgSeg_EvalMgr::Reset( ImgSegDataMgrRef a_arg )
	void ImgSeg_EvalMgr::Reset()
	{
		m_appSegMgr = NULL;
		m_humanSegMgr_Arr.ResetSize();
	}


	void ImgSeg_EvalMgr::Proceed()
	{
		//double indexVal = m_appSegMgr->CalcRandIndex( m_humanSegMgr_Arr[ 0 ] );

		//ImgSeg_EvalMgr::Calc_ExpectedIndex();
		//ImgSeg_EvalMgr::Calc_ExpectedIndex_2();
		//ImgSeg_EvalMgr::Save_ExpectedIndex_Text();

		double sum = 0;

		MaxFinder< double > maxFnd;

		for( int i=0; i < m_humanSegMgr_Arr.GetSize(); i++ )
		{
			double indexVal = m_appSegMgr->CalcRandIndex( m_humanSegMgr_Arr[ i ] );

			maxFnd.AddValue( indexVal );
			sum += indexVal;

			//m_humanSegMgr_Arr[ i ]->ShowSegmImg();
		}

		m_avgIndexVal = sum / m_humanSegMgr_Arr.GetSize();

		m_maxIndexVal = maxFnd.FindMax();	
	}



}