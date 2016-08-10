#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\ImgScanMgr.h>

//namespace Hcv
//{
using namespace Hcpl;
//using namespace Hcpl::Math;

using namespace Hcv;
using namespace ImgScanMgr_Ns;


int ImgScanMgr_Ns::EdgeInfo::s_nIndex = 0;


ScanDir::ScanDir( ImageRotationMgr_ExRef a_rotMgr,
	int a_nDirIdx, int a_nNormIdx, int a_nRevIdx )
{
	RotationMgr = a_rotMgr;

	nDirIdx = a_nDirIdx;

	nNormIdx = a_nNormIdx;

	nRevIdx = a_nRevIdx;

	ImgRotationMgrRef coreRot = a_rotMgr->GetCoreRotMgr();
	
	//ImageLineItrProvider * pItrProv;
	
	pItrProv = & (coreRot->GetImageItrMgr()->GetItrProvAt( 
		a_nNormIdx + 2 * a_nRevIdx ));

	//ImageLineItrProvider & rProv = coreRot->GetImageItrMgr()->GetItrProvAt( 
		//a_nNormIdx + 2 * a_nRevIdx );

	OrgToRot_Map_Buf = (int *) coreRot->Get_SrcToResMapImage(
		)->GetPixAt( 0, 0 );

	RotToOrg_Map_Buf = (int *) coreRot->Get_ResToSrcMapImage(
		)->GetPixAt( 0, 0 );

	GradBuf_Far = a_rotMgr->GetGradMgr_Far(
		)->GetGradImg_ByNormIdx( a_nNormIdx )->GetPixAt( 0, 0 );

	ConfBuf_Far = a_rotMgr->GetGradMgr_Far(
		)->GetConfImg_ByNormIdx( a_nNormIdx )->GetPixAt( 0, 0 );

	GradBuf_Loc = a_rotMgr->GetGradMgr_Loc(
		)->GetGradImg_ByNormIdx( a_nNormIdx )->GetPixAt( 0, 0 );

	NbrBuf_Loc = & (a_rotMgr->GetImgNbrMgr_Loc(
		)->GetNbr_Arr_ByNormIdx( a_nNormIdx ))[0];

	BiMixGrad_Buf_Loc = a_rotMgr->GetBiMixGrad_Loc(
		)->GetResImg_ByNormIdx( a_nNormIdx )->GetPixAt( 0, 0 );
}



RotationMgr_Ex_Coll::RotationMgr_Ex_Coll( 
	ImageRotationMgr_ExParamsRef a_rotExParams, int a_nofRots )
{
	Hcpl_ASSERT( a_nofRots >= 0 );

	//Hcpl_ASSERT( 0 == a_nofRots % 2 );

	m_rotExParams = a_rotExParams;

	m_nofRots = a_nofRots;

	m_rotExArr.SetCapacity( a_nofRots );

	float angStep = 90.0F / a_nofRots;

	for( int i=0; i < a_nofRots; i++ )
	{
		m_rotExParams->AngDig = i * angStep;
		
		ImageRotationMgr_ExRef rotEx = new ImageRotationMgr_Ex( m_rotExParams );

		m_rotExArr.PushBack( rotEx );
	}

}


ScanDir_Coll::ScanDir_Coll( RotationMgr_Ex_CollRef a_rotColl )
{
	m_scanDirArr.SetSize( a_rotColl->GetNofRots() * 2 );
	
	int nDirIdx = 0;

	const int nofRots = a_rotColl->GetNofRots();

	for( int i=0; i < nofRots; i++, nDirIdx++ )
	{
		ScanDirRef sd1 = new ScanDir(
			a_rotColl->GetRotEx_At( i ), nDirIdx, 0, 0 );

		m_scanDirArr[ nDirIdx ] = sd1;
	}

	for( int i=0; i < a_rotColl->GetNofRots(); i++, nDirIdx++ )
	{
		ScanDirRef sd1 = new ScanDir(
			a_rotColl->GetRotEx_At( i ), nDirIdx, 1, 0 );

		m_scanDirArr[ nDirIdx ] = sd1;

		ScanDirRef sd1_Norm = m_scanDirArr[ nDirIdx - nofRots ];
				
		sd1->pNormSD = ( ScanDir * )sd1_Norm;
		sd1_Norm->pNormSD = ( ScanDir * )sd1;
	}

	PrepareMaxLocDif_Stuff();
}



