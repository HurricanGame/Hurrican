#ifndef _GEGNER_DECKENKRABBE_HPP_
#define _GEGNER_DECKENKRABBE_HPP_

#include "GegnerClass.hpp"
#include "Gegner_Stuff.hpp"

class GegnerDeckenKrabbe : public GegnerClass {
  private:
    float zRot;    // Rotation
    float zSpeed;  // Rotationsspeed

  public:
    GegnerDeckenKrabbe(int Wert1,
                       int Wert2,  // Konstruktor
                       bool Light);
    void GegnerExplode() override;  // Gegner explodiert
    void DoKI() override;           // Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw() override;         // Gegner individuell rendern
};

#endif
