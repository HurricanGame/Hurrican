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
    void GegnerExplode();  // Gegner explodiert
    void DoKI();           // Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw();         // Eigene Drawroutine
};

#endif
