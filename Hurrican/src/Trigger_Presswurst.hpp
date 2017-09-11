#ifndef _TRIGGER_PRESSWURST_HPP_
#define _TRIGGER_PRESSWURST_HPP_

#include "GegnerClass.hpp"
#include "Gegner_Stuff.hpp"

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
