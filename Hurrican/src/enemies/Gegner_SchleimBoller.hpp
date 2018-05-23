#ifndef _GEGNER_SCHLEIMBOLLER_HPP_
#define _GEGNER_SCHLEIMBOLLER_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerSchleimBoller : public GegnerClass {
  private:
    int Size;

  public:
    GegnerSchleimBoller(int Wert1,
                        int Wert2,  // Konstruktor
                        bool Light);
    void GegnerExplode() override;  // Gegner explodiert
    void DoKI() override;           // Gegner individuell mit seiner eigenen kleinen KI bewegen
    void DoDraw() override;         // Gegner individuell rendern
};

#endif
