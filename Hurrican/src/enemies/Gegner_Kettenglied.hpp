#ifndef _GEGNER_KETTENGLIED_H
#define _GEGNER_KETTENGLIED_H

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerKettenglied : public GegnerClass {
  public:
    GegnerClass *pParent;

    GegnerKettenglied(int Wert1,
                      int Wert2,  // Konstruktor
                      bool Light);
    void GegnerExplode() override;  // Gegner explodiert
    void DoKI() override;           // Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw() override;         // Eigene Drawroutine
};

#endif
