#ifndef _GEGNER_REITFLUGSACK_HPP_
#define _GEGNER_REITFLUGSACK_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerReitFlugsack : public GegnerClass {
  private:
    float ShotCount;   // Schuss Delay
    float SmokeCount;  // Rauch  Delay
    float ArrowCount;

  public:
    GegnerReitFlugsack(int Wert1,
                       int Wert2,  // Konstruktor
                       bool Light);
    void GegnerExplode(void);  // Gegner explodiert
    void DoKI(void);           // Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw(void);
};

#endif
