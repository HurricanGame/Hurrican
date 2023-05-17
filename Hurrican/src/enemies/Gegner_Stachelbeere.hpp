#ifndef _GEGNER_STACHELBEERE_HPP_
#define _GEGNER_STACHELBEERE_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerStachelbeere : public GegnerClass {
  private:
    int ShotCount;
    float RollCount;

  public:
    GegnerStachelbeere(int Wert1,
                       int Wert2,  // Konstruktor
                       bool Light);
    void GegnerExplode() override;  // Gegner explodiert
    void DoKI() override;           // Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw() override;
};

#endif
