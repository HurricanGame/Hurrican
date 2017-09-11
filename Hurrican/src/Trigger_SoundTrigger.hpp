#ifndef _TRIGGER_SOUNDTRIGGER_HPP_
#define _TRIGGER_SOUNDTRIGGER_HPP_

#include "GegnerClass.hpp"
#include "Gegner_Stuff.hpp"

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