void ScanDir_Coll::PrepareMaxLocDif_Stuff()
{
	CvSize imgSiz = m_scanDirArr[ 0 ]->GetSrcImgSiz();
		
	int nSrcSiz1D = imgSiz.width * imgSiz.height;

	//FixedVector< float > testArr;
	//testArr.SetSize( nSrcSiz1D );



	m_maxLocDif_Img = F32Image::Create( imgSiz, 1 );

	m_maxLocDif_Img->SetAll( 0 );

	float * maxDifBuf_Loc = m_maxLocDif_Img->GetPixAt( 0, 0 );



	for( int k=0; k < m_scanDirArr.GetSize(); k++ )
	{
		ScanDirRef sd1 = m_scanDirArr[ k ];

		sd1->MaxGrad_Buf_Loc = maxDifBuf_Loc;

		FixedVector< ImageLineItr > & rLineItrArr =
			sd1->pItrProv->GetLineItrArr();

		for( int nLineIdx = 0; nLineIdx < rLineItrArr.GetSize(); nLineIdx++ )
		{
			//ImageLineItr & rLineItr = sd1->pItrProv->GetItrForPixIdx( nFocusIdx_Rot );
			ImageLineItr & rLineItr = rLineItrArr[ nLineIdx ];

			NumberIterator itr = rLineItr.GenBgnToEndItr();

			for( int i = itr.CalcCurStep(); i < itr.GetLimStep(); i++, itr.MoveNext() )
			{
				int nCPI_Rot = itr.GetCurVal();
				int nCPI_Org = sd1->RotToOrg_Map_Buf[ nCPI_Rot ];

				float gradVal = sd1->GradBuf_Loc[ nCPI_Rot ];

				float maxGadVal = maxDifBuf_Loc[ nCPI_Org ];

				if( gradVal > maxGadVal )
				{
					Hcpl_ASSERT( nCPI_Org >= 0 );
					Hcpl_ASSERT( nCPI_Org < nSrcSiz1D );

					//testArr[ nCPI_Org ] = gradVal;

					maxDifBuf_Loc[ nCPI_Org ] = gradVal;
				}
			}
		}

	}

	//ShowImage( m_maxLocDif_Img, "m_maxLocDif_Img" );

	//GlobalStuff::SetLinePathImg(
	//	GenTriChGrayImg( m_maxLocDif_Img ) );
	//GlobalStuff::ShowLinePathImg();
	
}


ImgScanMgr::ImgScanMgr( ImageRotationMgr_ExParamsRef a_rotExParams, 
	int a_nofRots, CvPoint a_p1 )
{
	Init( a_rotExParams, a_nofRots, a_p1 );
}


void ImgScanMgr::Init( ImageRotationMgr_ExParamsRef a_rotExParams, 	
	int a_nofRots, CvPoint a_p1 )
{
	m_p1 = a_p1;
	//m_p1 = cvPoint( 216, 33 );
	//m_p1 = cvPoint( 109, 146 );


	m_rotExParams = a_rotExParams;

	m_rotEx_Coll = new RotationMgr_Ex_Coll( m_rotExParams, a_nofRots );

	m_scanDir_Coll = new ScanDir_Coll( m_rotEx_Coll );

	m_sigDrawArr.SetSize( 40 );
	m_edgeInfoArr.SetCapacity( 5000000 );

	{
		CvSize srcSiz = this->m_rotExParams->SrcImg->GetSize();

		m_pixLinkingArr.SetSize( srcSiz.width * srcSiz.height );

		for( int i=0; i < m_pixLinkingArr.GetSize(); i++ )
		{
			PixLinking & rPL = m_pixLinkingArr[ i ];

			rPL.nIOA = i;

			for( int j=0; j < 4; j++ )
			{
				PixLink & rLink = rPL.Links[ j ];

				rLink.nIOA_Peer = -1;
				rLink.Rate = 10000000;
			}
		}

		int nMaxLen = 
			( srcSiz.width > srcSiz.height ) ? srcSiz.width : srcSiz.height;

		m_mergeRateArr.SetCapacity( nMaxLen * nMaxLen * 1.2 );

		for( int i=0; i < m_sigDrawArr.GetSize(); i++ )
		{
			SigDrawRef draw = new SigDraw();
			draw->DataArr.SetCapacity( nMaxLen * 2 );

			m_sigDrawArr[ i ] = draw;
		}
	}


	Proceed();
}


void ImgScanMgr::Proceed_0()
{

	F32ImageRef img1 = this->m_rotExParams->SrcImg->Clone();
		
		//sd1->RotationMgr->GetCoreRotMgr()->GetSrcImg(
		//)->Clone();

	CvSize orgSiz = img1->GetSize();
	IndexCalc2D idxCalc_Org( orgSiz.width, orgSiz.height );


	for( int k=0; k < m_scanDir_Coll->GetNofScanDirs(); k++ )
	{
		ScanDirRef sd1 = m_scanDir_Coll->GetScanDir_At( k );

		int nFocusIdx_Rot;
		{	
			int nFocusIdx_Org = idxCalc_Org.Calc( 
				//orgSiz.width / 2, orgSiz.height / 2 );
				m_p1.x, m_p1.y );

			nFocusIdx_Rot = sd1->OrgToRot_Map_Buf[ nFocusIdx_Org ];
		}

		//FixedVector< ImageLineItr > & rLineItrArr =
			//sd1->pItrProv->GetLineItrArr();

		ImageLineItr & rLineItr = sd1->pItrProv->GetItrForPixIdx( nFocusIdx_Rot );

		NumberIterator itr = rLineItr.GenBgnToEndItr();

		for( int i = itr.CalcCurStep(); i < itr.GetLimStep(); i++, itr.MoveNext() )
		{
			int nCPI_Rot = itr.GetCurVal();
			int nCPI_Org = sd1->RotToOrg_Map_Buf[ nCPI_Rot ];
		
			F32Point p1;

			p1.x = idxCalc_Org.Calc_X( nCPI_Org );
			p1.y = idxCalc_Org.Calc_Y( nCPI_Org );

			int nColor = i;

			if( nColor > 255 )
				nColor = 255;

			//DrawCircle( img1, p1, u8ColorVal( 0, 180, 0 ) );
			DrawCircle( img1, p1, u8ColorVal( 0, nColor, 0 ) );
		}
	}


	ShowImage( img1, "img1" );


}


