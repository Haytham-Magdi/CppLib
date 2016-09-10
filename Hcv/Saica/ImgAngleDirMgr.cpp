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

		ImgAngleDirMgr::ImgAngleDirMgr(ImgAngleDirMgr_Context_Ref a_context, ImgAngleDirMgr_Context_Ref a_normalContext)
		{
			m_context = a_context;
			m_normalContext = a_normalContext;
		}

		ImgAngleDirMgr::Context::Context(ImgRotationMgrRef a_imgRotMgr, F32ImageAccessor3C_Ref a_org_Img, 
			F32ImageAccessor1C_Ref a_magSqr_Img, char a_hv)
		{
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

			//ShowImage(cx.m_org_Img->GetSrcImg(), cx.MakeStrWithId("org_Img->GetSrcImg()").c_str());

			F32ImageAccessor1C_Ref avgStandev_H_Img = new F32ImageAccessor1C(cx.m_org_Img->GetOffsetCalc());
			Cala_AvgStandevImage_H(cx.m_org_Img->GetMemAccessor(), cx.m_magSqr_Img->GetMemAccessor(),
				//avgStandev_H_Img->GetMemAccessor(), Range<int>::New(-2, 2), Range<int>::New(-2, 2));
				avgStandev_H_Img->GetMemAccessor(), Range<int>::New(-2, 2), Range<int>::New(-1, 1));

			ShowImage(avgStandev_H_Img->GetSrcImg(), cx.MakeStrWithId("avgStandev_H_Img->GetSrcImg()").c_str());

		}


	}
}

