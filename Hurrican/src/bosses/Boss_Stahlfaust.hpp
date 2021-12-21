#ifndef _BOSS_STAHLFAUST_HPP_
#define _BOSS_STAHLFAUST_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerStahlfaust : public GegnerClass {
  private:
    float SmokeDelay;

  public:
    GegnerStahlfaust(int Wert1,
                     int Wert2,  // Konstruktor
                     bool Light);
    void GegnerExplode() override;  // Gegner explodiert
    void DoKI() override;           // Gegner individuell mit seiner eigenen kleinen KI bewegen
};

#endif
