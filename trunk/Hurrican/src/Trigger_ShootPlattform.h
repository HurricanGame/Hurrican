#ifndef _TRIGGER_SHOOTPLATTFORM_H_
#define _TRIGGER_SHOOTPLATTFORM_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerShootPlattform : public GegnerClass
{
private:
    GegnerClass		*pButton;						// Zeiger auf den Button, der die Plattform vorantreibt

public:

    GegnerShootPlattform  (int Wert1,	int Wert2,		// Konstruktor
                           bool Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
    void DoDraw		   (void);
};

#endif
