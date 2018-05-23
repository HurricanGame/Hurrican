#ifndef _GEGNER_MINIDRAGON_HPP_
#define _GEGNER_MINIDRAGON_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerMiniDragon : public GegnerClass {
  private:
    float NewX, NewY;
    float OldX, OldY;
    int Segments;
    float ShotDelay;

  public:
    GegnerMiniDragon(int Wert1,
                     int Wert2,  // Konstruktor
                     bool Light);
    void DoKI() override;           // Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw() override;         // Gegner individuell rendern
    void GegnerExplode() override;  // Gegner explodiert
};

#endif