void ImgScanMgr::Proceed_01()
{
	F32ImageRef img1 = this->m_rotExParams->SrcImg->Clone();
		
		//sd1->RotationMgr->GetCoreRotMgr()->GetSrcImg(
		//)->Clone();

	CvSize orgSiz = img1->GetSize();
	IndexCalc2D idxCalc_Org( orgSiz.width, orgSiz.height );


	{
		SigDrawRef draw = m_sigDrawArr[ (int) PeakVal ];
		draw->Color = U8ColorVal( 255, 255, 255 );

		draw->DataArr.SetSize( 
			draw->DataArr.GetCapacity() );

		draw->DataArr.SetAllToVal( 0 );
		draw->IsEnabled = false;
	}


	const int nFocusDir = GlobalStuff::ScanDir;

	for( int k=0; k < m_scanDir_Coll->GetNofScanDirs(); k++ )
	{
		ScanDirRef sd1 = m_scanDir_Coll->GetScanDir_At( k );

		int nFocusIdx_Rot;
		//{	
			int nFocusIdx_Org = idxCalc_Org.Calc( 
				//orgSiz.width / 2, orgSiz.height / 2 );
				m_p1.x, m_p1.y );

			nFocusIdx_Rot = sd1->OrgToRot_Map_Buf[ nFocusIdx_Org ];
		//}
				

		FixedVector< ImageLineItr > & rLineItrArr =
			sd1->pItrProv->GetLineItrArr();

		const int nFocusDir = GlobalStuff::ScanDir;

		int nLineIdx_Focus = 
			sd1->pItrProv->GetItrForPixIdx( nFocusIdx_Rot ).GetMyIdx();

		//ImageLineItr & rLineItr = sd1->pItrProv->GetItrForPixIdx( nFocusIdx_Rot );

		for( int nLineIdx = 0; nLineIdx < rLineItrArr.GetSize(); nLineIdx++ )
		{
			//ImageLineItr & rLineItr = sd1->pItrProv->GetItrForPixIdx( nFocusIdx_Rot );
			ImageLineItr & rLineItr = rLineItrArr[ nLineIdx ];

			NumberIterator itr = rLineItr.GenBgnToEndItr();

			bool bDraw = true;

			if( nFocusDir != k )
				bDraw = false;

			if( rLineItr.GetMyIdx() != nLineIdx_Focus )
				bDraw = false;

			if( bDraw )
				bDraw = bDraw;

			int nBgn = itr.CalcCurStep();
			int nEnd = itr.GetLimStep() - 1;

			int nMarg = m_rotExParams->nAprSiz_Far / 2;

			int nBgn2 = nBgn + nMarg;
			int nEnd2 = nEnd - nMarg;

			int nMargIdxDif = nMarg * itr.GetStepVal();


			int nFocusIdx_Org_R1;
			{
				int nIdx = nFocusIdx_Rot - nMargIdxDif;
				
				nFocusIdx_Org_R1 = 
					sd1->RotToOrg_Map_Buf[ nIdx ];
			}

			int nFocusIdx_Org_R2;
			{
				int nIdx = nFocusIdx_Rot + nMargIdxDif;
				
				nFocusIdx_Org_R2 = 
					sd1->RotToOrg_Map_Buf[ nIdx ];
			}

			for( int i = nBgn; i <= nEnd; i++, itr.MoveNext() )
			{
				bool bIsConflict = false;
				//float peakVal = 0;

				int nCPI_Rot = itr.GetCurVal();
				int nCPI_Org = sd1->RotToOrg_Map_Buf[ nCPI_Rot ];

				if( 243209 == nCPI_Rot &&
					7 == k )
					i = i;

				int nCPI_Rot_R1 = nCPI_Rot - nMargIdxDif;
				int nCPI_Rot_R2 = nCPI_Rot + nMargIdxDif;

				ImageNbrMgr::PixIndexNbr & rNormNbr_Loc = 
					sd1->pNormSD->NbrBuf_Loc[ nCPI_Rot ];

				if( i < nBgn2 || i > nEnd2 )
					goto EndProc;

				float dif_Far = sd1->GradBuf_Far[ nCPI_Rot ];

				if( dif_Far < 20 )
					goto EndProc;

				int nCPI_R1_Org = sd1->RotToOrg_Map_Buf[ nCPI_Rot_R1 ];
				int nCPI_R2_Org = sd1->RotToOrg_Map_Buf[ nCPI_Rot_R2 ];

				{
					float maxGrad_Loc_R1 = 
						sd1->MaxGrad_Buf_Loc[ nCPI_R1_Org ];

					float maxGrad_Loc_R2 = 
						sd1->MaxGrad_Buf_Loc[ nCPI_R2_Org ];

					//float dif_Far_06 = dif_Far * 0.6;
					float dif_Far_06 = dif_Far * 0.4;
									
					if( maxGrad_Loc_R1 > dif_Far_06 ||
						maxGrad_Loc_R2 > dif_Far_06 )
					{
						goto EndProc;
					}
				}

				{
					float grad_Loc_E = 
						sd1->GradBuf_Loc[ nCPI_Rot ];

					float grad_Loc_R1 = 
						sd1->GradBuf_Loc[ nCPI_Rot_R1 ];

					float grad_Loc_R2 = 
						sd1->GradBuf_Loc[ nCPI_Rot_R2 ];

					//float grad_Loc_E_06 = grad_Loc_E * 0.6;
					float grad_Loc_E_06 = grad_Loc_E * 0.4;
									
					if( grad_Loc_R1 > grad_Loc_E_06 ||
						grad_Loc_R2 > grad_Loc_E_06 )
					{
						goto EndProc;
					}
				}

				{
					float normGrad_Loc_R1 = 
						sd1->pNormSD->GradBuf_Loc[ nCPI_Rot_R1 ];
					
					float normGrad_Loc_R2 = 
						sd1->pNormSD->GradBuf_Loc[ nCPI_Rot_R2 ];

					float dif_Far_04 = dif_Far * 0.4;
									
					if( normGrad_Loc_R1 > dif_Far_04 ||
						normGrad_Loc_R2 > dif_Far_04 )
					{
						goto EndProc;
					}
				}



				{									
					if( rNormNbr_Loc.nIdxBef < 0 || 
						rNormNbr_Loc.nIdxAft < 0 )
					{
						goto EndProc;
					}

					float dif_Far_N1 = 
						sd1->GradBuf_Far[ rNormNbr_Loc.nIdxBef ];

					float dif_Far_N2 = 
						sd1->GradBuf_Far[ rNormNbr_Loc.nIdxAft ];

					float maxVal = 
						( dif_Far_N1 > dif_Far_N2 ) ? dif_Far_N1 : dif_Far_N2;

					float nbrDif = fabs( 
						(float) ( dif_Far_N1 - dif_Far_N2 ) );

					if( nbrDif / maxVal > 0.2 )
						goto EndProc;
				}


				//int nNormNbr_Loc_Rot = 


				bIsConflict = true;

EndProc:
				
				if( bIsConflict )
				{
					m_edgeInfoArr.IncSize();
					
					EdgeInfo & rEI = m_edgeInfoArr.GetAtBack( 0 );
					rEI.Init();

					if( 11652 == rEI.nIndex )
						rEI.nIndex = rEI.nIndex;

					rEI.nIOA_Rot_E = nCPI_Rot;
					rEI.nIOA_Rot_R1 = nCPI_Rot_R1;
					rEI.nIOA_Rot_R2 = nCPI_Rot_R2;

					rEI.m_scanDir = sd1;
				}

				if( bDraw )
				{
					if( nFocusIdx_Rot == nCPI_Rot )
						m_nIOL_DrawFocus = i;
					
					{
						SigDrawRef draw = m_sigDrawArr[ (int) Dif_Far ];
						if( 0 == i ) draw->Color = U8ColorVal( 255, 0, 0 );

						draw->DataArr.PushBack( dif_Far );
						draw->IsEnabled = false;
					}

					{
						SigDrawRef draw = m_sigDrawArr[ (int) Dif_Loc ];
						if( 0 == i ) draw->Color = U8ColorVal( 0, 255, 255 );

						draw->DataArr.PushBack( sd1->GradBuf_Loc[ nCPI_Rot ] );
						draw->IsEnabled = false;
					}

					{
						SigDrawRef draw = m_sigDrawArr[ (int) Dif_Loc_Max ];
						if( 0 == i ) draw->Color = U8ColorVal( 0, 255, 255 );

						draw->DataArr.PushBack( sd1->MaxGrad_Buf_Loc[ nCPI_Org ] );
						//draw->IsEnabled = false;
					}

					{
						SigDrawRef draw = m_sigDrawArr[ (int) Dif_Loc_Norm ];
						if( 0 == i ) draw->Color = U8ColorVal( 0, 120, 255 );

						draw->DataArr.PushBack( 
							sd1->pNormSD->GradBuf_Loc[ nCPI_Rot ] );
						//draw->IsEnabled = false;
					}

					if( rNormNbr_Loc.nIdxBef >= 0 )
					{
						int nIdx = rNormNbr_Loc.nIdxBef;

						SigDrawRef draw = m_sigDrawArr[ (int) Dif_Far_NormNbr_1 ];
						if( 0 == i ) draw->Color = U8ColorVal( 180, 180, 0 );

						draw->DataArr.PushBack( 
							sd1->GradBuf_Far[ nIdx ] );
						draw->IsEnabled = false;
					}

					if( rNormNbr_Loc.nIdxAft >= 0 )
					{
						int nIdx = rNormNbr_Loc.nIdxAft;

						SigDrawRef draw = m_sigDrawArr[ (int) Dif_Far_NormNbr_2 ];
						if( 0 == i ) draw->Color = U8ColorVal( 180, 0, 180 );

						draw->DataArr.PushBack( 
							sd1->GradBuf_Far[ nIdx ] );
						draw->IsEnabled = false;
					}

					{
						SigDrawRef draw = m_sigDrawArr[ (int) PeakVal ];
						if( 0 == i ) 
						{
							draw->Color = U8ColorVal( 255, 255, 255 );
							draw->DataArr.SetSize( itr.GetLimStep() );
							draw->IsEnabled = true;
						}

						if( bIsConflict )
						{
							draw->DataArr[ i ] = 100;

							draw->DataArr[ i - nMarg ] = - 25;
							draw->DataArr[ i + nMarg ] = - 25;
						}
					}


					

/////////////
					{
						F32Point p1;

						p1.x = idxCalc_Org.Calc_X( nCPI_Org );
						p1.y = idxCalc_Org.Calc_Y( nCPI_Org );

						U8ColorVal color = u8ColorVal( 255, 0, 0 );

						//DrawCircle( img1, p1, u8ColorVal( 0, nColor, 0 ) );
						DrawCircle( img1, p1, color, 1 );
					}

					if( rNormNbr_Loc.nIdxBef >= 0 )
					{
						int nIdx_Org = 
							sd1->RotToOrg_Map_Buf[ rNormNbr_Loc.nIdxBef ];

						F32Point p1;

						p1.x = idxCalc_Org.Calc_X( nIdx_Org );
						p1.y = idxCalc_Org.Calc_Y( nIdx_Org );

						DrawCircle( img1, p1, 
							u8ColorVal( 255, 0, 0 ), 1 );
					}

					if( rNormNbr_Loc.nIdxAft >= 0 )
					{
						int nIdx_Org = 
							sd1->RotToOrg_Map_Buf[ rNormNbr_Loc.nIdxAft ];

						F32Point p1;

						p1.x = idxCalc_Org.Calc_X( nIdx_Org );
						p1.y = idxCalc_Org.Calc_Y( nIdx_Org );

						DrawCircle( img1, p1, 
							u8ColorVal( 255, 0, 0 ), 1 );
					}

				}
			}

			if( bDraw )
			{
				{
					F32Point p1;

					p1.x = idxCalc_Org.Calc_X( nFocusIdx_Org );
					p1.y = idxCalc_Org.Calc_Y( nFocusIdx_Org );

					U8ColorVal color = u8ColorVal( 0, 0, 255 );

					DrawCircle( img1, p1, color, 1 );
				}

				{
					F32Point p1;

					p1.x = idxCalc_Org.Calc_X( nFocusIdx_Org_R1 );
					p1.y = idxCalc_Org.Calc_Y( nFocusIdx_Org_R1 );

					U8ColorVal color = u8ColorVal( 0, 180, 0 );

					DrawCircle( img1, p1, color, 1 );
				}

				{
					F32Point p1;

					p1.x = idxCalc_Org.Calc_X( nFocusIdx_Org_R2 );
					p1.y = idxCalc_Org.Calc_Y( nFocusIdx_Org_R2 );

					U8ColorVal color = u8ColorVal( 0, 180, 0 );

					DrawCircle( img1, p1, color, 1 );
				}

			}

		}
	}


	FillPixLinkingArr();

	//ShowImage( img1, "img1" );
	GlobalStuff::ShowLinePathImg( img1 );

	DrawSignals();
}



