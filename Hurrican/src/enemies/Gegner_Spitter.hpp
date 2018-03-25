#ifndef _GEGNER_SPITTER_H
#define _GEGNER_SPITTER_H

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerSpitter : public GegnerClass {
  public:
    GegnerSpitter(int Wert1,
                  int Wert2,  // Konstruktor
                  bool Light);
    void GegnerExplode();  // Gegner explodiert
    void DoKI();           // Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
