#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\Saica\AngleDirMgrColl.h>


#define M_PI 3.14159265358979323846


namespace Hcv
{
	using namespace Hcpl;
	//using namespace Hcpl::Math;

	using namespace Hcv;
	using namespace Hcv::Image_Operations;
	using namespace Hcv::ImageAccessorOperations;

	namespace Ns_Saica
	{
		AngleDirMgrColl::AngleDirMgrColl(RotationMgrCollRef a_rotMgrColl)
		{
			m_rotMgrColl = a_rotMgrColl;

			Prepare();

		}

		void AngleDirMgrColl::Prepare()
		{
			m_angleDirMgrArr.SetCapacity(m_rotMgrColl->GetNofRots() * 2);

			for (int i = 0; i < m_rotMgrColl->GetNofRots(); i++)
			{
				ImgRotationMgrRef rotMgr = m_rotMgrColl->GetRotAt(i);

				F32ImageAccessor3C_Ref org_Img = new F32ImageAccessor3C(rotMgr->GetResImg());

				F32ImageAccessor1C_Ref magSqr_Img = new F32ImageAccessor1C(org_Img->GetOffsetCalc());
				CalcMagSqrImage(org_Img->GetMemAccessor(), magSqr_Img->GetMemAccessor());

				//ImgAngleDirMgr_Context_Ref r1;
				ImgAngleDirMgr::Context * r1;


				////ImgAngleDirMgrRef angleDirMgrH = new ImgAngleDirMgr(rotMgr, rotMagSqrImg, 'H', m_nAprLen_1, m_nAprLen_2);
				//ImgAngleDirMgrRef angleDirMgrH = new ImgAngleDirMgr(rotMgr, rotMagSqrImg, 'H');
				//m_angleDirMgrArr.PushBack(angleDirMgrH);

				////ImgAngleDirMgrRef angleDirMgrV = new ImgAngleDirMgr(rotMgr, rotMagSqrImg, 'V', m_nAprLen_2, m_nAprLen_1);
				//ImgAngleDirMgrRef angleDirMgrV = new ImgAngleDirMgr(rotMgr, rotMagSqrImg, 'V');
				//m_angleDirMgrArr.PushBack(angleDirMgrV);
			}

		}



	}
}

