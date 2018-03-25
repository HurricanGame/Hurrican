#ifndef _TRIGGER_COLUMN2_HPP_
#define _TRIGGER_COLUMN2_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerColumn2 : public GegnerClass {
  private:
    float FallSpeed;
    float FallValue;

  public:
    GegnerColumn2(int Wert1,
                  int Wert2,  // Konstruktor
                  bool Light);
    void GegnerExplode();  // Gegner explodiert
    void DoKI();           // Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
    void DoDraw();
};

#endif
