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
			Context & cx = *m_context;
			Context & ncx = *m_normalContext;
			AngleDirMgrColl_Context & pcx = *m_parentContext;


			//ShowImage(cx.m_org_Img->GetSrcImg(), cx.MakeStrWithId("org_Img->GetSrcImg()").c_str());

			cx.m_avgStandev_H_Img = new F32ImageAccessor1C(cx.m_org_Img->GetOffsetCalc());
			Cala_AvgStandevImage_H(cx.m_org_Img->GetMemAccessor(), cx.m_magSqr_Img->GetMemAccessor(),
				//avgStandev_H_Img->GetMemAccessor(), Range<int>::New(-2, 2), Range<int>::New(-2, 2));
				cx.m_avgStandev_H_Img->GetMemAccessor(), Range<int>::New(-2, 2), Range<int>::New(-1, 1));

			ShowImage(cx.m_avgStandev_H_Img->GetSrcImg(), cx.MakeStrWithId("m_avgStandev_H_Img->GetSrcImg()").c_str());

			//AffectCommonAvgStandev();

		}

		void ImgAngleDirMgr::Proceed_2()
		{
			Context & cx = *m_context;
			Context & ncx = *m_normalContext;
			AngleDirMgrColl_Context & pcx = *m_parentContext;

			AffectCommonAvgStandev();

		}

		void ImgAngleDirMgr::AffectCommonAvgStandev_0()
		{
			Context & cx = *m_context;
			Context & ncx = *m_normalContext;

			int * rotToOrgMap_Buf = cx.m_rotToOrgMap_Img->GetMemAccessor()->GetDataPtr();

			MemAccessor_2D_REF(float) localAcc = cx.m_avgStandev_H_Img->GetMemAccessor();
			PixelStandevInfo * commonImgBuf = m_parentContext->m_standevInfoImg->GetMemAccessor()->GetDataPtr();

			OffsetCalc_1D_Ref localOffsetCalc_Y = localAcc->GenAccessor_1D_Y()->GetOffsetCalc();
			OffsetCalc_1D_Ref localOffsetCalc_X = localAcc->GenAccessor_1D_X()->GetOffsetCalc();
			float * localPtr = localAcc->GetDataPtr();

			float * localPtr_Norm = ncx.m_avgStandev_H_Img->GetMemAccessor()->GetDataPtr();

			int nOffset_Old = -100;
			for (int nOffset_Y = localOffsetCalc_Y->GetOffsetPart1(); nOffset_Y != localOffsetCalc_Y->GetActualLimOffset();
				nOffset_Y += localOffsetCalc_Y->GetActualStepSize())
			{
				const int nLimOffset_YX = nOffset_Y + localOffsetCalc_X->GetActualLimOffset();

				for (int nOffset_YX = nOffset_Y + localOffsetCalc_X->GetOffsetPart1(); nOffset_YX != nLimOffset_YX;
					nOffset_Old = nOffset_YX, nOffset_YX += localOffsetCalc_X->GetActualStepSize())
				{
					int nOffset_Mapped = rotToOrgMap_Buf[nOffset_YX];
					if (nOffset_Mapped < 0)
						continue;
					
					float standev_Local = localPtr[nOffset_YX];

					PixelStandevInfo & rCommonPsi = commonImgBuf[nOffset_Mapped];

					if (rCommonPsi.Val > standev_Local)
					{
						rCommonPsi.Val = standev_Local;
						rCommonPsi.NormVal = localPtr_Norm[nOffset_YX];
						rCommonPsi.Dir = cx.m_nIndex;
					}

				}
			}

		}

		void ImgAngleDirMgr::AffectCommonAvgStandev()
		{
			Context & cx = *m_context;
			Context & ncx = *m_normalContext;

			int * orgToRotMap_Buf = cx.m_orgToRotMap_Img->GetMemAccessor()->GetDataPtr();

			MemAccessor_2D_REF(float) localAcc = cx.m_avgStandev_H_Img->GetMemAccessor();
			PixelStandevInfo * commonImgBuf = m_parentContext->m_standevInfoImg->GetMemAccessor()->GetDataPtr();

			OffsetCalc_1D_Ref localOffsetCalc_Y = localAcc->GenAccessor_1D_Y()->GetOffsetCalc();
			OffsetCalc_1D_Ref localOffsetCalc_X = localAcc->GenAccessor_1D_X()->GetOffsetCalc();
			float * localPtr = localAcc->GetDataPtr();

			float * localPtr_Norm = ncx.m_avgStandev_H_Img->GetMemAccessor()->GetDataPtr();

			int nOffset_Old = -100;
			for (int nOffset_Y = localOffsetCalc_Y->GetOffsetPart1(); nOffset_Y != localOffsetCalc_Y->GetActualLimOffset();
				nOffset_Y += localOffsetCalc_Y->GetActualStepSize())
			{
				const int nLimOffset_YX = nOffset_Y + localOffsetCalc_X->GetActualLimOffset();

				for (int nOffset_YX = nOffset_Y + localOffsetCalc_X->GetOffsetPart1(); nOffset_YX != nLimOffset_YX;
					nOffset_Old = nOffset_YX, nOffset_YX += localOffsetCalc_X->GetActualStepSize())
				{
					int nOffset_Mapped = orgToRotMap_Buf[nOffset_YX];
					if (nOffset_Mapped < 0)
						continue;

					float standev_Local = localPtr[nOffset_YX];

					PixelStandevInfo & rCommonPsi = commonImgBuf[nOffset_Mapped];

					if (rCommonPsi.Val > standev_Local)
					{
						rCommonPsi.Val = standev_Local;
						rCommonPsi.NormVal = localPtr_Norm[nOffset_YX];
						rCommonPsi.Dir = cx.m_nIndex;
					}

				}
			}

		}


	}
}

