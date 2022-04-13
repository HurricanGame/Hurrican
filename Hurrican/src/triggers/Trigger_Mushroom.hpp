#ifndef _TRIGGER_MUSHROOM_HPP_
#define _TRIGGER_MUSHROOM_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerMushroom : public GegnerClass {
  private:
    float ySize;
    float yCount;
    float count;

  public:
    GegnerMushroom(int Wert1,
                   int Wert2,  // Konstruktor
                   bool Light);
    void GegnerExplode() override;  // Gegner explodiert
    void DoKI() override;           // Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
    void DoDraw() override;
    void PlayerJumps(PlayerClass *pPlayer);
};

#endif
