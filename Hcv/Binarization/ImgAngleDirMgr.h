#pragma once

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Cpp\Math\mathLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\ImageAccessor.h>

//#include <Lib\Hcv\ImageLineItrProvider.h>
#include <Lib\Hcv\ImageItrMgr.h>

#include <Lib\Hcv\ImgSizeRotation.h>
#include <Lib\Hcv\Binarization\ImgAngleDirMgr.h>
#include <Lib\Hcv\Binarization\AngleDirMgrColl_Context.h>


namespace Hcv
{
	namespace Ns_Binarization
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

				Context(int a_nIndex, ImgSizeRotationRef a_imgRot, F32ImageAccessor3C_Ref a_org_Img, F32ImageAccessor1C_Ref a_magSqr_Img, char a_hv);

				std::string & GetIdStr() {
					return m_idStr;
				}

				std::string MakeStrWithId(char * a_str) {
						return (GetIdStr() + std::string(a_str));
				}

				//protected:

				int m_nIndex;
				std::string m_idStr;

				F32ImageAccessor3C_Ref m_org_Img;
				F32ImageAccessor1C_Ref m_magSqr_Img;

				char m_hv;
				ImgSizeRotationRef m_imgRot;

				F32ImageAccessor1C_Ref m_avgStandev_H_Img;

				S32ImageAccessor1C_Ref m_rotToOrgMap_Img;
				S32ImageAccessor1C_Ref m_orgToRotMap_Img;
				float m_angle;
				TempImageAccessor_REF(ConflictInfo) m_conflict_Img;

				//F32ImageAccessor1C_Ref m_standev_InrWide_Img;

				F32ImageAccessor1C_Ref m_wideConflictDiff_Img;
				F32VectorValImageAcc_4C_Ref m_avgPStandev_InrWide_Img;
			};

		public:

			ImgAngleDirMgr(Hcpl::ObjRef< ImgAngleDirMgr::Context > a_context, Hcpl::ObjRef< ImgAngleDirMgr::Context > a_normalContext,
				AngleDirMgrColl_Context_Ref a_parentContext);

			Hcpl::ObjRef< ImgAngleDirMgr::Context > GetContext()
			{
				return m_context;
			}

			Hcpl::ObjRef< ImgAngleDirMgr::Context > GetNormalContext()
			{
				return m_normalContext;
			}

			void Proceed_1();
			void Proceed_2();
			void Proceed_3();
			void Proceed_4();
			void Proceed_5_1();
			void Proceed_5_2();
			void Proceed_5_3();
			void Proceed_6();

		protected:

			//void AffectCommonAvgStandev_0();
			void AffectCommonAvgStandev();
			void AffectCommonConflict();
			void AffectCommonWideConflictDiff();
			void AffectCommonAvgPStandev_InrWide();


			void DisplayConflictImg();

		protected:

			//ImgAngleDirMgr_Context_Ref m_context;
			//ImgAngleDirMgr_Context_Ref m_normalContext;

			Hcpl::ObjRef< ImgAngleDirMgr::Context > m_context;
			Hcpl::ObjRef< ImgAngleDirMgr::Context > m_normalContext;
			AngleDirMgrColl_Context_Ref m_parentContext;
		};

		typedef Hcpl::ObjRef< ImgAngleDirMgr > ImgAngleDirMgrRef;
		typedef Hcpl::ObjRef< ImgAngleDirMgr::Context > ImgAngleDirMgr_Context_Ref;

	};



}