#ifndef _TRIGGER_SHOOTBUTTON_H_
#define _TRIGGER_SHOOTBUTTON_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerShootButton : public GegnerClass
{
private:
    GegnerClass		*pPlattForm;					// Zeiger auf die Plattform, die der Button vorantreibt
    float			SmokeCount;						// Antiebsflamme

public:

    GegnerShootButton  (GegnerClass *Plattform);		// Konstruktor
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
    void DoDraw			(void);
};

#endif
