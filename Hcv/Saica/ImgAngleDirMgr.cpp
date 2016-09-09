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

	namespace Ns_Saica
	{

		ImgAngleDirMgr::ImgAngleDirMgr(ImgAngleDirMgr_Context_Ref a_context, ImgAngleDirMgr_Context_Ref a_normalContext)
		{
			m_context = a_context;
			m_normalContext = a_normalContext;




			//Prepare();
		}

		ImgAngleDirMgr::Context::Context(ImgRotationMgrRef a_imgRotMgr, F32ImageAccessor3C_Ref a_org_Img, 
			F32ImageAccessor1C_Ref a_magSqr_Img, char a_hv)
		{
			m_imgRotMgr = a_imgRotMgr;

			m_org_Img = a_org_Img;
			m_magSqr_Img = a_magSqr_Img;

			Hcpl_ASSERT(a_hv == 'H' || a_hv == 'V');
			m_hv = a_hv;
		}

		void ImgAngleDirMgr::Prepare()
		{
		
		}


	}
}

