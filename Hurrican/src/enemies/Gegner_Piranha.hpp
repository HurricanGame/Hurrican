#ifndef _GEGNER_PIRANHA_HPP_
#define _GEGNER_PIRANHA_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerPiranha : public GegnerClass {
  private:
    float xsave, ysave;

  public:
    GegnerPiranha(int Wert1,
                  int Wert2,  // Konstruktor
                  bool Light);
    void GegnerExplode();  // Gegner explodiert
    void DoKI();           // Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw();         // Gegner individuell rendern
};

#endif
