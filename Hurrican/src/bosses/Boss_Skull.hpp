#ifndef _BOSS_SKULL_HPP_
#define _BOSS_SKULL_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerSkull : public GegnerClass {
  private:
    GegnerClass *pMachine;
    int ShotCount;
    float mxPos, myPos;  // Koordinaten der Bossmaschine
    float SmokeDelay;
    float ShotDelay;
    float Disappear;

  public:
    GegnerSkull(int Wert1,
                int Wert2,  // Konstruktor
                bool Light);
    void GegnerExplode() override;  // Gegner explodiert
    void DoKI() override;           // Gegner individuell mit seiner eigenen kleinen KI bewegen
};

#endif
