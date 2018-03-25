#ifndef _GEGNER_FALLINGROCK_HPP_
#define _GEGNER_FALLINGROCK_HPP_

#include "GegnerClass.hpp"
#include "Gegner_Stuff.hpp"

class GegnerFallingRock : public GegnerClass {
  public:
    GegnerFallingRock(int Wert1,
                      int Wert2,  // Konstruktor
                      bool Light);
    void GegnerExplode();  // Gegner explodiert
    void DoKI();           // Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
