#ifndef _GEGNER_LAVABALLSPAWNER_HPP_
#define _GEGNER_LAVABALLSPAWNER_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerLavaBallSpawner : public GegnerClass {
  private:
    float Delay;

  public:
    GegnerLavaBallSpawner(int Wert1,
                          int Wert2,  // Konstruktor
                          bool Light);
    void GegnerExplode() override;  // Gegner explodiert
    void DoKI() override;           // Gegner individuell mit seiner eigenen kleinen KI bewegen
};

#endif
