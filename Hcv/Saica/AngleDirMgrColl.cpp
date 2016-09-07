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

	namespace Ns_Saica
	{

		//AngleDirMgrColl::AngleDirMgrColl(F32ImageRef a_srcImg, int a_nofRots)
		//{
		//	m_srcImg = a_srcImg;
		//	m_nofRots = a_nofRots;

		//	Prepare();
		//}

		AngleDirMgrColl::AngleDirMgrColl(RotationMgrCollRef a_rotMgrColl, int a_nAprLen_1, int a_nAprLen_2)
		{
			m_rotMgrColl = a_rotMgrColl;			
			
			m_nAprLen_1 = a_nAprLen_1;
			m_nAprLen_2 = a_nAprLen_2;

		}

		void AngleDirMgrColl::Prepare()
		{
			m_angleDirMgrArr.SetCapacity(m_rotMgrColl->GetNofRots() * 2);

			for (int i = 0; i < m_rotMgrColl->GetNofRots(); i++)
			{
				ImgRotationMgr_2_Ref rotMgr = m_rotMgrColl->GetRotAt(i);
				F32ImageRef rotImg = rotMgr->GetResImg();
				F32ImageRef rotMagSqrImg = GenMagSqrImg(rotImg);

				ImgAngleDirMgrRef angleDirMgrH = new ImgAngleDirMgr(rotMgr, rotMagSqrImg, 'H', m_nAprLen_1, m_nAprLen_2);
				m_angleDirMgrArr.PushBack(angleDirMgrH);

				ImgAngleDirMgrRef angleDirMgrV = new ImgAngleDirMgr(rotMgr, rotMagSqrImg, 'V', m_nAprLen_2, m_nAprLen_1);
				m_angleDirMgrArr.PushBack(angleDirMgrV);
			}

		}



	}
}

