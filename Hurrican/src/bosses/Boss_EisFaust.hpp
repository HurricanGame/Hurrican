#ifndef _BOSS_EISFAUST_HPP_
#define _BOSS_EISFAUST_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerEisFaust : public GegnerClass {
  public:
    GegnerEisFaust(int Wert1,
                   int Wert2,  // Konstruktor
                   bool Light);
    void GegnerExplode(void);  // Gegner explodiert
    void DoKI(void);           // Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