void ImgScanMgr::Proceed()
{
	F32ImageRef img1 = this->m_rotExParams->SrcImg->Clone();
		
		//sd1->RotationMgr->GetCoreRotMgr()->GetSrcImg(
		//)->Clone();

	CvSize orgSiz = img1->GetSize();
	IndexCalc2D idxCalc_Org( orgSiz.width, orgSiz.height );


	{
		SigDrawRef draw = m_sigDrawArr[ (int) PeakVal ];
		draw->Color = U8ColorVal( 255, 255, 255 );

		draw->DataArr.SetSize( 
			draw->DataArr.GetCapacity() );

		draw->DataArr.SetAllToVal( 0 );
		draw->IsEnabled = false;
	}


	const int nFocusDir = GlobalStuff::ScanDir;

	for( int k=0; k < m_scanDir_Coll->GetNofScanDirs(); k++ )
	{
		ScanDirRef sd1 = m_scanDir_Coll->GetScanDir_At( k );

		int nFocusIdx_Rot;
		//{	
			int nFocusIdx_Org = idxCalc_Org.Calc( 
				//orgSiz.width / 2, orgSiz.height / 2 );
				m_p1.x, m_p1.y );

			nFocusIdx_Rot = sd1->OrgToRot_Map_Buf[ nFocusIdx_Org ];
		//}
				

		FixedVector< ImageLineItr > & rLineItrArr =
			sd1->pItrProv->GetLineItrArr();

		const int nFocusDir = GlobalStuff::ScanDir;

		int nLineIdx_Focus = 
			sd1->pItrProv->GetItrForPixIdx( nFocusIdx_Rot ).GetMyIdx();

		//ImageLineItr & rLineItr = sd1->pItrProv->GetItrForPixIdx( nFocusIdx_Rot );

		for( int nLineIdx = 0; nLineIdx < rLineItrArr.GetSize(); nLineIdx++ )
		{
			//ImageLineItr & rLineItr = sd1->pItrProv->GetItrForPixIdx( nFocusIdx_Rot );
			ImageLineItr & rLineItr = rLineItrArr[ nLineIdx ];

			NumberIterator itr = rLineItr.GenBgnToEndItr();

			bool bDraw = true;

			if( nFocusDir != k )
				bDraw = false;

			if( rLineItr.GetMyIdx() != nLineIdx_Focus )
				bDraw = false;

			if( bDraw )
				bDraw = bDraw;

			int nBgn = itr.CalcCurStep();
			int nEnd = itr.GetLimStep() - 1;

			int nMarg = m_rotExParams->nAprSiz_Far / 2;

			int nBgn2 = nBgn + nMarg;
			int nEnd2 = nEnd - nMarg;

			int nMargIdxDif = nMarg * itr.GetStepVal();


			int nFocusIdx_Org_R1;
			{
				int nIdx = nFocusIdx_Rot - nMargIdxDif;
				
				nFocusIdx_Org_R1 = 
					sd1->RotToOrg_Map_Buf[ nIdx ];
			}

			int nFocusIdx_Org_R2;
			{
				int nIdx = nFocusIdx_Rot + nMargIdxDif;
				
				nFocusIdx_Org_R2 = 
					sd1->RotToOrg_Map_Buf[ nIdx ];
			}

			for( int i = nBgn; i <= nEnd; i++, itr.MoveNext() )
			{
				bool bIsConflict = false;
				//float peakVal = 0;

				int nCPI_Rot = itr.GetCurVal();
				int nCPI_Org = sd1->RotToOrg_Map_Buf[ nCPI_Rot ];

				if( 243209 == nCPI_Rot &&
					7 == k )
					i = i;

				int nCPI_Rot_R1 = nCPI_Rot - nMargIdxDif;
				int nCPI_Rot_R2 = nCPI_Rot + nMargIdxDif;

				ImageNbrMgr::PixIndexNbr & rNormNbr_Loc = 
					sd1->pNormSD->NbrBuf_Loc[ nCPI_Rot ];

				if( i < nBgn2 || i > nEnd2 )
					goto EndProc;


				float conf_Far = sd1->ConfBuf_Far[ nCPI_Rot ];

				if( conf_Far > 50 )
				{
					bIsConflict = true;
				}
				goto EndProc;



				float dif_Far = sd1->GradBuf_Far[ nCPI_Rot ];

				if( dif_Far < 20 )
					goto EndProc;

				int nCPI_R1_Org = sd1->RotToOrg_Map_Buf[ nCPI_Rot_R1 ];
				int nCPI_R2_Org = sd1->RotToOrg_Map_Buf[ nCPI_Rot_R2 ];

				{
					float maxGrad_Loc_R1 = 
						sd1->MaxGrad_Buf_Loc[ nCPI_R1_Org ];

					float maxGrad_Loc_R2 = 
						sd1->MaxGrad_Buf_Loc[ nCPI_R2_Org ];

					//float dif_Far_06 = dif_Far * 0.6;
					float dif_Far_06 = dif_Far * 0.4;
									
					if( maxGrad_Loc_R1 > dif_Far_06 ||
						maxGrad_Loc_R2 > dif_Far_06 )
					{
						goto EndProc;
					}
				}

				{
					float grad_Loc_E = 
						sd1->GradBuf_Loc[ nCPI_Rot ];

					float grad_Loc_R1 = 
						sd1->GradBuf_Loc[ nCPI_Rot_R1 ];

					float grad_Loc_R2 = 
						sd1->GradBuf_Loc[ nCPI_Rot_R2 ];

					//float grad_Loc_E_06 = grad_Loc_E * 0.6;
					float grad_Loc_E_06 = grad_Loc_E * 0.4;
									
					if( grad_Loc_R1 > grad_Loc_E_06 ||
						grad_Loc_R2 > grad_Loc_E_06 )
					{
						goto EndProc;
					}
				}

				{
					float normGrad_Loc_R1 = 
						sd1->pNormSD->GradBuf_Loc[ nCPI_Rot_R1 ];
					
					float normGrad_Loc_R2 = 
						sd1->pNormSD->GradBuf_Loc[ nCPI_Rot_R2 ];

					float dif_Far_04 = dif_Far * 0.4;
									
					if( normGrad_Loc_R1 > dif_Far_04 ||
						normGrad_Loc_R2 > dif_Far_04 )
					{
						goto EndProc;
					}
				}



				{									
					if( rNormNbr_Loc.nIdxBef < 0 || 
						rNormNbr_Loc.nIdxAft < 0 )
					{
						goto EndProc;
					}

					float dif_Far_N1 = 
						sd1->GradBuf_Far[ rNormNbr_Loc.nIdxBef ];

					float dif_Far_N2 = 
						sd1->GradBuf_Far[ rNormNbr_Loc.nIdxAft ];

					float maxVal = 
						( dif_Far_N1 > dif_Far_N2 ) ? dif_Far_N1 : dif_Far_N2;

					float nbrDif = fabs( 
						(float) ( dif_Far_N1 - dif_Far_N2 ) );

					if( nbrDif / maxVal > 0.2 )
						goto EndProc;
				}


				//int nNormNbr_Loc_Rot = 


				bIsConflict = true;

EndProc:
				
				if( bIsConflict )
				{
					m_edgeInfoArr.IncSize();
					
					EdgeInfo & rEI = m_edgeInfoArr.GetAtBack( 0 );
					rEI.Init();

					if( 11652 == rEI.nIndex )
						rEI.nIndex = rEI.nIndex;

					rEI.nIOA_Rot_E = nCPI_Rot;
					rEI.nIOA_Rot_R1 = nCPI_Rot_R1;
					rEI.nIOA_Rot_R2 = nCPI_Rot_R2;

					rEI.m_scanDir = sd1;
				}

				if( bDraw )
				{
					if( nFocusIdx_Rot == nCPI_Rot )
						m_nIOL_DrawFocus = i;
					
					{
						SigDrawRef draw = m_sigDrawArr[ (int) Dif_Far ];
						if( 0 == i ) draw->Color = U8ColorVal( 255, 0, 0 );

						draw->DataArr.PushBack( dif_Far );
						draw->IsEnabled = false;
					}

					{
						SigDrawRef draw = m_sigDrawArr[ (int) Dif_Loc ];
						if( 0 == i ) draw->Color = U8ColorVal( 0, 255, 255 );

						draw->DataArr.PushBack( sd1->GradBuf_Loc[ nCPI_Rot ] );
						draw->IsEnabled = false;
					}

					{
						SigDrawRef draw = m_sigDrawArr[ (int) Dif_Loc_Max ];
						if( 0 == i ) draw->Color = U8ColorVal( 0, 255, 255 );

						draw->DataArr.PushBack( sd1->MaxGrad_Buf_Loc[ nCPI_Org ] );
						//draw->IsEnabled = false;
					}

					{
						SigDrawRef draw = m_sigDrawArr[ (int) Dif_Loc_Norm ];
						if( 0 == i ) draw->Color = U8ColorVal( 0, 120, 255 );

						draw->DataArr.PushBack( 
							sd1->pNormSD->GradBuf_Loc[ nCPI_Rot ] );
						draw->IsEnabled = false;
					}

					if( rNormNbr_Loc.nIdxBef >= 0 )
					{
						int nIdx = rNormNbr_Loc.nIdxBef;

						SigDrawRef draw = m_sigDrawArr[ (int) Dif_Far_NormNbr_1 ];
						if( 0 == i ) draw->Color = U8ColorVal( 180, 180, 0 );

						draw->DataArr.PushBack( 
							sd1->GradBuf_Far[ nIdx ] );
						draw->IsEnabled = false;
					}

					if( rNormNbr_Loc.nIdxAft >= 0 )
					{
						int nIdx = rNormNbr_Loc.nIdxAft;

						SigDrawRef draw = m_sigDrawArr[ (int) Dif_Far_NormNbr_2 ];
						if( 0 == i ) draw->Color = U8ColorVal( 180, 0, 180 );

						draw->DataArr.PushBack( 
							sd1->GradBuf_Far[ nIdx ] );
						draw->IsEnabled = false;
					}

					{
						SigDrawRef draw = m_sigDrawArr[ (int) PeakVal ];
						if( 0 == i ) 
						{
							draw->Color = U8ColorVal( 255, 255, 255 );
							draw->DataArr.SetSize( itr.GetLimStep() );
							draw->IsEnabled = true;
						}

						float conf_Far = sd1->ConfBuf_Far[ nCPI_Rot ];
						//float conf_Far = sd1->ConfBuf_Far[ i ];
						//float conf_Far = sd1->GradBuf_Far[ i ];
						//float conf_Far = sd1->GradBuf_Loc[ nCPI_Rot ];
						
						//draw->DataArr.PushBack( sd1->GradBuf_Loc[ nCPI_Rot ] );
						//draw->DataArr.PushBack( sd1->ConfBuf_Far[ nCPI_Rot ] );

						//draw->DataArr[ i ] = conf_Far;

						if( bIsConflict )
						//if( false )
						{
							draw->DataArr[ i ] = 100;

							//draw->DataArr[ i - nMarg ] = - 25;
							//draw->DataArr[ i + nMarg ] = - 25;
						}
					}


					

/////////////
					{
						F32Point p1;

						p1.x = idxCalc_Org.Calc_X( nCPI_Org );
						p1.y = idxCalc_Org.Calc_Y( nCPI_Org );

						U8ColorVal color = u8ColorVal( 255, 0, 0 );

						//DrawCircle( img1, p1, u8ColorVal( 0, nColor, 0 ) );
						DrawCircle( img1, p1, color, 1 );
					}

					if( rNormNbr_Loc.nIdxBef >= 0 )
					{
						int nIdx_Org = 
							sd1->RotToOrg_Map_Buf[ rNormNbr_Loc.nIdxBef ];

						F32Point p1;

						p1.x = idxCalc_Org.Calc_X( nIdx_Org );
						p1.y = idxCalc_Org.Calc_Y( nIdx_Org );

						DrawCircle( img1, p1, 
							u8ColorVal( 255, 0, 0 ), 1 );
					}

					if( rNormNbr_Loc.nIdxAft >= 0 )
					{
						int nIdx_Org = 
							sd1->RotToOrg_Map_Buf[ rNormNbr_Loc.nIdxAft ];

						F32Point p1;

						p1.x = idxCalc_Org.Calc_X( nIdx_Org );
						p1.y = idxCalc_Org.Calc_Y( nIdx_Org );

						DrawCircle( img1, p1, 
							u8ColorVal( 255, 0, 0 ), 1 );
					}

				}
			}

			if( bDraw )
			{
				{
					F32Point p1;

					p1.x = idxCalc_Org.Calc_X( nFocusIdx_Org );
					p1.y = idxCalc_Org.Calc_Y( nFocusIdx_Org );

					U8ColorVal color = u8ColorVal( 0, 0, 255 );

					DrawCircle( img1, p1, color, 1 );
				}

				{
					F32Point p1;

					p1.x = idxCalc_Org.Calc_X( nFocusIdx_Org_R1 );
					p1.y = idxCalc_Org.Calc_Y( nFocusIdx_Org_R1 );

					U8ColorVal color = u8ColorVal( 0, 180, 0 );

					DrawCircle( img1, p1, color, 1 );
				}

				{
					F32Point p1;

					p1.x = idxCalc_Org.Calc_X( nFocusIdx_Org_R2 );
					p1.y = idxCalc_Org.Calc_Y( nFocusIdx_Org_R2 );

					U8ColorVal color = u8ColorVal( 0, 180, 0 );

					DrawCircle( img1, p1, color, 1 );
				}

			}

		}
	}


	FillPixLinkingArr();

	//ShowImage( img1, "img1" );
	GlobalStuff::ShowLinePathImg( img1 );

	DrawSignals();
}


