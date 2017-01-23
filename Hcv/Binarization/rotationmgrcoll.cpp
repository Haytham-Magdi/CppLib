#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\Binarization\RotationMgrColl.h>


#define M_PI 3.14159265358979323846


namespace Hcv
{
	using namespace Hcpl;
	//using namespace Hcpl::Math;

	namespace Ns_Binarization
	{

		//RotationMgrColl::RotationMgrColl(F32ImageRef a_srcImg, int a_nofRots)
		//{
		//	m_srcImg = a_srcImg;
		//	m_nofRots = a_nofRots;

		//	Prepare();
		//}

		//void RotationMgrColl::Prepare()
		//{
		//
		//}

		RotationMgrColl::RotationMgrColl(
			F32ImageRef a_srcImg, int a_nofRots)
		{
			Hcpl_ASSERT(a_nofRots >= 0);

			//Hcpl_ASSERT( 0 == a_nofRots % 2 );

			m_srcImg = a_srcImg;

			m_nofRots = a_nofRots;

			m_rotMgrArr.SetCapacity(a_nofRots);

			float angStep = 90.0F / a_nofRots;

			for (int i = 0; i < a_nofRots; i++)
			{
				float angDig = i * angStep;

				ImgRotationMgrRef rot = new ImgRotationMgr(m_srcImg, angDig);

				m_rotMgrArr.PushBack(rot);
			}

		}



	}
}

