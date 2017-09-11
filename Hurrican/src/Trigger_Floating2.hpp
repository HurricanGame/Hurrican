#ifndef _TRIGGER_FLOATING2_HPP_
#define _TRIGGER_FLOATING2_HPP_

#include "GegnerClass.hpp"
#include "Gegner_Stuff.hpp"

class GegnerFloating2 : public GegnerClass
{
private :
    bool	playeron;

public:

    GegnerFloating2	   (int Wert1,	int Wert2,			// Konstruktor
                        bool		  Light);
    void GegnerExplode (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
