#ifndef _GEGNER_FLUGKANONE_HPP_
#define _GEGNER_FLUGKANONE_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerFlugKanone : public GegnerClass {
  private:
    float ShotDelay;

  public:
    GegnerFlugKanone(int Wert1,
                     int Wert2,  // Konstruktor
                     bool Light);
    void GegnerExplode();  // Gegner explodiert
    void DoKI();           // Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
