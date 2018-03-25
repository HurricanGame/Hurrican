#ifndef _TRIGGER_TUBE_H
#define _TRIGGER_TUBE_H

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerTube : public GegnerClass {
  private:
    float SinOff;

  public:
    GegnerTube(int Wert1,
               int Wert2,  // Konstruktor
               bool Light);
    void GegnerExplode();  // Gegner explodiert
    void DoKI();           // Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw();         // Zeichnen
};

#endif
