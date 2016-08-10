//#include "StdAfx.h"
#include <StdAfx.h>

#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Cpp\Math\mathLib.h>
#include <Lib\Hcv\HcvLib.h>

#include "cvapp.h"


#include "GlobalStuff.h"



Hcv::F32ImageRef GlobalStuff::OrgImg = NULL;

//std::vector < Hcv::F32ImageRef >  GlobalStuff::m_linePathImgVect;
Hcpl::FixedVector  < Hcv::F32ImageRef >  GlobalStuff::m_linePathImgVect( 300 );


int GlobalStuff::m_linePathImgIndex = -1;

//int GlobalStuff::AprSize1D = 15;
float GlobalStuff::AprSize1D = 15;
//int GlobalStuff::AprSize1D = 85;

int Hcpl::FixedVectorDebug::AllocCnt = 0;

int Hcpl::FixedVectorDebug::FixedVectorID = 0;

Hcv::IRegionSegmentor20Ref GlobalStuff::m_RgnSgm;

Hcv::F32Point GlobalStuff::m_point0;
Hcv::F32Point GlobalStuff::m_point1;


GlobalStuff::GlobalStuff(void)
{
}

GlobalStuff::~GlobalStuff(void)
{
}
