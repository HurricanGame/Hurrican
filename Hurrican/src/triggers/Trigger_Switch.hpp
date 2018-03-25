#ifndef _TRIGGER_SWITCH_HPP_
#define _TRIGGER_SWITCH_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerSwitch : public GegnerClass {
  public:
    GegnerSwitch(int Wert1,
                 int Wert2,  // Konstruktor
                 bool Light);
    void GegnerExplode();  // Gegner explodiert
    void DoKI();           // Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
