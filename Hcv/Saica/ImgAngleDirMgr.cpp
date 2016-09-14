#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\Saica\ImgAngleDirMgr.h>


#define M_PI 3.14159265358979323846


namespace Hcv
{
	using namespace Hcpl;
	//using namespace Hcpl::Math;

	using namespace Hcv::Image_Operations;
	using namespace Hcv::ImageAccessorOperations;

	namespace Ns_Saica
	{

		ImgAngleDirMgr::ImgAngleDirMgr(ImgAngleDirMgr_Context_Ref a_context, ImgAngleDirMgr_Context_Ref a_normalContext,
			AngleDirMgrColl_Context_Ref a_parentContext)
		{
			m_context = a_context;
			m_normalContext = a_normalContext;
			m_parentContext = a_parentContext;
		}

		ImgAngleDirMgr::Context::Context(int a_nIndex, ImgRotationMgrRef a_imgRotMgr, F32ImageAccessor3C_Ref a_org_Img,
			F32ImageAccessor1C_Ref a_magSqr_Img, char a_hv)
		{
			m_nIndex = a_nIndex;
			m_imgRotMgr = a_imgRotMgr;

			m_org_Img = a_org_Img;
			m_magSqr_Img = a_magSqr_Img;

			Hcpl_ASSERT(a_hv == 'H' || a_hv == 'V');
			m_hv = a_hv;

			{
				char idStr_0[70];
				sprintf(idStr_0, "%.2f, %c: ", m_imgRotMgr->GetAngleByDigree(), m_hv);
				m_idStr = idStr_0;
			}

		}

		void ImgAngleDirMgr::Proceed_1()
		{
			return;

			Context & cx = *m_context;
			Context & ncx = *m_normalContext;
			AngleDirMgrColl_Context & pcx = *m_parentContext;


			//ShowImage(cx.m_org_Img->GetSrcImg(), cx.MakeStrWithId("org_Img->GetSrcImg()").c_str());

			cx.m_avgStandev_H_Img = new F32ImageAccessor1C(cx.m_org_Img->GetOffsetCalc());
			Cala_AvgStandevImage_H(cx.m_org_Img->GetMemAccessor(), cx.m_magSqr_Img->GetMemAccessor(),
				cx.m_avgStandev_H_Img->GetMemAccessor(), Range<int>::New(-2, 2), Range<int>::New(-2, 2));

			//cx.m_avgStandev_H_Img->GetMemAccessor(), Range<int>::New(-3, 3), Range<int>::New(-1, 1));
			//cx.m_avgStandev_H_Img->GetMemAccessor(), Range<int>::New(-5, 5), Range<int>::New(-5, 5));

			//ShowImage(cx.m_avgStandev_H_Img->GetSrcImg(), cx.MakeStrWithId("m_avgStandev_H_Img->GetSrcImg()").c_str());

			//AffectCommonAvgStandev();

		}

		void ImgAngleDirMgr::Proceed_2()
		{
			return;

			//Context & cx = *m_context;
			//Context & ncx = *m_normalContext;
			//AngleDirMgrColl_Context & pcx = *m_parentContext;

			AffectCommonAvgStandev();

		}

		void ImgAngleDirMgr::Proceed_3()
		{
			Context & cx = *m_context;
			Context & ncx = *m_normalContext;
			AngleDirMgrColl_Context & pcx = *m_parentContext;

			Window<int> avgWin = Window<int>::New(-1, 1, -5, 5);

			cx.m_conflict_Img = new TempImageAccessor<ConflictInfo>(cx.m_org_Img->GetOffsetCalc());

			F32ImageAccessor3C_Ref avg_Img = cx.m_org_Img->CloneAccAndImage();
			AvgImage(cx.m_org_Img->GetMemAccessor(), avg_Img->GetMemAccessor(), avgWin);

			F32ImageAccessor1C_Ref avg_MagSqr_Img = new F32ImageAccessor1C(cx.m_org_Img->GetOffsetCalc());
			AvgImage(cx.m_magSqr_Img->GetMemAccessor(), avg_MagSqr_Img->GetMemAccessor(), avgWin);

			Range<int> confRange = Range<int>::New(
				-1 - avgWin.Get_X2(), 1 - avgWin.Get_X1());

			CalcConflictImage_H(avg_Img->GetMemAccessor(), avg_MagSqr_Img->GetMemAccessor(),
				cx.m_conflict_Img->GetMemAccessor(), confRange);

			//if (0 == cx.m_nIndex)
			{
				//DisplayConflictImg();
			}

		}

		void ImgAngleDirMgr::Proceed_4()
		{
			//Context & cx = *m_context;
			//Context & ncx = *m_normalContext;
			//AngleDirMgrColl_Context & pcx = *m_parentContext;

			AffectCommonConflict();
		}

