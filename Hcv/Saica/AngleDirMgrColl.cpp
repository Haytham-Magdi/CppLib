#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\Image_Operations.h>
#include <Lib\Hcv\Saica\AngleDirMgrColl.h>
#include <Lib\Hcv\Saica\PixelInfo_1.h>


#define M_PI 3.14159265358979323846


namespace Hcv
{
	using namespace Hcpl;
	//using namespace Hcpl::Math;

	using namespace Hcv;
	using namespace Hcv::Image_Operations;
	//using namespace Hcv::ImageAccessorOperations;

	namespace Ns_Saica
	{
		AngleDirMgrColl::AngleDirMgrColl(RotationMgrCollRef a_rotMgrColl)
		{
			m_rotMgrColl = a_rotMgrColl;
			m_context_H = new AngleDirMgrColl_Context();
			m_context_V = new AngleDirMgrColl_Context();

			Prepare();
		}

		void AngleDirMgrColl::Prepare()
		{
			F32ImageAccessor3C_Ref org_Img_H = new F32ImageAccessor3C(m_rotMgrColl->GetRotAt(0)->GetSrcImg());
			//F32ImageAccessor3C_Ref org_Img_V = org_Img_H->CloneAccessorOnly(); org_Img_V->SwitchXY();

			ShowImage(org_Img_H->GetSrcImg(), "org_Img_H->GetSrcImg()");

			m_context_H->m_org_Img = org_Img_H;
			m_context_H->m_standevInfoImg = new TempImageAccessor<PixelStandevInfo>(
				org_Img_H->GetMemAccessor()->GetOffsetCalc());
			{
				PixelStandevInfo initPsi;
				initPsi.Dir = -1;
				initPsi.Val = 10000000;
				initPsi.NormVal = 0;
				FillImage(m_context_H->m_standevInfoImg->GetMemAccessor(), initPsi);
			}

			m_context_H->m_conflictInfoImg = new TempImageAccessor<ConflictInfo_Ex>(
				org_Img_H->GetMemAccessor()->GetOffsetCalc());
			{
				ConflictInfo_Ex ci_Init;
				ci_Init.Dir = -1;
				ci_Init.Exists = false;
				FillImage(m_context_H->m_conflictInfoImg->GetMemAccessor(), ci_Init);
			}

			m_context_H->m_wideConflictDiff_Img = new F32ImageAccessor1C(
				org_Img_H->GetMemAccessor()->GetOffsetCalc());
			{
				float initVal = -10000000;
				FillImage(m_context_H->m_wideConflictDiff_Img->GetMemAccessor(), initVal);
			}

			m_context_V->m_org_Img = m_context_H->m_org_Img->CloneAccessorOnly(); m_context_V->m_org_Img->SwitchXY();
			m_context_V->m_standevInfoImg = m_context_H->m_standevInfoImg->CloneAccessorOnly(); m_context_V->m_standevInfoImg->SwitchXY();
			m_context_V->m_conflictInfoImg = m_context_H->m_conflictInfoImg->CloneAccessorOnly(); m_context_V->m_conflictInfoImg->SwitchXY();
			m_context_V->m_wideConflictDiff_Img = m_context_H->m_wideConflictDiff_Img->CloneAccessorOnly(); m_context_V->m_wideConflictDiff_Img->SwitchXY();
			
			m_angleDirMgrArr.SetSize(m_rotMgrColl->GetNofRots() * 2);

			for (int i = 0; i < m_rotMgrColl->GetNofRots(); i++)
			{
				ImgRotationMgrRef rotMgr = m_rotMgrColl->GetRotAt(i);

				F32ImageAccessor3C_Ref rot_Img_H = new F32ImageAccessor3C(rotMgr->GetResImg());

				F32ImageAccessor1C_Ref magSqr_Img_H = new F32ImageAccessor1C(rot_Img_H->GetOffsetCalc());
				CalcMagSqrImage(rot_Img_H->GetMemAccessor(), magSqr_Img_H->GetMemAccessor());

				ImgAngleDirMgr_Context_Ref dirContext_H = new ImgAngleDirMgr::Context(i, rotMgr,
					rot_Img_H, magSqr_Img_H, 'H');

				dirContext_H->m_rotToOrgMap_Img = new S32ImageAccessor1C(rotMgr->Get_ResToSrcMapImage());
				dirContext_H->m_orgToRotMap_Img = new S32ImageAccessor1C(rotMgr->Get_SrcToResMapImage());

				dirContext_H->m_angle = rotMgr->GetAngleByRad();

				//dirContext_H->m_conflict_Img = new TempImageAccessor<ConflictInfo>(
				//	rot_Img_H->GetMemAccessor()->GetOffsetCalc());

				F32ImageAccessor3C_Ref rot_Img_V = rot_Img_H->CloneAccessorOnly(); rot_Img_V->SwitchXY();
				F32ImageAccessor1C_Ref magSqr_Img_V = magSqr_Img_H->CloneAccessorOnly(); magSqr_Img_V->SwitchXY();

				ImgAngleDirMgr_Context_Ref dirContext_V = new ImgAngleDirMgr::Context(i + m_rotMgrColl->GetNofRots(),
					rotMgr, rot_Img_V, magSqr_Img_V, 'V');

				dirContext_V->m_rotToOrgMap_Img = dirContext_H->m_rotToOrgMap_Img->CloneAccessorOnly(); dirContext_V->m_rotToOrgMap_Img->SwitchXY();
				dirContext_V->m_orgToRotMap_Img = dirContext_H->m_orgToRotMap_Img->CloneAccessorOnly(); dirContext_V->m_orgToRotMap_Img->SwitchXY();
				dirContext_V->m_angle = rotMgr->GetAngleByRad() + M_PI / 2;
				//dirContext_V->m_conflict_Img = dirContext_H->m_conflict_Img->CloneAccessorOnly(); dirContext_V->m_conflict_Img->SwitchXY();

				ImgAngleDirMgrRef angleDirMgr_H = new ImgAngleDirMgr(dirContext_H, dirContext_V, m_context_H);
				m_angleDirMgrArr[i] = angleDirMgr_H;

				ImgAngleDirMgrRef angleDirMgr_V = new ImgAngleDirMgr(dirContext_V, dirContext_H, m_context_V);
				m_angleDirMgrArr[i + m_rotMgrColl->GetNofRots()] = angleDirMgr_V;
			}

			for (int i = 0; i < m_angleDirMgrArr.GetSize(); i++) {
				m_angleDirMgrArr[i]->Proceed_1();
			}

			for (int i = 0; i < m_angleDirMgrArr.GetSize(); i++) {
				m_angleDirMgrArr[i]->Proceed_2();
			}

			for (int i = 0; i < m_angleDirMgrArr.GetSize(); i++) {
				m_angleDirMgrArr[i]->Proceed_3();
			}

			for (int i = 0; i < m_angleDirMgrArr.GetSize(); i++) {
				m_angleDirMgrArr[i]->Proceed_4();
			}

			for (int i = 0; i < m_angleDirMgrArr.GetSize(); i++) {
				m_angleDirMgrArr[i]->Proceed_5();
			}

			for (int i = 0; i < m_angleDirMgrArr.GetSize(); i++) {
				m_angleDirMgrArr[i]->Proceed_6();
			}

			DisplayStandiv_Dir_Img();

			DisplayConflictImg();

			//ShowImage(standev_InrWide_Img->GetSrcImg(), "standev_InrWide_Img->GetSrcImg()");

			GlobalStuff::SetLinePathImg(GenTriChGrayImg(m_context_H->m_wideConflictDiff_Img->GetSrcImg())); GlobalStuff::ShowLinePathImg();
			ShowImage(m_context_H->m_wideConflictDiff_Img->GetSrcImg(), "m_wideConflictDiff_Img->GetSrcImg()");

			SaveImage(m_context_H->m_wideConflictDiff_Img->GetSrcImg(), "m_wideConflictDiff_Img.jpg");

						

			ManageThresholding();

		}


