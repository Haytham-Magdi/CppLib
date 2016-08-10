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


	ImgSeg_EvalMgr::ImgSeg_EvalMgr( char * a_sFilePath )
	{
		strcpy( m_sFilePath, a_sFilePath );

		Proceed();
	}



	void ImgSeg_EvalMgr::Proceed()
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

		char buf_100[100];

		memcpy( buf_100, & fileData_Arr[ nCnt ], 100 );



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


	void ImgSeg_EvalMgr::ShowSegmImg()
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

		for( int i=0; i < nSiz_1D; i++ )
		{
			int nIdx_Color = src_Buf[ i ];

			Hcpl_ASSERT( nIdx_Color >= 0 && nIdx_Color < m_nofSegments ); 
						
			//img1_Buf[ i ] = color_Arr[ nIdx_Color ];
			//img1_Buf[ i ] = c2;
			//img1_Buf[ i ].AssignVal( c2.val0, c2.val1, c2.val2 );
			//img1_Buf[ i ].AssignVal( c2 );
			img1_Buf[ i ].AssignVal( color_Arr[ nIdx_Color ] );
		}

		//ShowImage(img1, "img1");
		ShowImage(img1, m_sFileName);

		

	}

}