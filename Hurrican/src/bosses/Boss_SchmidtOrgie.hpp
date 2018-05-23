#ifndef _BOSS_SCHMIDTORGIE_HPP_
#define _BOSS_SCHMIDTORGIE_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerSchmidtOrgie : public GegnerClass {
  private:
    float ActionCounter;

  public:
    GegnerSchmidtOrgie(int Wert1,
                       int Wert2,  // Konstruktor
                       bool Light);
    void GegnerExplode() override;  // Gegner explodiert
    void DoKI() override;           // Gegner individuell mit seiner eigenen kleinen KI bewegen
};

#endif
