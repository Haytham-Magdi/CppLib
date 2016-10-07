#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\ImgSizeRotationColl.h>


#define M_PI 3.14159265358979323846


namespace Hcv
{
	using namespace Hcpl;
	//using namespace Hcpl::Math;

	namespace Ns_Saica
	{

		//ImgSizeRotationColl::ImgSizeRotationColl(F32ImageRef a_srcImg, int a_nofRots)
		//{
		//	m_srcImg = a_srcImg;
		//	m_nofRots = a_nofRots;

		//	Prepare();
		//}

		//void ImgSizeRotationColl::Prepare()
		//{
		//
		//}

		ImgSizeRotationColl::ImgSizeRotationColl(
			CvSize a_srcSiz, int a_nofRots)
		{
			Hcpl_ASSERT(a_nofRots >= 0);

			//Hcpl_ASSERT( 0 == a_nofRots % 2 );

			m_srcSiz = a_srcSiz;

			m_nofRots = a_nofRots;

			m_rotMgrArr.SetCapacity(a_nofRots);

			float angStep = 90.0F / a_nofRots;

			for (int i = 0; i < a_nofRots; i++)
			{
				float angDig = i * angStep;

				ImgSizeRotationRef rot = new ImgSizeRotation(m_srcSiz, angDig);

				m_rotMgrArr.PushBack(rot);
			}

		}



	}
}

