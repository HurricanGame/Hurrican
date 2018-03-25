#ifndef _BOSS_RIESENWESPE_HPP_
#define _BOSS_RIESENWESPE_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerRiesenWasp : public GegnerClass {
  private:
    float ShotDelay;

  public:
    GegnerRiesenWasp(int Wert1,
                     int Wert2,  // Konstruktor
                     bool Light);
    void GegnerExplode();  // Gegner explodiert
    void DoKI();           // Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
