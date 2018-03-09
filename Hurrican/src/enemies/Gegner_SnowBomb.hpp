#ifndef _GEGNER_SNOWBOMB_H
#define _GEGNER_SNOWBOMB_H

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerSnowBomb : public GegnerClass {
  public:
    GegnerSnowBomb(int Wert1,
                   int Wert2,  // Konstruktor
                   bool Light);
    void GegnerExplode(void);  // Gegner explodiert
    void DoKI(void);           // Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw(void);         // Gegner individuell rendern
};

#endif
