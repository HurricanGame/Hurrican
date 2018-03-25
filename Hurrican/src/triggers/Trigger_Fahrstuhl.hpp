#ifndef _TRIGGER_FAHRSTUHL_HPP_
#define _TRIGGER_FAHRSTUHL_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerFahrstuhl : public GegnerClass {
  private:
    float new_ySpeed;

  public:
    GegnerFahrstuhl(int Wert1,
                    int Wert2,  // Konstruktor
                    bool Light);
    void GegnerExplode();  // Gegner explodiert
    void DoKI();           // Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
    void DoDraw();
};

#endif
