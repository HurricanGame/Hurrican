#ifndef _TRIGGER_SECRET_HPP_
#define _TRIGGER_SECRET_HPP_

#include "GegnerClass.hpp"
#include "Gegner_Stuff.hpp"

class GegnerSecret : public GegnerClass
{
public:

    GegnerSecret	   (int Wert1,	int Wert2,			// Konstruktor
                        bool		  Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
