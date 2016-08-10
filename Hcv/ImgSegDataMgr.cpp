#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\ImgSegDataMgr.h>



namespace Hcv
{
	using namespace Hcpl;
	using namespace Hcpl::Math;


	ImgSegDataMgr::ImgSegDataMgr( char * a_sFilePath )
	{
		strcpy( m_sFilePath, a_sFilePath );

		Proceed();
	}



	void ImgSegDataMgr::Proceed()
	{		
		{
			int nPathLen = strlen( m_sFilePath );

			int i;
			for( i = nPathLen - 1; i >= 0; i-- )
			{
				if( '\\' == m_sFilePath[ i ] )
					break;

			}

			char * sFN = & m_sFilePath[ i + 1 ];

			strcpy( m_sFileName, sFN );


			i = i;
		}

		char * sFilePath = m_sFilePath;

		CFile file1;

		if( ! file1.Open(sFilePath, CFile::modeRead) )
			ThrowHcvException();

		

		ULONGLONG nFileLen = file1.GetLength();


		FixedVector<byte> fileData_Arr;

		fileData_Arr.SetSize( (int) nFileLen );

		//byte * fileBuf

		file1.Read( & fileData_Arr[0], fileData_Arr.GetSize());

		char sMark[] = "Seg2_File";

		int nMarkLen = strlen(sMark);

		int nCnt = 1;

		char sFileMark[100];

		//char buf_100[100];

		//memcpy( buf_100, & fileData_Arr[ nCnt ], 100 );



		strncpy(sFileMark, (char *) & fileData_Arr[ nCnt ], nMarkLen);
		sFileMark[nMarkLen] = 0;

		nCnt += nMarkLen;

		if( 0 != strcmp(sMark, sFileMark) )
			ThrowHcvException();

		m_nWidth = *( (int *) & fileData_Arr[ nCnt ] );
		nCnt += sizeof(int);

		m_nHeight = *( (int *) & fileData_Arr[ nCnt ] );
		nCnt += sizeof(int);

		m_nofSegments = *( (int *) & fileData_Arr[ nCnt ] );
		nCnt += sizeof(int);


		int nSiz_1D = m_nWidth * m_nHeight;

		m_pixArr.SetSize(nSiz_1D);


		int * src_Buf_0 = (int *) & fileData_Arr[ nCnt ];

		memcpy( & m_pixArr[0], & fileData_Arr[ nCnt ], 
			m_pixArr.GetSize() * sizeof(int));
		
	}


	void ImgSegDataMgr::ShowSegmImg( char * a_sWindowName )
	{
		FixedVector< F32ColorVal > color_Arr(10000);

		color_Arr.ResetSize();

		F32ColorVal c1;

		for( int i=0; i < m_nofSegments; i++ )
		{
			c1.val0 = rand() % 256;
			c1.val1 = rand() % 256;
			c1.val2 = rand() % 256;

			//c1.AssignVal( 0, 0, 160 );

			color_Arr.PushBack( c1 );
		}

		F32ImageRef img1 = F32Image::Create( cvSize( m_nWidth, m_nHeight ), 3 );
		img1->SetAll( 130 );

		//ShowImage(img1, "img1");

		F32ColorVal * img1_Buf = (F32ColorVal *) img1->GetPixAt(0, 0);


		int nSiz_1D = m_pixArr.GetSize();


		int * src_Buf = (int *) & m_pixArr[0];

		F32ColorVal c2;
		c2.AssignVal( 0, 130, 0 );

		MaxFinder< int > maxFnd;

		for( int i=0; i < nSiz_1D; i++ )
		{
			int nIdx_Color = src_Buf[ i ];

			maxFnd.AddValue( nIdx_Color );

			//	Hthm Tmp
			nIdx_Color %= m_nofSegments;

			Hcpl_ASSERT( nIdx_Color >= 0 && nIdx_Color < m_nofSegments ); 
						
			//img1_Buf[ i ] = color_Arr[ nIdx_Color ];
			//img1_Buf[ i ] = c2;
			//img1_Buf[ i ].AssignVal( c2.val0, c2.val1, c2.val2 );
			//img1_Buf[ i ].AssignVal( c2 );
			img1_Buf[ i ].AssignVal( color_Arr[ nIdx_Color ] );
		}

		int nMax = maxFnd.FindMax();

		char * sWindowName = a_sWindowName;

		if( NULL == sWindowName )
			sWindowName = m_sFileName;

		//ShowImage(img1, "img1");
		ShowImage( img1, sWindowName );

		

	}


