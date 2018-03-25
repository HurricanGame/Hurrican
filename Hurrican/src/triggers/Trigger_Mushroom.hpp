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
    void GegnerExplode();  // Gegner explodiert
    void DoKI();           // Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
    void DoDraw();
    void PlayerJumps(PlayerClass *pPlayer);
};

#endif
