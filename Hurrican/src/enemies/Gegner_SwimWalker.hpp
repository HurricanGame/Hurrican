#ifndef _GEGNER_SWIMWALKER_H
#define _GEGNER_SWIMWALKER_H

#include "GegnerClass.hpp"
#include "Gegner_Stuff.hpp"

class GegnerSwimWalker : public GegnerClass {
  public:
    GegnerSwimWalker(int Wert1,
                     int Wert2,  // Konstruktor
                     bool Light);
    void GegnerExplode() override;  // Gegner explodiert
    void DoKI() override;           // Gegner individuell mit seiner eigenen kleinen KI bewegen
};

#endif
