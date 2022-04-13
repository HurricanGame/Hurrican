#ifndef _BOSS_RIESENRAUPE_HPP_
#define _BOSS_RIESENRAUPE_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerRiesenRaupe : public GegnerClass {
  private:
    int ShotCount;

  public:
    GegnerRiesenRaupe(int Wert1,
                      int Wert2,  // Konstruktor
                      bool Light);
    void GegnerExplode() override;  // Gegner explodiert
    void DoKI() override;           // Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
