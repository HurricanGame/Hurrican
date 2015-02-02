#ifndef _GEGNER_DECKENTURM_H_
#define _GEGNER_DECKENTURM_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerDeckenturm : public GegnerClass
{
private:
    float Winkel;
public:

    GegnerDeckenturm  (int  Wert1, int Wert2,			// Konstruktor
                       bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
