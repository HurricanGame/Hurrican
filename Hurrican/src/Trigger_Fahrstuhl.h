#ifndef _TRIGGER_FAHRSTUHL_H_
#define _TRIGGER_FAHRSTUHL_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerFahrstuhl : public GegnerClass
{
private:
    float			new_ySpeed;

public:

    GegnerFahrstuhl   (int Wert1,	int Wert2,			// Konstruktor
                       bool		  Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
    void DoDraw			(void);
};

#endif
