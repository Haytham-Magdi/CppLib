#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\CBIS_Berkeley_Executer.h>



namespace Hcv
{
	using namespace Hcpl;
	//using namespace Hcpl::Math;


	CBIS_Berkeley_Executer::CBIS_Berkeley_Executer()
	{
		m_methodDirPath_BufArr.SetSize(2000);
	}



	void CBIS_Berkeley_Executer::Proceed()
	{

		FixedVector< ImgSeg_EvalMgr::HumanImg_ExpectedVal > expIndex_Info_Arr;



		{
			CString sSrcFilePath = 
				"E:\\HthmWork\\Lib\\BSDS_300_2\\exp_Rand_Index.dat";

			CFile file1;

			if( ! file1.Open(sSrcFilePath, CFile::modeRead) )
				ThrowHcvException();

			LONGLONG nFileLen = file1.GetLength();

			 
			expIndex_Info_Arr.SetSize( (int)( nFileLen / 
				sizeof( ImgSeg_EvalMgr::HumanImg_ExpectedVal ) ) );

			file1.Read( & expIndex_Info_Arr[ 0 ],
				nFileLen );

			file1.Close();
		}




		char sMethodImgSgm_Path[1000];

		strcpy( sMethodImgSgm_Path, m_methodDirPath_BufArr.GetHeadPtr() );

		strcat( sMethodImgSgm_Path, "\\Segms" );
	
		FixedVector< CString > segPath_Arr( 2000 );


		{
			CString sPath = sMethodImgSgm_Path;
			CFileFind hFile;
			BOOL bFound = hFile.FindFile( sPath + _T("\\*.seg2") );

			while ( bFound )   
			{
				bFound = hFile.FindNextFile();

				//CString sName = hFile.GetFileName();
				CString sPath_1 = hFile.GetFilePath();
				//CString sTitle = hFile.GetFileTitle();

				if( ( ! hFile.IsDots()) && ( ! hFile.IsDirectory() ) )		
				{
					segPath_Arr.PushBack( sPath_1 );
				}
			}

		}

		FixedVector< char > fileTitle_Cont( 500 );

		CString sHumanSegRoot =
			"E:\\HthmWork\\Lib\\BSDS_300_2\\Img_Human_1";

		FixedVector< ImgMth_SegVal > indexInfo_Arr;
		indexInfo_Arr.SetSize( segPath_Arr.GetSize() );

		for( int i=0; i < segPath_Arr.GetSize(); i++ )
		{
			CString & rSegPath = segPath_Arr[ i ];

			CString sPath_W_ImgID = rSegPath;

			sPath_W_ImgID.MakeLower();
			sPath_W_ImgID.Replace("-res", "");

			FilePathMgr fpm( sPath_W_ImgID.GetBuffer() );

			fpm.GetFileTitle( fileTitle_Cont );

			ImgSeg_EvalMgrRef sem = new ImgSeg_EvalMgr();

			sem->Set_AppSegMgr( new ImgSegDataMgr( 
				rSegPath.GetBuffer() ) );

			CString sHumanCollPath =
				sHumanSegRoot + "\\" + fileTitle_Cont.GetHeadPtr();

			sem->Set_HumanSegMgr_Coll( sHumanCollPath );
			
			sem->Proceed();

			int nImgNum = atoi( fileTitle_Cont.GetHeadPtr() );

			Hcpl_ASSERT( nImgNum > 0 );

			indexInfo_Arr[ i ].nImgNum = nImgNum;

			indexInfo_Arr[ i ].indexVal_Avg = sem->GetAvgIndexVal();
			indexInfo_Arr[ i ].indexVal_Max = sem->GetMaxIndexVal();

			int j = 0;

			for( ; j < expIndex_Info_Arr.GetSize(); j++ )
			{
				if( expIndex_Info_Arr[ j ].nImgNum == nImgNum )
					break;
			}

			Hcpl_ASSERT( j < expIndex_Info_Arr.GetSize() );


			indexInfo_Arr[ i ].expected_Val = 
				expIndex_Info_Arr[ j ].expectedVal;

			indexInfo_Arr[ i ].npr_Avg = 				
				( indexInfo_Arr[ i ].indexVal_Avg - expIndex_Info_Arr[ j ].expectedVal ) /
				( 1 - expIndex_Info_Arr[ j ].expectedVal );

			indexInfo_Arr[ i ].npr_Max = 				
				( indexInfo_Arr[ i ].indexVal_Max - expIndex_Info_Arr[ j ].expectedVal ) /
				( 1 - expIndex_Info_Arr[ j ].expectedVal );



			i = i;

		}



		{
			CString sFilePath = 
				//"E:\\HthmWork\\Lib\\BSDS_300_2\\method_Rand_Index.dat";
				
				CString("") +
				m_methodDirPath_BufArr.GetHeadPtr() +
				"\\method_Rand_Index.dat";


			CFile file1;

			if( ! file1.Open(sFilePath, CFile::modeCreate | CFile::modeWrite) )
				ThrowHcvException();

			file1.Write( & indexInfo_Arr[ 0 ], 				
				indexInfo_Arr.GetSize() * sizeof( ImgMth_SegVal ) );


			file1.Close();
		}
		

		{
			CString sFilePath = 
				//"E:\\HthmWork\\Lib\\BSDS_300_2\\method_Rand_Index.dat";
				
				CString("") +
				m_methodDirPath_BufArr.GetHeadPtr() +
				"\\method_Rand_Index_Dsp.txt";



			char sOut_Buf[ 100000 ];
			sOut_Buf[ 0 ] = 0;

			int nOutBuf_Cnt = 0;

			for( int i=0; i < indexInfo_Arr.GetSize(); i++ )
			{
				nOutBuf_Cnt += sprintf( & sOut_Buf[ nOutBuf_Cnt ], 
					"%d\t%f\t%f\t%f\t%f\t%f\r\n", 
					indexInfo_Arr[ i ].nImgNum,
					indexInfo_Arr[ i ].expected_Val,
					indexInfo_Arr[ i ].indexVal_Avg,
					indexInfo_Arr[ i ].indexVal_Max,
					indexInfo_Arr[ i ].npr_Avg,
					indexInfo_Arr[ i ].npr_Max
					);
			}
			

			CFile file1;

			if( ! file1.Open(sFilePath, CFile::modeCreate | CFile::modeWrite) )
				ThrowHcvException();
			
			file1.Write( sOut_Buf, nOutBuf_Cnt );

			file1.Close();



		}
		



	}



	void CBIS_Berkeley_Executer::Set_MethodDirPath( char * a_methodDirPath )
	{
		strcpy( m_methodDirPath_BufArr.GetHeadPtr(), a_methodDirPath );
	}


}