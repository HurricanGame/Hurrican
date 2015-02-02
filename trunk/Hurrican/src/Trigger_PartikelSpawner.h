#ifndef _TRIGGER_PARTIKELSPAWNER_H_
#define _TRIGGER_PARTIKELSPAWNER_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerPartikelSpawner : public GegnerClass
{
private:
    float	Aussetzer;
public:

    GegnerPartikelSpawner(int Wert1,	int Wert2,			// Konstruktor
                          bool		  Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
