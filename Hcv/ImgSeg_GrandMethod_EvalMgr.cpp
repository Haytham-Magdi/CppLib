#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\ImgSeg_Method_EvalMgr.h>

#include <Lib\Hcv\ImgSeg_GrandMethod_EvalMgr.h>


namespace Hcv
{
	using namespace Hcpl;
	//using namespace Hcpl::Math;


	ImgSeg_GrandMethod_EvalMgr::ImgSeg_GrandMethod_EvalMgr()
	{
		//m_methodDirPath_BufArr.SetSize(2000);
	}



	void ImgSeg_GrandMethod_EvalMgr::Proceed()
	{
		FixedVector< CString > sGrandMth_Path_Arr( 2000 );

		//sGrandMth_Path_Arr.PushBack( 
		//	"E:\\HthmWork\\Master\\Image-Segmentation-Source-Code\\Special\\Mean-Shift\\Result-Images");

		//sGrandMth_Path_Arr.PushBack( 
		//	"E:\\HthmWork\\Master\\Image-Segmentation-Source-Code\\Special\\Efficient-Graph-Based-Image-Segmentation\\Result-Images");

		sGrandMth_Path_Arr.PushBack( 
			"E:\\HthmWork\\Lib\\BSDS_300_2\\CBIS");

		FixedVector< CString > sMethod_Path_Arr( 2000 );

		//sMethod_Path_Arr.PushBack( 
			//"E:\\HthmWork\\Lib\\BSDS_300_2\\CBIS");

		for( int i=0; i < sGrandMth_Path_Arr.GetSize(); i++ )
		{
			CString sPath = sGrandMth_Path_Arr[ i ];
			CFileFind hFile;
			BOOL bFound = hFile.FindFile( sPath + "\\*.*" );

			while ( bFound )   
			{
				bFound = hFile.FindNextFile();

				//CString sName = hFile.GetFileName();
				CString sPath_1 = hFile.GetFilePath();
				//CString sTitle = hFile.GetFileTitle();

				if( ( ! hFile.IsDots()) && ( hFile.IsDirectory() ) )		
				{
					sMethod_Path_Arr.PushBack( sPath_1 );
				}
			}

		}


		for( int i=0; i < sMethod_Path_Arr.GetSize(); i++ )
		{
			CString sMethodPath = sMethod_Path_Arr[ i ];


			{
				CString sPath = sMethodPath;
				CFileFind hFile;
				BOOL bFound = hFile.FindFile( sPath + _T("\\method_Rand_Index.dat") );
				
				//hFile.

				if ( bFound )   
				{
					bFound = hFile.FindNextFile();

					long nFileLen = (long) hFile.GetLength();

					if( 14400 == nFileLen )
						continue;
				}
			}
			

			ImgSeg_Method_EvalMgrRef smm = new ImgSeg_Method_EvalMgr();

			smm->Set_MethodDirPath( sMethodPath.GetBuffer() );
			//smm->Set_MethodDirPath( sMethodPath );

			smm->Proceed();
		}

		return;

	}


}