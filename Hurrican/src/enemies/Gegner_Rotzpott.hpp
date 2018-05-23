#ifndef _GEGNER_ROTZPOTT_HPP_
#define _GEGNER_ROTZPOTT_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerRotzpott : public GegnerClass {
  private:
    float GunWinkel;
    float ShotDelay;
    float NewWinkel;

  public:
    GegnerRotzpott(int Wert1,
                   int Wert2,  // Konstruktor
                   bool Light);
    void GegnerExplode() override;  // Gegner explodiert
    void DoKI() override;           // Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw() override;         // Eigene Drawroutine
    void CalcKnarreWinkel();
};

#endif
