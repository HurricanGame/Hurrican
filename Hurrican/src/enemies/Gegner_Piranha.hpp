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
    void GegnerExplode() override;  // Gegner explodiert
    void DoKI() override;           // Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw() override;         // Gegner individuell rendern
};

#endif