void ImgScanMgr::DrawSignals()
{
	//ThrowHcvException();

	Signal1DViewer sv1;

	int nDrawCnt = 0;

	for( int i=0; i < m_sigDrawArr.GetSize(); i++ )
	{
		SigDrawRef draw = m_sigDrawArr[ i ];

		if( NULL == draw )
			continue;

		if( 0 == draw->DataArr.GetSize() )
			continue;

		if( ! draw->IsEnabled )
			continue;

		Signal1DBuilder sb1( 3000 );

		sb1.AddData( & ( draw->DataArr )[ 0 ], 
			draw->DataArr.GetSize() );

		sv1.AddSignal( sb1.GetResult(), draw->Color );

		nDrawCnt++;
	}

	if( nDrawCnt > 0 )
	{
		sv1.AddSignal( DeltaSignal::CreateAt( m_nIOL_DrawFocus ),
			u8ColorVal( 0, 0, 255 ) );

		ShowImage( sv1.GenDisplayImage(), "Line Signals" );
	}
}


//void ImgScanMgr::FillMergeRateArr_FromDir( int a_nDirIdx )
void ImgScanMgr::FillPixLinkingArr()
{
	m_mergeRateArr.ResetSize();

	for( int k=0; k < m_scanDir_Coll->GetNofScanDirs(); k++ )
	{
		//ScanDirRef sd1 = m_scanDir_Coll->GetScanDir_At( a_nDirIdx );
		ScanDirRef sd1 = m_scanDir_Coll->GetScanDir_At( k );

		FixedVector< ImageLineItr > & rLineItrArr =
			sd1->pItrProv->GetLineItrArr();

		CvSize srcSiz = this->m_rotExParams->SrcImg->GetSize();

		FixedVector< int > linkMap;
		{
			linkMap.SetSize( srcSiz.width + 10 );

			for( int i=0; i < linkMap.GetSize(); i++ )
				linkMap[ i ] = -1;

			linkMap[ 1 ] = 0;
			linkMap[ srcSiz.width + 1 ] = 1;
			linkMap[ srcSiz.width ] = 2;
			linkMap[ srcSiz.width - 1 ] = 3;
		}

		for( int nLineIdx = 0; nLineIdx < rLineItrArr.GetSize(); nLineIdx++ )
		{
			ImageLineItr & rLineItr = rLineItrArr[ nLineIdx ];

			NumberIterator itr = rLineItr.GenItr(
				rLineItr.GetBgn(), rLineItr.GetAftEnd( -1 ) );

			int nIdxDif = itr.GetStepVal();

			for( int i = itr.CalcCurStep(); i < itr.GetLimStep(); i++, itr.MoveNext() )
			{
				int nCPI_Rot_1 = itr.GetCurVal();
				int nCPI_Org_1 = sd1->RotToOrg_Map_Buf[ nCPI_Rot_1 ];

				int nCPI_Rot_2 = nCPI_Rot_1 + nIdxDif;
				int nCPI_Org_2 = sd1->RotToOrg_Map_Buf[ nCPI_Rot_2 ];

				float biGrad_1 = sd1->BiMixGrad_Buf_Loc[ nCPI_Rot_1 ];
				float biGrad_2 = sd1->BiMixGrad_Buf_Loc[ nCPI_Rot_2 ];

				float rate = 
					( biGrad_1 < biGrad_2 ) ? biGrad_1 : biGrad_2;

				if( nCPI_Org_1 == nCPI_Org_2 )
					continue;

				//Hcpl_ASSERT( nCPI_Org_1 <= nCPI_Org_2 );

				int nIOA_Min;
				int nIOA_Max;
				
				if( nCPI_Org_1 < nCPI_Org_2 )
				{
					nIOA_Min = nCPI_Org_1;
					nIOA_Max = nCPI_Org_2;
				}
				else
				{
					nIOA_Min = nCPI_Org_2;
					nIOA_Max = nCPI_Org_1;
				}

				int nLinkIdx = linkMap[ nIOA_Max - nIOA_Min ];

				PixLinking & rPL = m_pixLinkingArr[ nIOA_Min ];
				PixLink & rLink = rPL.Links[ nLinkIdx ];

				rLink.nIOA_Peer = nIOA_Max;

				if( rLink.Rate > rate )
					rLink.Rate = rate;

				Hcpl_ASSERT( rate >= 0 );

				Hcpl_ASSERT( rLink.Rate >= 0 );
			 
				//m_mergeRateArr.IncSize();
				//MergeRateElm & rElm = m_mergeRateArr.GetAtBack( 0 );

				//rElm.nIOA_1 = nCPI_Org_1;
				//rElm.nIOA_2 = nCPI_Org_2;

				//rElm.Rate = rate;
			}
		}

	}


}




//}