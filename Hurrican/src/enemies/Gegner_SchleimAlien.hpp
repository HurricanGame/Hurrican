#ifndef _GEGNER_SCHLEIMALIEN_HPP_
#define _GEGNER_SCHLEIMALIEN_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerSchleimAlien : public GegnerClass {
  private:
    float Size;

  public:
    GegnerSchleimAlien(int Wert1, int Wert2, bool Light);
    void GegnerExplode();  // Gegner explodiert
    void DoKI();           // Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw();         // Gegner individuell rendern
};

#endif
