#ifndef _TRIGGER_BRUECKE_HPP_
#define _TRIGGER_BRUECKE_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerBruecke : public GegnerClass {
  public:
    GegnerBruecke(int Wert1,
                  int Wert2,  // Konstruktor
                  bool Light);
    void GegnerExplode() override;  // Gegner explodiert
    void DoKI() override;           // Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
};

#endif
