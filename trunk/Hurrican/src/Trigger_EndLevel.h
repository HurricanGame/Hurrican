#ifndef _TRIGGER_ENDLEVEL_H_
#define _TRIGGER_ENDLEVEL_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerEndLevel : public GegnerClass
{
public:

    GegnerEndLevel   (int Wert1,	int Wert2,			// Konstruktor
                      bool		  Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
