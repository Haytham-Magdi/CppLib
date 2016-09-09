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

#include <Lib\Hcv\ImgRotationMgr.h>
#include <Lib\Hcv\Saica\ImgAngleDirMgr.h>


namespace Hcv
{
	namespace Ns_Saica
	{

		//class ImgAngleDirMgr;
		//class ImgAngleDirMgr::Context;
		//typedef Hcpl::ObjRef< ImgAngleDirMgr::Context > ImgAngleDirMgr_Context_Ref;

		class ImgAngleDirMgr : FRM_Object
		{
		public:

			class Context : FRM_Object
			{
			public:

				Context(ImgRotationMgrRef a_imgRotMgr, F32ImageAccessor3C_Ref a_org_Img, F32ImageAccessor1C_Ref a_magSqr_Img, char a_hv);

			protected:

				F32ImageAccessor3C_Ref m_org_Img;
				F32ImageAccessor1C_Ref m_magSqr_Img;

				char m_hv;
				ImgRotationMgrRef m_imgRotMgr;
			};

			typedef Hcpl::ObjRef< ImgAngleDirMgr::Context > ImgAngleDirMgr_Context_Ref;

		public:

			ImgAngleDirMgr(ImgAngleDirMgr_Context_Ref a_context, ImgAngleDirMgr_Context_Ref a_normalContext);

			ImgAngleDirMgr_Context_Ref GetContext()
			{
				return m_context;
			}

		protected:

			void Prepare();

		protected:

			ImgAngleDirMgr_Context_Ref m_context;
			ImgAngleDirMgr_Context_Ref m_normalContext;

		};

		typedef Hcpl::ObjRef< ImgAngleDirMgr > ImgAngleDirMgrRef;
	};



}