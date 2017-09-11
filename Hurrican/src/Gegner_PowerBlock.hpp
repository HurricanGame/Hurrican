#ifndef _GEGNER_POWERBLOCK_HPP_
#define _GEGNER_POWERBLOCK_HPP_

#include "GegnerClass.hpp"
#include "Gegner_Stuff.hpp"

class GegnerPowerBlock : public GegnerClass
{
private:
    int  WeaponSpawned;
    bool AlreadySpawned[16];
public:
    int	ExtraType;

    GegnerPowerBlock	(int Wert1, int Wert2,			// Konstruktor
                         bool			Light);
    void GegnerExplode  (void);							// Gegner explodiert
    void DoKI		   (void);							// Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw		   (void);							// Gegner individuell rendern
};

#endif
