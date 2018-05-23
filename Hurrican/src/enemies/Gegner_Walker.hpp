#ifndef _GEGNER_WALKER_H
#define _GEGNER_WALKER_H

#include "GegnerClass.hpp"
#include "Gegner_Stuff.hpp"

class GegnerWalker : public GegnerClass {
  private:
    float ShotDelay;

  public:
    GegnerWalker(int Wert1,
                 int Wert2,  // Konstruktor
                 bool Light);
    void GegnerExplode() override;  // Gegner explodiert
    void DoKI() override;           // Gegner individuell mit seiner eigenen kleinen KI bewegen
};

#endif
