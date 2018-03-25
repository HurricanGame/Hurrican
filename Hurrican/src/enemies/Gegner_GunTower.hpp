#ifndef _GEGNER_GUNTOWER_HPP_
#define _GEGNER_GUNTOWER_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerGunTower : public GegnerClass {
  public:
    GegnerGunTower(int Wert1,
                   int Wert2,  // Konstruktor
                   bool Light);
    void GegnerExplode();  // Gegner explodiert
    void DoKI();           // Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
