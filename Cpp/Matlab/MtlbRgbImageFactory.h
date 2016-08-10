#pragma once

#pragma inline_depth( 255 )


#include <Lib\Cpp\Common\common.h>
#include <Lib\Cpp\Common\Object.h>
#include <Lib\Cpp\Image\IRgbImage.h>
#include <Lib\Cpp\Image\IRgbImageFactory.h>
#include <Lib\Cpp\Image\IImage.h>
#include <Lib\Cpp\Matlab\MtlbRgbImage.h>
#include <Lib\Cpp\Matlab\MtlbImage.h>

#include <Lib\Cpp\Matlab\Mtlb.h>



namespace Hcpl
{
	template<class T>	
		class MtlbRgbImageFactory : FRM_IRgbImageFactory_IFC(T), FRM_Object
	{
		protected:
			MtlbRgbImageFactory() {}
			static MtlbRgbImageFactory<T> * m_inst;


		public:

			static MtlbRgbImageFactory<T> * GetInst(void)
			{
				if(NULL == m_inst)
					m_inst = new MtlbRgbImageFactory();
				return m_inst;
			}

			virtual IRgbImage<T> * RgbImageFromFile(char * a_sPath)
			{
				char sVarName[30];

				Mtlb::NewVarName(sVarName, sizeof(sVarName)-1);

				char sCmd[150];

				sCmd[0] = 0;

				strcat(sCmd, sVarName);
				strcat(sCmd, " = imread('");
				strcat(sCmd, a_sPath);
				strcat(sCmd, "');");


				Engine * pEng = Mtlb::GetEngine();
				engEvalString(pEng, sCmd);

				mxArray * pMxArr = engGetVariable(pEng, sVarName);

				MtlbRgbImage<T> * pRgbImg = new MtlbRgbImage<T>(pMxArr, sVarName);
				//MtlbRgbImage<T> * pRgbImg = NULL;

				sCmd[0] = 0;
				strcat(sCmd, " imview(");
				strcat(sCmd, sVarName);
				strcat(sCmd, ");");

				engEvalString(pEng, sCmd);


				return NULL;
			}
				
			virtual Int32 GetType()
			{
				return 0;
			}


	};

	template<class T>
		MtlbRgbImageFactory<T> * MtlbRgbImageFactory<T>::m_inst = NULL;

	
}