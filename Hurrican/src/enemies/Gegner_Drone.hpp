#ifndef _GEGNER_DRONE_HPP_
#define _GEGNER_DRONE_HPP_

#include "GegnerClass.hpp"
#include "Gegner_Stuff.hpp"

class GegnerDrone : public GegnerClass {
  private:
    int ShotCount;      // Wieviele Schüsse abgeben ?
    float ShotDelay;    // Zeitabstand zwischen Schüssen
    float ActionDelay;  // Zeitabstand bis zur nächsten Salve

  public:
    GegnerDrone(int Wert1,
                int Wert2,  // Konstruktor
                bool Light);
    void GegnerExplode() override;  // Gegner explodiert
    void DoKI() override;           // Gegner individuell mit seiner eigenen kleinen KI bewegen
};

#endif
