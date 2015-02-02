#ifndef _GEGNER_PIRANHA_H_
#define _GEGNER_PIRANHA_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerPiranha : public GegnerClass
{
private:
    float xsave, ysave;
public:

    GegnerPiranha	   (int Wert1, int Wert2,			// Konstruktor
                        bool		  Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw		   (void);							// Gegner individuell rendern
};

#endif
