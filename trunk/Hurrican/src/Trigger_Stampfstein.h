#ifndef _TRIGGER_STAMPFSTEIN_H_
#define _TRIGGER_STAMPFSTEIN_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerStampfstein : public GegnerClass
{
private:
    float	oldy;

    //DKS - The chain retracting sound was disabled in the original code, but I've
    //      added it back in. In order to allow this, we must keep track of which
    //      channel this particular stampfstein's chain sound is playing on, so that
    //      multiple stampfsteins can play and the correct one will be stopped when
    //      fully retracted. (-1 means it is not playing)
    int sfx_chain_channel;

public:

    GegnerStampfstein	   (int Wert1,	int Wert2,			// Konstruktor
                            bool		  Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
    void DoDraw			(void);
};

#endif
