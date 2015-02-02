#ifndef _TRIGGER_FLOATING3_H_
#define _TRIGGER_FLOATING3_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

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