	double ImgSegDataMgr::CalcRandIndex( ImgSegDataMgrRef a_peer )
	{
		Hcpl_ASSERT( this->m_nWidth = a_peer->m_nWidth );
		Hcpl_ASSERT( this->m_nHeight = a_peer->m_nHeight );

		int nofSegms_1 = this->m_nofSegments;
		int nofSegms_2 = a_peer->m_nofSegments;

		//Accessor2D< int > contingencyTbl( 
		Accessor2D< int > contTbl( 
			nofSegms_1, nofSegms_2 );


		{
			int * cont_Buf = contTbl.GetPtrAt( 0, 0 );

			int nSiz1D = contTbl.GetTotalSize();

			for( int i=0; i < nSiz1D; i++ )
				cont_Buf[ i ] = 0;
		}

		
		IndexCalc2D idx2D( this->m_nWidth, this->m_nHeight );

		FixedVector< int > & rPixArr_1 = this->GetPixArr();
		FixedVector< int > & rPixArr_2 = a_peer->GetPixArr();

		for( int y=0; y < this->m_nHeight; y++ )
		{
			for( int x=0; x < this->m_nWidth; x++ )
			{
				int i = idx2D.Calc( x, y );

				int nSeg_1 = rPixArr_1[ i ];
				int nSeg_2 = rPixArr_2[ i ];

				int a = contTbl.GetAt( nSeg_1, nSeg_2 );

				contTbl.GetAt( nSeg_1, nSeg_2 )++;

				int b = contTbl.GetAt( nSeg_1, nSeg_2 );

				a = a;
			}
		}


		LONGLONG val_12 = 0;

		for( int nSeg_1 = 0; nSeg_1 < nofSegms_1; nSeg_1++ )
		{
			for( int nSeg_2 = 0; nSeg_2 < nofSegms_2; nSeg_2++ )
			{
				LONGLONG val_0 = contTbl.GetAt( nSeg_1, nSeg_2 );

				LONGLONG val_1 = SomeFuncCP_2( val_0 );

				val_12 += val_1;
			}
		}

		FixedVector< LONGLONG > sum_1_Arr;
		sum_1_Arr.SetSize( nofSegms_1 );

		for( int nSeg_1 = 0; nSeg_1 < nofSegms_1; nSeg_1++ )
		{
			sum_1_Arr[ nSeg_1 ] = 0;

			for( int nSeg_2 = 0; nSeg_2 < nofSegms_2; nSeg_2++ )
			{
				sum_1_Arr[ nSeg_1 ] += contTbl.GetAt( nSeg_1, nSeg_2 );
			}
		}



		FixedVector< LONGLONG > sum_2_Arr;
		sum_2_Arr.SetSize( nofSegms_2 );

		for( int nSeg_2 = 0; nSeg_2 < nofSegms_2; nSeg_2++ )
		{
			sum_2_Arr[ nSeg_2 ] = 0;

			for( int nSeg_1 = 0; nSeg_1 < nofSegms_1; nSeg_1++ )
			{
				sum_2_Arr[ nSeg_2 ] += contTbl.GetAt( nSeg_1, nSeg_2 );
			}
		}


		LONGLONG nSiz_1D = m_pixArr.GetSize();

		LONGLONG nN_2 = SomeFuncCP_2( nSiz_1D );

		LONGLONG val_13_1 = nN_2;


		LONGLONG val_13_2 = val_12;


		LONGLONG val_13_3 = 0;

		for( int i = 0; i < nofSegms_1; i++ )
		{
			val_13_3 += SomeFuncCP_2( sum_1_Arr[ i ] );
		}


		LONGLONG val_13_4 = 0;

		for( int i = 0; i < nofSegms_2; i++ )
		{
			LONGLONG val__0 = sum_2_Arr[ i ];

			if( val__0 < 0 )
				i = i;

			LONGLONG val__1 = SomeFuncCP_2( val__0 );

			if( val__1 < 0 )
				i = i;


			val_13_4 += val__1;
		}



		LONGLONG val_13 = val_13_1 + val_13_2
			- val_13_3 - val_13_4;



		double res = val_12 + val_13;

		res /= nN_2;




		return res;
	}


}