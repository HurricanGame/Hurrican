#ifndef _TRIGGER_FEUERSPUCKER_HPP_
#define _TRIGGER_FEUERSPUCKER_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerFeuerspucker : public GegnerClass {
  private:
    float ShotDelay;

  public:
    GegnerFeuerspucker(int Wert1,
                       int Wert2,  // Konstruktor
                       bool Light);
    void GegnerExplode();  // Gegner explodiert
    void DoKI();           // Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
