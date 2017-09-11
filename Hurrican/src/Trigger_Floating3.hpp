#ifndef _TRIGGER_FLOATING3_HPP_
#define _TRIGGER_FLOATING3_HPP_

#include "GegnerClass.hpp"
#include "Gegner_Stuff.hpp"

class GegnerFloating3 : public GegnerClass
{
private :
    bool	playeron;
    float	winkel;
    float	drehdir;
    float	turncount;

public:

    GegnerFloating3	   (int Wert1,	int Wert2,			// Konstruktor
                        bool		  Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner bewegen
    void DoDraw		   (void);							// Eigene Draw Routine

};

#endif
