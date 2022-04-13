#ifndef _TRIGGER_SHOOTPLATTFORM_HPP_
#define _TRIGGER_SHOOTPLATTFORM_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerShootPlattform : public GegnerClass {
  private:
    GegnerClass *pButton;  // Zeiger auf den Button, der die Plattform vorantreibt

  public:
    GegnerShootPlattform(int Wert1,
                         int Wert2,  // Konstruktor
                         bool Light);
    void GegnerExplode() override;  // Gegner explodiert
    void DoKI() override;           // Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
    void DoDraw() override;
};

#endif
