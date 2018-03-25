#ifndef _TRIGGER_FLOATING3_HPP_
#define _TRIGGER_FLOATING3_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerFloating3 : public GegnerClass {
  private:
    bool playeron;
    float winkel;
    float drehdir;
    float turncount;

  public:
    GegnerFloating3(int Wert1,
                    int Wert2,  // Konstruktor
                    bool Light);
    void GegnerExplode();  // Gegner explodiert
    void DoKI();           // Gegner bewegen
    void DoDraw();         // Eigene Draw Routine
};

#endif
