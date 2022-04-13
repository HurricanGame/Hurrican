#ifndef _BOSS_EVILHURRI_HPP_
#define _BOSS_EVILHURRI_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerEvilHurri : public GegnerClass {
  private:
    DirectionEnum BlinkDirection;  // Animationsphase der Beine
    float BlinkWert;       // Y-Offset des Körpers
    float ActionDelay;     // Warten

  public:
    GegnerEvilHurri(int Wert1,
                    int Wert2,  // Konstruktor
                    bool Light);
    void GegnerExplode() override;  // Gegner explodiert
    void DoKI() override;           // Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw() override;         // Gegner individuell rendern
};

#endif
