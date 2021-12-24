#ifndef _GEGNER_FETTESPINNE_HPP_
#define _GEGNER_FETTESPINNE_HPP_

#include "GegnerClass.hpp"
#include "Gegner_Stuff.hpp"

class GegnerFetteSpinne : public GegnerClass {
  private:
    float ShotDelay;
    int WalkState;  // 0 == unten, 1 == an der Decke

  public:
    GegnerFetteSpinne(int Wert1,
                      int Wert2,  // Konstruktor
                      bool Light);
    void GegnerExplode() override;  // Gegner explodiert
    void DoKI() override;           // Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw() override;         // Gegner individuell rendern
};

#endif
