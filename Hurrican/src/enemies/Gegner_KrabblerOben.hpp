#ifndef _GEGNER_KRABBLEROBEN_HPP_
#define _GEGNER_KRABBLEROBEN_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerKrabblerOben : public GegnerClass {
  private:
    float ShotDelay;  // Schussverzögerung

  public:
    GegnerKrabblerOben(int Wert1,
                       int Wert2,  // Konstruktor
                       bool Light);
    void GegnerExplode() override;  // Gegner explodiert
    void DoKI() override;           // Gegner individuell mit seiner eigenen kleinen KI bewegen
};

#endif