		void ImgAngleDirMgr::AffectCommonAvgStandev()
		{
			Context & cx = *m_context;
			Context & ncx = *m_normalContext;

			int * orgToRotMap_Buf = cx.m_orgToRotMap_Img->GetMemAccessor()->GetDataPtr();

			//m_parentContext->m_standevInfoImg->GetMemAccessor();

			MemAccessor_2D_REF(int) orgToRotMap_Acc = cx.m_orgToRotMap_Img->GetMemAccessor();

			OffsetCalc_1D_Ref commonOffsetCalc_Y = orgToRotMap_Acc->GenAccessor_1D_Y()->GetOffsetCalc();
			OffsetCalc_1D_Ref commonOffsetCalc_X = orgToRotMap_Acc->GenAccessor_1D_X()->GetOffsetCalc();

			PixelStandevInfo * commonImgBuf = m_parentContext->m_standevInfoImg->GetMemAccessor()->GetDataPtr();

			float * localPtr = cx.m_avgStandev_H_Img->GetMemAccessor()->GetDataPtr();
			float * localPtr_Norm = ncx.m_avgStandev_H_Img->GetMemAccessor()->GetDataPtr();

			for (int nOffset_Y = commonOffsetCalc_Y->GetOffsetPart1(); nOffset_Y != commonOffsetCalc_Y->GetActualLimOffset();
				nOffset_Y += commonOffsetCalc_Y->GetActualStepSize())
			{
				const int nLimOffset_YX = nOffset_Y + commonOffsetCalc_X->GetActualLimOffset();

				for (int nOffset_YX = nOffset_Y + commonOffsetCalc_X->GetOffsetPart1(); nOffset_YX != nLimOffset_YX;
					nOffset_YX += commonOffsetCalc_X->GetActualStepSize())
				{
					PixelStandevInfo & rCommonPsi = commonImgBuf[nOffset_YX];

					int nOffset_Mapped = orgToRotMap_Buf[nOffset_YX];
					Hcpl_ASSERT(nOffset_Mapped >= 0);

					float standev_Local = localPtr[nOffset_Mapped];

					if (standev_Local < rCommonPsi.Val)
					{
						rCommonPsi.Val = standev_Local;
						rCommonPsi.NormVal = localPtr_Norm[nOffset_Mapped];
						rCommonPsi.Dir = cx.m_nIndex;
					}

				}
			}

		}

		void ImgAngleDirMgr::AffectCommonConflict()
		{
			Context & cx = *m_context;
			Context & ncx = *m_normalContext;
			AngleDirMgrColl_Context & pcx = *m_parentContext;

			int * orgToRotMap_Buf = cx.m_orgToRotMap_Img->GetMemAccessor()->GetDataPtr();
			int * rotToOrgMap_Buf = cx.m_rotToOrgMap_Img->GetMemAccessor()->GetDataPtr();

			//pcx.m_conflictInfoImg->GetMemAccessor();

			MemAccessor_2D_REF(int) orgToRotMap_Acc = cx.m_orgToRotMap_Img->GetMemAccessor();

			OffsetCalc_1D_Ref commonOffsetCalc_Y = orgToRotMap_Acc->GenAccessor_1D_Y()->GetOffsetCalc();
			OffsetCalc_1D_Ref commonOffsetCalc_X = orgToRotMap_Acc->GenAccessor_1D_X()->GetOffsetCalc();

			ConflictInfo_Ex * commonImgBuf = pcx.m_conflictInfoImg->GetMemAccessor()->GetDataPtr();

			ConflictInfo * localPtr = cx.m_conflict_Img->GetMemAccessor()->GetDataPtr();
			//ConflictInfo * localPtr_Norm = ncx.m_conflict_Img->GetMemAccessor()->GetDataPtr();

			for (int nOffset_Y = commonOffsetCalc_Y->GetOffsetPart1(); nOffset_Y != commonOffsetCalc_Y->GetActualLimOffset();
				nOffset_Y += commonOffsetCalc_Y->GetActualStepSize())
			{
				const int nLimOffset_YX = nOffset_Y + commonOffsetCalc_X->GetActualLimOffset();

				for (int nOffset_YX = nOffset_Y + commonOffsetCalc_X->GetOffsetPart1(); nOffset_YX != nLimOffset_YX;
					nOffset_YX += commonOffsetCalc_X->GetActualStepSize())
				{
					ConflictInfo_Ex & rCommonConf = commonImgBuf[nOffset_YX];

					int nOffset_Mapped = orgToRotMap_Buf[nOffset_YX];
					Hcpl_ASSERT(nOffset_Mapped >= 0);

					ConflictInfo & conf_Local = localPtr[nOffset_Mapped];

					if (conf_Local.Exists)
					{
						//*((ConflictInfo *)&rCommonConf) = conf_Local;

						rCommonConf.Exists = true;
						rCommonConf.Offset_Side_1 = rotToOrgMap_Buf[conf_Local.Offset_Side_1];
						rCommonConf.Offset_Side_2 = rotToOrgMap_Buf[conf_Local.Offset_Side_2];

						rCommonConf.Dir = cx.m_nIndex;
					}

				}
			}

		}


		void ImgAngleDirMgr::DisplayConflictImg()
		{
			Context & cx = *m_context;

			MemAccessor_2D_REF(ConflictInfo) confAcc = cx.m_conflict_Img->GetMemAccessor();
			F32ImageRef confDsp_Img = F32Image::Create(cvSize(confAcc->GetIndexSize_X_Org(), confAcc->GetIndexSize_Y_Org()), 3);

			confDsp_Img->SetAll(0);

			const int nSize_1D = confAcc->GetIndexSize_X() * confAcc->GetIndexSize_Y();

			F32ColorVal * destPtr = (F32ColorVal *)confDsp_Img->GetDataPtr();
			ConflictInfo * srcPtr = confAcc->GetDataPtr();

			float angle_Old = -1;
			for (int i = 0; i < nSize_1D; i++)
			{
				ConflictInfo & rSrc = srcPtr[i];
				F32ColorVal & rDest = destPtr[i];

				//rDest.val0 = 0;
				//rDest.val1 = 255;
				//rDest.val2 = 255;

				//continue;

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

			ShowImage(confDsp_Img, cx.MakeStrWithId("confDsp_Img").c_str());
		}

	}
}

