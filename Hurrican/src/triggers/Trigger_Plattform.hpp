#ifndef _TRIGGER_PLATTFORM_HPP_
#define _TRIGGER_PLATTFORM_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerPlattform : public GegnerClass {
  public:
    GegnerPlattform(int Wert1,
                    int Wert2,  // Konstruktor
                    bool Light);
    void GegnerExplode();  // Gegner explodiert
    void DoKI();           // Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
