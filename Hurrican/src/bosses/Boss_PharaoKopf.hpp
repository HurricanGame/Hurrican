#ifndef _BOSS_PHARAOKOPF_HPP_
#define _BOSS_PHARAOKOPF_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

#define BORDER  25
#define BORDER2 20

class GegnerPharaoKopf : public GegnerClass
{
public:

    GegnerPharaoKopf   (int Wert1,	int Wert2,			// Konstruktor
                        bool		  Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner

    bool Links		   (void);
    bool Rechts		   (void);
    bool Unten		   (void);
    bool Oben		   (void);
    // eigenen kleinen KI bewegen
};

#endif
