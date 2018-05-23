#ifndef _GEGNER_FALLBOMBE_HPP_
#define _GEGNER_FALLBOMBE_HPP_

#include "GegnerClass.hpp"
#include "Gegner_Stuff.hpp"

class GegnerFallBombe : public GegnerClass {
  private:
    float SmokeDelay;

  public:
    GegnerFallBombe(int Wert1,
                    int Wert2,  // Konstruktor
                    bool Light);
    void GegnerExplode() override;  // Gegner explodiert
    void DoKI() override;           // Gegner individuell mit seiner eigenen kleinen KI bewegen
};

#endif
