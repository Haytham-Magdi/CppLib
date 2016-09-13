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

			m_context_V->m_standevInfoImg = m_context_H->m_standevInfoImg->CloneAccessorOnly(); m_context_V->m_standevInfoImg->SwitchXY();
			m_context_V->m_conflictInfoImg = m_context_H->m_conflictInfoImg->CloneAccessorOnly(); m_context_V->m_conflictInfoImg->SwitchXY();

			m_angleDirMgrArr.SetSize(m_rotMgrColl->GetNofRots() * 2);

			for (int i = 0; i < m_rotMgrColl->GetNofRots(); i++)
			{
				ImgRotationMgrRef rotMgr = m_rotMgrColl->GetRotAt(i);

				F32ImageAccessor3C_Ref rot_Img_H = new F32ImageAccessor3C(rotMgr->GetResImg());

				F32ImageAccessor1C_Ref magSqr_Img_H = new F32ImageAccessor1C(rot_Img_H->GetOffsetCalc());
				CalcMagSqrImage(rot_Img_H->GetMemAccessor(), magSqr_Img_H->GetMemAccessor());

				ImgAngleDirMgr_Context_Ref dirContext_H = new ImgAngleDirMgr::Context(i, rotMgr,
					rot_Img_H, magSqr_Img_H, 'H');

				//dirContext_H->m_rotToOrgMap_Img = new S32ImageAccessor1C(rotMgr->Get_ResToSrcMapImage());
				dirContext_H->m_orgToRotMap_Img = new S32ImageAccessor1C(rotMgr->Get_SrcToResMapImage());

				dirContext_H->m_angle = rotMgr->GetAngleByRad();

				dirContext_H->m_conflict_Img = new TempImageAccessor<bool>(
					rot_Img_H->GetMemAccessor()->GetOffsetCalc());


				F32ImageAccessor3C_Ref rot_Img_V = rot_Img_H->CloneAccessorOnly(); rot_Img_V->SwitchXY();
				F32ImageAccessor1C_Ref magSqr_Img_V = magSqr_Img_H->CloneAccessorOnly(); magSqr_Img_V->SwitchXY();

				ImgAngleDirMgr_Context_Ref dirContext_V = new ImgAngleDirMgr::Context(i + m_rotMgrColl->GetNofRots(),
					rotMgr, rot_Img_V, magSqr_Img_V, 'V');

				//dirContext_V->m_rotToOrgMap_Img = dirContext_H->m_rotToOrgMap_Img; dirContext_V->m_rotToOrgMap_Img->SwitchXY();
				dirContext_V->m_orgToRotMap_Img = dirContext_H->m_orgToRotMap_Img->CloneAccessorOnly(); dirContext_V->m_orgToRotMap_Img->SwitchXY();
				dirContext_V->m_angle = rotMgr->GetAngleByRad() + M_PI / 2;
				dirContext_V->m_conflict_Img = dirContext_H->m_conflict_Img->CloneAccessorOnly(); dirContext_V->m_conflict_Img->SwitchXY();

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

			DisplayStandiv_Dir_Img();

			DisplayConflictImg();
		}


		void AngleDirMgrColl::DisplayStandiv_Dir_Img()
		{
			MemAccessor_2D_REF(PixelStandevInfo) psiAcc = m_context_H->m_standevInfoImg->GetMemAccessor();
			F32ImageRef dspImg = F32Image::Create(cvSize(psiAcc->GetIndexSize_X(), psiAcc->GetIndexSize_Y()), 3);

			const int nSize_1D = psiAcc->GetIndexSize_X() * psiAcc->GetIndexSize_Y();

			F32ColorVal * destPtr = (F32ColorVal *)dspImg->GetDataPtr();
			PixelStandevInfo * srcPtr = psiAcc->GetDataPtr();

			float angle_Old = -1;
			for (int i = 0; i < nSize_1D; i++)
			{
				PixelStandevInfo & rSrc = srcPtr[i];
				F32ColorVal & rDest = destPtr[i];

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

				rDest.val0 = 0;
				rDest.val1 = (fabs(cos(angle)) * rSrc.NormVal * 2 / 3);
				rDest.val2 = (fabs(sin(angle)) * rSrc.NormVal * 2 / 3);

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

			ShowImage(dspImg, "dspImg");
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






	}
}

