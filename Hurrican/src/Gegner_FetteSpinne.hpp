#ifndef _GEGNER_FETTESPINNE_HPP_
#define _GEGNER_FETTESPINNE_HPP_

#include "GegnerClass.hpp"
#include "Gegner_Stuff.hpp"

class GegnerFetteSpinne : public GegnerClass
{
public :
    float	ShotDelay;
    int		WalkState;			// 0 == unten, 1 == an der Decke

    GegnerFetteSpinne (int  Wert1, int Wert2,				// Konstruktor
                       bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw		   (void);							// Gegner individuell rendern
};

#endif