		void AngleDirMgrColl::DisplayStandiv_Dir_Img()
		{
			MemAccessor_2D_REF(PixelStandevInfo) psiAcc = m_context_H->m_standevInfoImg->GetMemAccessor();
			//F32ImageRef dspImg_Values = F32Image::Create(cvSize(psiAcc->GetIndexSize_X(), psiAcc->GetIndexSize_Y()), 1);
			F32ImageRef dspImg_Colored = F32Image::Create(cvSize(psiAcc->GetIndexSize_X(), psiAcc->GetIndexSize_Y()), 3);

			const int nSize_1D = psiAcc->GetIndexSize_X() * psiAcc->GetIndexSize_Y();

			PixelStandevInfo * srcPtr = psiAcc->GetDataPtr();
			//float * destPtr_Values = dspImg_Values->GetDataPtr();
			F32ColorVal * destPtr_Colored = (F32ColorVal *)dspImg_Colored->GetDataPtr();

			float angle_Old = -1;
			for (int i = 0; i < nSize_1D; i++)
			{
				PixelStandevInfo & rSrc = srcPtr[i];
				//float & rDest_Values = destPtr_Values[i];
				F32ColorVal & rDest_Colored = destPtr_Colored[i];

				Hcpl_ASSERT(-1 != rSrc.Dir);

				float angle = m_angleDirMgrArr[rSrc.Dir]->GetContext()->m_angle;

				if (fabs(angle - angle_Old) > 0.01)
				{
					angle = angle;
				}
				angle_Old = angle;

				////rDest.val0 = 127 + rSrc.NormVal / 2;
				//rDest.val0 = 127;
				//rDest.val1 = (127 + 127 * cos(angle) * rSrc.NormVal * 2 / 3);
				//rDest.val2 = (127 + 127 * sin(angle) * rSrc.NormVal * 2 / 3);

				//rDest_Values = rSrc.NormVal;

				rDest_Colored.val0 = 0;
				//rDest.val1 = (fabs(cos(angle)) * rSrc.NormVal * 2 / 3);
				//rDest.val2 = (fabs(sin(angle)) * rSrc.NormVal * 2 / 3);
				rDest_Colored.val1 = (fabs(cos(angle)) * rSrc.NormVal * 5 / 3);
				rDest_Colored.val2 = (fabs(sin(angle)) * rSrc.NormVal * 5 / 3);
				//rDest.val1 = (fabs(cos(angle)) * rSrc.NormVal);
				//rDest.val2 = (fabs(sin(angle)) * rSrc.NormVal);

				////if (0 == rSrc.Dir)
				//if (5 == rSrc.Dir)
				////if (false)
				//{
				//	rDest.val0 = rSrc.NormVal;
				//	rDest.val1 = rSrc.NormVal;
				//	rDest.val2 = rSrc.NormVal;
				//}
				//else
				//{
				//	rDest.val0 = 0;
				//	rDest.val1 = 0;
				//	rDest.val2 = 0;
				//}

				//rDest.val0 = 127 + rSrc.NormVal;
				//rDest.val1 = 127 + 127 * cos(angle);
				//rDest.val2 = 127 + 127 * sin(angle);
			}

			//GlobalStuff::SetLinePathImg(GenTriChGrayImg(dspImg_Values));
			//GlobalStuff::ShowLinePathImg();

			//ShowImage(dspImg_Values, "dspImg_Colored");
		}

