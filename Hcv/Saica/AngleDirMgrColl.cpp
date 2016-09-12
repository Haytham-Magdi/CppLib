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

			PixelStandevInfo initPsi;
			initPsi.Dir = -1;
			initPsi.Val = 10000000;
			initPsi.NormVal = 0;
			FillImage(m_context_H->m_standevInfoImg->GetMemAccessor(), initPsi);

			m_context_V->m_standevInfoImg = m_context_H->m_standevInfoImg->CloneAccessorOnly();
			m_context_V->m_standevInfoImg->SwitchXY();

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


				F32ImageAccessor3C_Ref rot_Img_V = rot_Img_H->CloneAccessorOnly(); rot_Img_V->SwitchXY();
				F32ImageAccessor1C_Ref magSqr_Img_V = magSqr_Img_H->CloneAccessorOnly(); magSqr_Img_V->SwitchXY();

				ImgAngleDirMgr_Context_Ref dirContext_V = new ImgAngleDirMgr::Context(i + m_rotMgrColl->GetNofRots(),
					rotMgr, rot_Img_V, magSqr_Img_V, 'V');

				//dirContext_V->m_rotToOrgMap_Img = dirContext_H->m_rotToOrgMap_Img; dirContext_V->m_rotToOrgMap_Img->SwitchXY();
				dirContext_V->m_orgToRotMap_Img = dirContext_H->m_orgToRotMap_Img; dirContext_V->m_orgToRotMap_Img->SwitchXY();
				dirContext_V->m_angle = rotMgr->GetAngleByRad() + M_PI / 2;

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

		}



	}
}

