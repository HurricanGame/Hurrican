#ifndef _GEGNER_QUALLE_HPP_
#define _GEGNER_QUALLE_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerQualle : public GegnerClass {
  private:
    float ShotDelay;

  public:
    GegnerQualle(int Wert1,
                 int Wert2,  // Konstruktor
                 bool Light);
    void GegnerExplode() override;  // Gegner explodiert
    void DoKI() override;           // Gegner individuell mit seiner eigenen kleinen KI bewegen
};

#endif
