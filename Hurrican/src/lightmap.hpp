//DKS - Lightmaps have been disabled (never worked originally, see Tileengine.cpp's
//      comments for DrawLightmap()), so all the following is commented out now:
#if 0

// Datei : lightmap.hpp

// --------------------------------------------------------------------------------------
//
// Lightmaps für das Hurrican Level
//
// (c) 2005 Jörg M. Winterstein
//
// --------------------------------------------------------------------------------------

#ifndef _LIGHTMAP_HPP_
#define _LIGHTMAP_HPP_

// --------------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------------

#include "Globals.hpp"


// --------------------------------------------------------------------------------------
// LigthMap Klasse
// --------------------------------------------------------------------------------------

class CLightMap
{

public:

    int			xsize, ysize;
    D3DCOLOR*	map;

    CLightMap(void);
    ~CLightMap(void);

    void	  Load(const char *filename);
};

#endif

#endif //0
