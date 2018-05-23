#ifndef _BOSS_UFO_HPP_
#define _BOSS_UFO_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerUfo : public GegnerClass {
  public:
    float ShotDelay;    // Schussverzögerung
    float ActionDelay;  // Wie lange wird die aktuelle Aktion durchgeführt
    float SmokeDelay;

    GegnerUfo(int Wert1,
              int Wert2,  // Konstruktor
              bool Light);
    void GegnerExplode() override;  // Gegner explodiert
    void DoKI() override;           // Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw() override;
};

#endif
