#pragma once


#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Cpp\Math\mathLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>

#include <Lib\Hcv\RegionSegmentor20.h>



class GlobalStuff
{
public:
	GlobalStuff(void);
	~GlobalStuff(void);


	static Hcv::F32ImageRef OrgImg;


	static void ResetLinePathImg()
	{
		m_linePathImgIndex = -1;
		m_linePathImgVect.ResetSize();

		SetLinePathImg( OrgImg );		
	}

	static void ShowLinePathImg( Hcv::F32ImageRef a_dspImg = NULL )
	//static void ShowLinePathImg( Hcv::F32ImageRef a_dspImg )
	{
		if( NULL == a_dspImg )
			a_dspImg = GlobalStuff::GetLinePathImg();

		ShowImage( a_dspImg, "LinePathImg" );
	}



	static Hcv::F32ImageRef GetLinePathImg( )
	{
		return m_linePathImgVect[ m_linePathImgIndex ];
	}

	static void SetLinePathImg( Hcv::F32ImageRef a_src )
	{
		m_linePathImgIndex++;
		m_linePathImgVect.SetSize( m_linePathImgIndex + 1 );

		m_linePathImgVect[ m_linePathImgIndex ] = a_src;
	}




	static bool UndoLinePathImg( )
	{
		if( m_linePathImgIndex <= 0 )
			return false;

		m_linePathImgIndex--;

		return true;
	}

	static bool RedoLinePathImg( )
	{
		if( m_linePathImgIndex >= m_linePathImgVect.GetSize() - 1 )
			return false;

		m_linePathImgIndex++;

		return true;
	}


	static Hcv::IRegionSegmentor20Ref GetRegionSegmentor( )
	{
		return m_RgnSgm;
	}

	static void SetRegionSegmentor( Hcv::IRegionSegmentor20Ref a_src )
	{
		m_RgnSgm = a_src;
	}

	static Hcv::F32Point GetPoint0( )
	{
		return m_point0;
	}

	static Hcv::F32Point GetPoint1( )
	{
		return m_point1;
	}

	static void SetPoint0( Hcv::F32Point & a_pnt )
	{
		m_point0 = a_pnt;
	}

	static void SetPoint1( Hcv::F32Point & a_pnt )
	{
		m_point1 = a_pnt;
	}


	

public:

	//static int AprSize1D;
	static float AprSize1D;

protected:

	//static Hcv::F32ImageRef LinePathImg;
	
	//static std::vector < Hcv::F32ImageRef > m_linePathImgVect;
	static Hcpl::FixedVector < Hcv::F32ImageRef > m_linePathImgVect;

	static int m_linePathImgIndex;

	static Hcv::F32Point m_point0;
	static Hcv::F32Point m_point1;

	static Hcv::IRegionSegmentor20Ref m_RgnSgm;
	
};
