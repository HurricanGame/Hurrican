#ifndef _TRIGGER_PRESSWURST_H_
#define _TRIGGER_PRESSWURST_H_

#include "GegnerClass.h"
#include "Gegner_Stuff.h"

class GegnerPresswurst : public GegnerClass
{
private:
    float	oldy;
    float	SmokeCount;

public:

    GegnerPresswurst	   (int Wert1,	int Wert2,			// Konstruktor
                            bool		  Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
    void DoDraw		   (void);
};

#endif
