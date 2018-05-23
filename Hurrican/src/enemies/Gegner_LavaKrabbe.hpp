#ifndef _GEGNER_LAVAKRABBE_HPP_
#define _GEGNER_LAVAKRABBE_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerLavaKrabbe : public GegnerClass {
  public:
    float zPos;  // zPos da die Krabbe ja aus dem Hintergrund kommt

    GegnerLavaKrabbe(int Wert1,
                     int Wert2,  // Konstruktor
                     bool Light);
    void GegnerExplode() override;  // Gegner explodiert
    void DoKI() override;           // Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw() override;         // Gegner individuell rendern
};

#endif