		void AngleDirMgrColl::DisplayConflictImg()
		{
			MemAccessor_2D_REF(ConflictInfo_Ex) confAcc = m_context_H->m_conflictInfoImg->GetMemAccessor();
			F32ImageRef confDsp_Img = F32Image::Create(cvSize(confAcc->GetIndexSize_X_Org(), confAcc->GetIndexSize_Y_Org()), 3);

			confDsp_Img->SetAll(0);

			const int nSize_1D = confAcc->GetIndexSize_X() * confAcc->GetIndexSize_Y();

			F32ColorVal * destPtr = (F32ColorVal *)confDsp_Img->GetDataPtr();
			ConflictInfo_Ex * srcPtr = confAcc->GetDataPtr();

			float angle_Old = -1;
			for (int i = 0; i < nSize_1D; i++)
			{
				ConflictInfo_Ex & rSrc = srcPtr[i];
				F32ColorVal & rDest = destPtr[i];

				//Hcpl_ASSERT(-1 != rSrc.Dir);

				if (rSrc.Exists)
				{
					F32ColorVal & rDest_Side_1 = destPtr[rSrc.Offset_Side_1];
					F32ColorVal & rDest_Side_2 = destPtr[rSrc.Offset_Side_2];

					rDest.val0 = 0;
					rDest.val1 = 0;
					rDest.val2 = 255;

					rDest_Side_1.val0 = 0;
					rDest_Side_1.val1 = 255;
					rDest_Side_1.val2 = 0;

					rDest_Side_2.val0 = 0;
					rDest_Side_2.val1 = 255;
					rDest_Side_2.val2 = 0;
				}
				//else
				//{
				//	rDest.val0 = 0;
				//	rDest.val1 = 0;
				//	rDest.val2 = 0;
				//}
			}

			ShowImage(confDsp_Img, "confDsp_Img->GetSrcImg()");
		}


