#ifndef _BOSS_SKULL_HPP_
#define _BOSS_SKULL_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerSkull : public GegnerClass {
  private:
    float mxPos, myPos;  // Koordinaten der Bossmaschine
    float SmokeDelay;
    float ShotDelay;
    int ShotCount;
    float Disappear;
    GegnerClass *pMachine;

  public:
    GegnerSkull(int Wert1,
                int Wert2,  // Konstruktor
                bool Light);
    void GegnerExplode();  // Gegner explodiert
    void DoKI();           // Gegner individuell mit seiner eigenen kleinen KI bewegen
};

#endif
