#ifndef _TRIGGER_LAFASSSPAWNER_H_
#define _TRIGGER_LAFASSSPAWNER_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerLaFassSpawner : public GegnerClass
{
public:

    GegnerLaFassSpawner  (int Wert1,	int Wert2,			// Konstruktor
                          bool		  Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
