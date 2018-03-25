#ifndef _GEGNER_LAVAMANN_H
#define _GEGNER_LAVAMANN_H

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerLavamann : public GegnerClass {
  private:
    float FlameCount;

  public:
    GegnerLavamann(int Wert1,
                   int Wert2,  // Konstruktor
                   bool Light);
    void GegnerExplode();  // Gegner explodiert
    void DoKI();           // Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw();         // Gegner individuell rendern
};

#endif
