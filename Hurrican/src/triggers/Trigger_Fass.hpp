#ifndef _TRIGGER_FASS_HPP_
#define _TRIGGER_FASS_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerFass : public GegnerClass {
  public:
    GegnerFass(int Wert1,
               int Wert2,  // Konstruktor
               bool Light);
    void GegnerExplode() override;  // Gegner explodiert
    void DoKI() override;           // Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
