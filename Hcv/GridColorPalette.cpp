#include <Lib\Cpp\Common\commonLib.h>
#include <Lib\Hcv\CvIncludes.h>
#include <Lib\Hcv\Types.h>
#include <Lib\Hcv\error.h>
#include <vector>
#include <Lib\Hcv\Channel.h>
#include <Lib\Hcv\Image.h>
#include <Lib\Hcv\funcs1.h>

#include <Lib\Hcv\GridColorPalette.h>

namespace Hcv
{
	using namespace Hcpl;
	//using namespace Hcpl::Math;

	GridColorPalette::CoreRef GridColorPalette::m_core = NULL;


	GridColorPalette::Elm::Elm()
	{

	}


	GridColorPalette::Core::Core()
	{
		Prepare();
	}
	

	void GridColorPalette::Core::PrepareColorRep( 
		F32ColorVal & a_rColor, ColorRep * a_pOutRep )
	{
		int x_Poses[2];
		int y_Poses[2];
		int z_Poses[2];

		x_Poses[ 0 ] = ((int)a_rColor.val0 / m_nSep) + 1;
		x_Poses[ 1 ] = x_Poses[ 0 ] + 1;

		y_Poses[ 0 ] = ((int)a_rColor.val1 / m_nSep) + 1;
		y_Poses[ 1 ] = y_Poses[ 0 ] + 1;

		z_Poses[ 0 ] = ((int)a_rColor.val2 / m_nSep) + 1;
		z_Poses[ 1 ] = z_Poses[ 0 ] + 1;

		int x_Mods[2];
		int y_Mods[2];
		int z_Mods[2];

		int nMod;

		nMod = (int)a_rColor.val0 % m_nSep;
		x_Mods[ 0 ] = m_nSep - nMod;
		x_Mods[ 1 ] = nMod;
		
		nMod = (int)a_rColor.val1 % m_nSep;
		y_Mods[ 0 ] = m_nSep - nMod;
		y_Mods[ 1 ] = nMod;
		
		nMod = (int)a_rColor.val2 % m_nSep;
		z_Mods[ 0 ] = m_nSep - nMod;
		z_Mods[ 1 ] = nMod;
		
		int nSep3 = m_nSep * m_nSep * m_nSep;

		int i = 0;
		for( int z=0; z < 2; z++ )
		{
			for( int y=0; y < 2; y++ )
			{		
				for( int x=0; x < 2; x++, i++ )
				{
					Share & rShare = a_pOutRep->Shares[ i ];

					//rShare.ElmIdx = m_idxCalc.Calc( x, y, z );
					
					rShare.ElmIdx = m_idxCalc.Calc( 
						x_Poses[ x ], y_Poses[ y ], z_Poses[ z ] );

					Elm & rElm = m_elmArr[ rShare.ElmIdx ];

					rShare.nSize = x_Mods[ x ] * y_Mods[ y ] * z_Mods[ z ] * 100;
					rShare.nSize /= nSep3;

					Hcpl_ASSERT( rShare.nSize <= 100 );
				}
			}
		}

	}


	void GridColorPalette::Core::Prepare()
	{
		//m_nSep = 10;
		m_nSep = 20;

		m_nDimLen = 1 + 255 / m_nSep + 2;

		m_idxCalc.Init( m_nDimLen, m_nDimLen, m_nDimLen );

		m_elmArr.SetSize( m_idxCalc.GetTotalSize() );

		//int i = 0;
		for( int z=0; z < m_idxCalc.GetSizeZ(); z++ )
		{
			for( int y=0; y < m_idxCalc.GetSizeY(); y++ )
			{		
				//for( int x=0; x < m_idxCalc.GetSizeX(); x++, i++ )
				for( int x=0; x < m_idxCalc.GetSizeX(); x++ )
				{
					int nIdx = m_idxCalc.Calc( x, y, z );

					Elm & rElm = m_elmArr[ nIdx ];

					rElm.nIndex = nIdx;

					rElm.X = x;
					rElm.Y = y;
					rElm.Z = z;
				}
			}
		}

	}


}