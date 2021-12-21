#ifndef _GEGNER_LAVABALL_HPP_
#define _GEGNER_LAVABALL_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerLavaBall : public GegnerClass {
  private:
    float yStart;
    float SmokeDelay, FlareDelay;
    bool InLava;

  public:
    GegnerLavaBall(int Wert1,
                   int Wert2,  // Konstruktor
                   bool Light);
    void GegnerExplode() override;  // Gegner explodiert
    void DoKI() override;           // Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw() override;         // Eigene Drawroutine
};

#endif
