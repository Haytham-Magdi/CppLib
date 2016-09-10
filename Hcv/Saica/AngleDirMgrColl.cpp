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
			m_context = new AngleDirMgrColl_Context();

			Prepare();
		}

		void AngleDirMgrColl::Prepare()
		{
			ImgRotationMgrRef rotMgr = m_rotMgrColl->GetRotAt(i);
			F32ImageAccessor3C_Ref org_Img_H = new F32ImageAccessor3C(rotMgr->GetResImg());


			AngleDirMgrColl_Context & cx = *m_context;


			
			cx.m_standevInfoImg = new TempImageAccessor<PixelStandevInfo>(();





			//m_angleDirMgrArr.SetCapacity(m_rotMgrColl->GetNofRots() * 2);
			m_angleDirMgrArr.SetSize(m_rotMgrColl->GetNofRots() * 2);

			//int nDirIndex = 0;
			for (int i = 0; i < m_rotMgrColl->GetNofRots(); i++)
			{
				ImgRotationMgrRef rotMgr = m_rotMgrColl->GetRotAt(i);

				F32ImageAccessor3C_Ref org_Img_H = new F32ImageAccessor3C(rotMgr->GetResImg());

				F32ImageAccessor1C_Ref magSqr_Img_H = new F32ImageAccessor1C(org_Img_H->GetOffsetCalc());
				CalcMagSqrImage(org_Img_H->GetMemAccessor(), magSqr_Img_H->GetMemAccessor());

				//ImgAngleDirMgr_Context_Ref dirContext_H = new ImgAngleDirMgr::Context(nDirIndex++, rotMgr,
					ImgAngleDirMgr_Context_Ref dirContext_H = new ImgAngleDirMgr::Context(i, rotMgr,
					org_Img_H, magSqr_Img_H, 'H');

				F32ImageAccessor3C_Ref org_Img_V = org_Img_H->CloneAccessorOnly(); org_Img_V->SwitchXY();
				F32ImageAccessor1C_Ref magSqr_Img_V = magSqr_Img_H->CloneAccessorOnly(); magSqr_Img_V->SwitchXY();

				//ImgAngleDirMgr_Context_Ref dirContext_V = new ImgAngleDirMgr::Context(nDirIndex++, rotMgr,
				ImgAngleDirMgr_Context_Ref dirContext_V = new ImgAngleDirMgr::Context(i + m_rotMgrColl->GetNofRots(),
						rotMgr, org_Img_V, magSqr_Img_V, 'V');

				ImgAngleDirMgrRef angleDirMgr_H = new ImgAngleDirMgr(dirContext_H, dirContext_V, m_context);
				//m_angleDirMgrArr.PushBack(angleDirMgr_H);
				m_angleDirMgrArr[i] = angleDirMgr_H;

				ImgAngleDirMgrRef angleDirMgr_V = new ImgAngleDirMgr(dirContext_V, dirContext_H, m_context);
				//m_angleDirMgrArr.PushBack(angleDirMgr_V);
				m_angleDirMgrArr[i + m_rotMgrColl->GetNofRots()] = angleDirMgr_V;
			}

			for (int i = 0; i < m_angleDirMgrArr.GetSize(); i++)
			{
				ImgAngleDirMgrRef angleDirMgr = m_angleDirMgrArr[i];
				angleDirMgr->Proceed_1();
			}

		}



	}
}

