#ifndef _GEGNER_BROCKELROCK_H
#define _GEGNER_BROCKELROCK_H

#include "GegnerClass.hpp"
#include "Gegner_Stuff.hpp"

class GegnerBrockelRock : public GegnerClass {
  public:
    GegnerBrockelRock(int Wert1,
                      int Wert2,  // Konstruktor
                      bool Light);
    void GegnerExplode() override;  // Gegner explodiert
    void DoKI() override;           // Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw() override;         // Gegner individuell rendern
};

#endif
