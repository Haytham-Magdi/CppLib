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

		ImgAngleDirMgr::ImgAngleDirMgr(ImgRotationMgrRef a_imgRotMgr, F32ImageRef a_rotSqrImg, char a_hv, int a_nAprLen_1, int a_nAprLen_2)
		{
			Debug::Assert(a_hv == 'H' || a_hv == 'V');

				//if (a_hv != 'H' && a_hv != 'V')
			//	throw "Invalid a_hv";


			m_imgRotMgr = a_imgRotMgr;
			
			m_nAprLen_1 = a_nAprLen_1;
			m_nAprLen_2 = a_nAprLen_2;

			m_srcImg = m_imgRotMgr->GetSrcImg();
			m_rotImg = m_imgRotMgr->GetResImg();

			Debug::Assert(m_rotImg->GetSize().width == a_rotSqrImg->GetSize().width
				&& m_rotImg->GetSize().height == a_rotSqrImg->GetSize().height);
			m_rotSqrImg = a_rotSqrImg;



			//Prepare();
		}

		//void ImgAngleDirMgr::Prepare()
		//{
		//
		//}



	}
}

