#pragma once

#include <Lib\Hcv\CvIncludes.h>

#define HCV_CALL( Func )                                             \
{                                                                   \
    Func;                                                           \
    HCV_CHECK();                                                     \
}

#define HCV_CHECK()                                                  \
{                                                                   \
    if( cvGetErrStatus() < 0 )                                      \
        HCV_ERROR();      \
}


#define HCV_ERROR()                                             \
{                                                                   \
    Hcpl_ASSERT(false);                                          \
    cvError( -1, "Hcv", "", __FILE__, __LINE__ );        \
}
	//goto Error;											\
