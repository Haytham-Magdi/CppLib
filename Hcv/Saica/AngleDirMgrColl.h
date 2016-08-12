#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Cpp\Math\mathLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>

//#include <Lib\Hcv\ImageLineItrProvider.h>
#include <Lib\Hcv\ImageItrMgr.h>

#include <Lib\Hcv\Saica\ImgAngleDirMgr.h>
#include <Lib\Hcv\Saica\RotationMgrColl.h>


namespace Hcv
{
	namespace Ns_Saica
	{

		class AngleDirMgrColl : FRM_Object
		{
		public:

			AngleDirMgrColl(RotationMgrCollRef a_rotMgrColl, int a_nAprLen_1, int a_nAprLen_2);

			//ImgAngleDirMgrRef GetRotAt(int a_nRotIdx)
			//{
			//	return m_rotMgrArr[a_nRotIdx];
			//}

			//int GetNofRots()
			//{
			//	return m_rotMgrArr.GetSize();
			//}

			//F32ImageRef GetSrcImg()
			//{
			//	return m_srcImg;
			//}

		protected:

			void Prepare();

		protected:

			RotationMgrCollRef m_rotMgrColl;
			//F32ImageRef m_srcImg;

			int m_nAprLen_1;
			int m_nAprLen_2;

			//int m_nofRots;
			FixedVector< ImgAngleDirMgrRef > m_rotMgrArr;
		};

		typedef Hcpl::ObjRef< AngleDirMgrColl > AngleDirMgrCollRef;
	};

}