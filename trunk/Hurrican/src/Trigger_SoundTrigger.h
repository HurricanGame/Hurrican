#ifndef _TRIGGER_SOUNDTRIGGER_H_
#define _TRIGGER_SOUNDTRIGGER_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerSoundTrigger : public GegnerClass
{
public:

    GegnerSoundTrigger  (int Wert1,	int Wert2,			// Konstruktor
                         bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
