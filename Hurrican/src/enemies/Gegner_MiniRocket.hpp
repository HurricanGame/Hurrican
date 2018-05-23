#ifndef _GEGNER_MINIROCKET_HPP_
#define _GEGNER_MINIROCKET_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerMiniRocket : public GegnerClass {
  private:
    float SmokeDelay;
    float FlareSin;
    float rot;

  public:
    GegnerMiniRocket(int Wert1,
                     int Wert2,  // Konstruktor
                     bool Light);
    void GegnerExplode() override;  // Gegner explodiert
    void DoKI() override;           // Gegner individuell mit seiner
    void DoDraw() override;         // Gegner individuell rendern eigenen kleinen KI bewegen
};

#endif