		void AngleDirMgrColl::ManageThresholding_0()
		{
			AngleDirMgrColl_Context & cx = *m_context_H;

			F32ColorVal * orgImg_Ptr = (F32ColorVal *)cx.m_org_Img->GetDataPtr();
			ConflictInfo_Ex * conf_Ptr = cx.m_conflictInfoImg->GetDataPtr();

			const int nSize_1D = cx.m_conflictInfoImg->GetSize_1D();

			F32ImageAccessor3C_Ref threshold_Img = new F32ImageAccessor3C(cx.m_org_Img->GetOffsetCalc());
			F32ColorVal * threshold_Ptr = (F32ColorVal *)threshold_Img->GetDataPtr();

			F32ImageAccessor1C_Ref weight_Img = new F32ImageAccessor1C(cx.m_org_Img->GetOffsetCalc());
			float * weight_Ptr = weight_Img->GetDataPtr();
			{
				{
					float init_Val = 0.0f;
					FillImage(weight_Img->GetMemAccessor(), init_Val);
				}

				for (int i = 0; i < nSize_1D; i++)
				{
					ConflictInfo_Ex & rConf = conf_Ptr[i];
					float & rWeight = weight_Ptr[i];
					F32ColorVal & rThreshold = threshold_Ptr[i];

					if (!rConf.Exists)
					{
						rWeight = 0;
						rThreshold.AssignVal(0, 0, 0);
						continue;
					}

					rWeight = 1;

					F32ColorVal & rVal_Side_1 = orgImg_Ptr[rConf.Offset_Side_1];
					F32ColorVal & rVal_Side_2 = orgImg_Ptr[rConf.Offset_Side_2];

					rThreshold = F32ColorVal::Add(rVal_Side_1, rVal_Side_2).DividBy(2);

					//avg_Wide_Mag_Diff_Ptr[i] = fabs(mag_Ptr[i] - avg_Wide_Ptr[i]);
				}
			}

			//GlobalStuff::SetLinePathImg(GenTriChGrayImg(threshold_Mag_Img->GetSrcImg())); GlobalStuff::ShowLinePathImg();
			//ShowImage(threshold_Img->GetSrcImg(), "threshold_Img->GetSrcImg()");

			F32ImageAccessor3C_Ref avg_Threshold_Img = new F32ImageAccessor3C(cx.m_org_Img->GetOffsetCalc());
			{
				//const int nWinRadius = 5;
				//const int nWinRadius = 8;
				const int nWinRadius = 15;
				AvgImage_Weighted(threshold_Img->GetMemAccessor(), weight_Img->GetMemAccessor(), avg_Threshold_Img->GetMemAccessor(),
					Window<int>::New(-nWinRadius, nWinRadius, -nWinRadius, nWinRadius));
			}
			ShowImage(avg_Threshold_Img->GetSrcImg(), "avg_Threshold_Img->GetSrcImg()");

			F32ImageAccessor1C_Ref mag_Avg_Threshold_Img = new F32ImageAccessor1C(cx.m_org_Img->GetOffsetCalc());
			CalcMagImage(avg_Threshold_Img->GetMemAccessor(), mag_Avg_Threshold_Img->GetMemAccessor());
			float * mag_Avg_Threshold_Ptr = mag_Avg_Threshold_Img->GetDataPtr();

			ShowImage(mag_Avg_Threshold_Img->GetSrcImg(), "mag_Avg_Threshold_Img->GetSrcImg()");

			F32ImageAccessor1C_Ref magImg = new F32ImageAccessor1C(cx.m_org_Img->GetOffsetCalc());
			CalcMagImage(cx.m_org_Img->GetMemAccessor(), magImg->GetMemAccessor());
			float * mag_Ptr = magImg->GetDataPtr();

			F32ImageAccessor1C_Ref bin_Img = new F32ImageAccessor1C(cx.m_org_Img->GetOffsetCalc());
			{
				//{
				//	float init_Val = 0.0f;
				//	FillImage(bin_Img->GetMemAccessor(), init_Val);
				//}
				float * bin_Ptr = bin_Img->GetDataPtr();

				for (int i = 0; i < nSize_1D; i++)
				{
					float & rMag_Avg_Threshold = mag_Avg_Threshold_Ptr[i];
					float & rMag = mag_Ptr[i];
					float & rBin = bin_Ptr[i];

					if (rMag_Avg_Threshold < 5.0)
					{
						rBin = 128;
						continue;
					}

					if (rMag > rMag_Avg_Threshold)
					{
						rBin = 255;
					}
					else
					{
						rBin = 0;
					}
				}
			}
			ShowImage(bin_Img->GetSrcImg(), "bin_Img->GetSrcImg()");


			//SaveImage(GenTriChGrayImg(bin_Img->GetSrcImg()), "E:\\bin_Img.jpg");
			SaveImage(GenTriChGrayImg(bin_Img->GetSrcImg()), "bin_Img.jpg");


			//GlobalStuff::SetLinePathImg(GenTriChGrayImg(avg_Wide_Mag_Diff_Img->GetSrcImg())); GlobalStuff::ShowLinePathImg();
			//ShowImage(standev_Thin_Img->GetSrcImg(), "avg_Wide_Mag_Diff_Img->GetSrcImg()");






		}

