#ifndef _BOSS_RIESENQUALLE_HPP_
#define _BOSS_RIESENQUALLE_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerRiesenQualle : public GegnerClass {
  public:
    GegnerRiesenQualle(int Wert1,
                       int Wert2,  // Konstruktor
                       bool Light);
    void GegnerExplode() override;  // Gegner explodiert
    void DoKI() override;           // Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
