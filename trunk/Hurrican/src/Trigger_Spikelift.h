#ifndef _TRIGGER_SPIKELIFT_H_
#define _TRIGGER_SPIKELIFT_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerSpikelift : public GegnerClass
{
private:
    float	oldypos;

public:

    GegnerSpikelift	   (int Wert1,	int Wert2,			// Konstruktor
                        bool		  Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner // eigenen kleinen KI bewegen
    void DoDraw		   (void);							// Eigene Draw Routine

};

#endif
