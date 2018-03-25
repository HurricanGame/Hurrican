#ifndef _GEGNER_PFLANZE_HPP_
#define _GEGNER_PFLANZE_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerPflanze : public GegnerClass {
  public:
    GegnerPflanze(int Wert1,
                  int Wert2,  // Konstruktor
                  bool Light);
    void GegnerExplode();  // Gegner explodiert
    void DoKI();           // Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
