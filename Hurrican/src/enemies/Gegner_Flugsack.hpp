#ifndef _GEGNER_FLUGSACK_HPP_
#define _GEGNER_FLUGSACK_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerFlugsack : public GegnerClass {
  private:
    float ShotCount;   // Schuss Delay
    float SmokeCount;  // Rauch  Delay

  public:
    GegnerFlugsack(int Wert1,
                   int Wert2,  // Konstruktor
                   bool Light);
    void GegnerExplode() override;  // Gegner explodiert
    void DoKI() override;           // Gegner individuell mit seiner eigenen kleinen KI bewegen
};

#endif