		void AngleDirMgrColl::ManageThresholding()
		{
			AngleDirMgrColl_Context & cx = *m_context_H;

			F32ColorVal * orgImg_Ptr = (F32ColorVal *)cx.m_org_Img->GetDataPtr();


			TempImageAccessor_REF(PixelInfo_1) rgnGrow_Img = new TempImageAccessor<PixelInfo_1>(
				cx.m_org_Img->GetOffsetCalc());

			const int nQueScale = 10;

			MemSimpleAccessor_2D<float> sac_WideOutStandev = cx.m_wideConflictDiff_Img->GetMemAccessor()->GenSimpleAccessor();
			MemSimpleAccessor_2D<ConflictInfo_Ex> sac_Conflicts = cx.m_conflictInfoImg->GetMemAccessor()->GenSimpleAccessor();

			MultiListQueMgr< PixelInfo_1 > rgnGrowQues;
			rgnGrowQues.InitSize(700 * nQueScale + 2);

			MemSimpleAccessor_2D<PixelInfo_1> sac_RgnGrow = rgnGrow_Img->GetMemAccessor()->GenSimpleAccessor();

			for (int y = 0; y < sac_RgnGrow.GetSize_Y(); y++)
			{
				for (int x = 0; x < sac_RgnGrow.GetSize_X(); x++)
				{
					PixelInfo_1 & rPixInfo = sac_RgnGrow.GetAt(x, y);

					rPixInfo.X = x;
					rPixInfo.Y = y;

					rPixInfo.IsProcessed = false;

					rPixInfo.pConflictInfo = &sac_Conflicts.GetAt(x, y);

					rPixInfo.Val_WideOutStandev = sac_WideOutStandev.GetAt(x, y);
					if (rPixInfo.pConflictInfo->Exists)
					{
						rgnGrowQues.PushPtr(rPixInfo.Val_WideOutStandev * nQueScale, &rPixInfo);
					}
				}
			}


			return;


			//AngleDirMgrColl_Context & cx = *m_context_H;

			//F32ColorVal * orgImg_Ptr = (F32ColorVal *)cx.m_org_Img->GetDataPtr();
			ConflictInfo_Ex * conf_Ptr = cx.m_conflictInfoImg->GetDataPtr();

			const int nSize_1D = cx.m_conflictInfoImg->GetSize_1D();

			F32ImageAccessor3C_Ref threshold_Img = new F32ImageAccessor3C(cx.m_org_Img->GetOffsetCalc());
			F32ColorVal * threshold_Ptr = (F32ColorVal *)threshold_Img->GetDataPtr();

			F32ImageAccessor1C_Ref weight_Img = new F32ImageAccessor1C(cx.m_org_Img->GetOffsetCalc());
			float * weight_Ptr = weight_Img->GetDataPtr();
			{
				{
					float init_Val = 0.0f;
					FillImage(weight_Img->GetMemAccessor(), init_Val);
				}

				for (int i = 0; i < nSize_1D; i++)
				{
					ConflictInfo_Ex & rConf = conf_Ptr[i];
					float & rWeight = weight_Ptr[i];
					F32ColorVal & rThreshold = threshold_Ptr[i];

					if (!rConf.Exists)
					{
						rWeight = 0;
						rThreshold.AssignVal(0, 0, 0);
						continue;
					}

					rWeight = 1;

					F32ColorVal & rVal_Side_1 = orgImg_Ptr[rConf.Offset_Side_1];
					F32ColorVal & rVal_Side_2 = orgImg_Ptr[rConf.Offset_Side_2];

					rThreshold = F32ColorVal::Add(rVal_Side_1, rVal_Side_2).DividBy(2);

					//avg_Wide_Mag_Diff_Ptr[i] = fabs(mag_Ptr[i] - avg_Wide_Ptr[i]);
				}
			}

			//GlobalStuff::SetLinePathImg(GenTriChGrayImg(threshold_Mag_Img->GetSrcImg())); GlobalStuff::ShowLinePathImg();
			//ShowImage(threshold_Img->GetSrcImg(), "threshold_Img->GetSrcImg()");

			F32ImageAccessor3C_Ref avg_Threshold_Img = new F32ImageAccessor3C(cx.m_org_Img->GetOffsetCalc());
			{
				//const int nWinRadius = 5;
				//const int nWinRadius = 8;
				const int nWinRadius = 15;
				AvgImage_Weighted(threshold_Img->GetMemAccessor(), weight_Img->GetMemAccessor(), avg_Threshold_Img->GetMemAccessor(),
					Window<int>::New(-nWinRadius, nWinRadius, -nWinRadius, nWinRadius));
			}
			ShowImage(avg_Threshold_Img->GetSrcImg(), "avg_Threshold_Img->GetSrcImg()");

			F32ImageAccessor1C_Ref mag_Avg_Threshold_Img = new F32ImageAccessor1C(cx.m_org_Img->GetOffsetCalc());
			CalcMagImage(avg_Threshold_Img->GetMemAccessor(), mag_Avg_Threshold_Img->GetMemAccessor());
			float * mag_Avg_Threshold_Ptr = mag_Avg_Threshold_Img->GetDataPtr();

			ShowImage(mag_Avg_Threshold_Img->GetSrcImg(), "mag_Avg_Threshold_Img->GetSrcImg()");

			F32ImageAccessor1C_Ref magImg = new F32ImageAccessor1C(cx.m_org_Img->GetOffsetCalc());
			CalcMagImage(cx.m_org_Img->GetMemAccessor(), magImg->GetMemAccessor());
			float * mag_Ptr = magImg->GetDataPtr();

			F32ImageAccessor1C_Ref bin_Img = new F32ImageAccessor1C(cx.m_org_Img->GetOffsetCalc());
			{
				//{
				//	float init_Val = 0.0f;
				//	FillImage(bin_Img->GetMemAccessor(), init_Val);
				//}
				float * bin_Ptr = bin_Img->GetDataPtr();

				for (int i = 0; i < nSize_1D; i++)
				{
					float & rMag_Avg_Threshold = mag_Avg_Threshold_Ptr[i];
					float & rMag = mag_Ptr[i];
					float & rBin = bin_Ptr[i];

					if (rMag_Avg_Threshold < 5.0)
					{
						rBin = 128;
						continue;
					}

					if (rMag > rMag_Avg_Threshold)
					{
						rBin = 255;
					}
					else
					{
						rBin = 0;
					}
				}
			}
			ShowImage(bin_Img->GetSrcImg(), "bin_Img->GetSrcImg()");


			//SaveImage(GenTriChGrayImg(bin_Img->GetSrcImg()), "E:\\bin_Img.jpg");
			SaveImage(GenTriChGrayImg(bin_Img->GetSrcImg()), "bin_Img.jpg");


			//GlobalStuff::SetLinePathImg(GenTriChGrayImg(avg_Wide_Mag_Diff_Img->GetSrcImg())); GlobalStuff::ShowLinePathImg();
			//ShowImage(standev_Thin_Img->GetSrcImg(), "avg_Wide_Mag_Diff_Img->GetSrcImg()");






		}



	}
}

