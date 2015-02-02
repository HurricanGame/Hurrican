#ifndef _GEGNER_STAHLMUECKE_H_
#define _GEGNER_STAHLMUECKE_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerStahlmuecke : public GegnerClass
{
public:

    GegnerStahlmuecke  (int Wert1, int Wert2,			// Konstruktor
                        bool		  Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw		   (void);

};

#endif
