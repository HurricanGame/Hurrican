#ifndef _GEGNER_LAVABALL_HPP_
#define _GEGNER_LAVABALL_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerLavaBall : public GegnerClass {
  public:
    float yStart;
    float SmokeDelay, FlareDelay;
    bool InLava;

    GegnerLavaBall(int Wert1,
                   int Wert2,  // Konstruktor
                   bool Light);
    void GegnerExplode() override;  // Gegner explodiert
    void DoKI() override;           // Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw() override;         // Eigene Drawroutine
};

#endif
