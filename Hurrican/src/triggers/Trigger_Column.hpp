#ifndef _TRIGGER_COLUMN_HPP_
#define _TRIGGER_COLUMN_HPP_

#include "GegnerClass.hpp"
#include "enemies/Gegner_Stuff.hpp"

class GegnerColumn : public GegnerClass {
  private:
    float FallSpeed;
    float FallValue;

  public:
    GegnerColumn(int Wert1,
                 int Wert2,  // Konstruktor
                 bool Light);
    void GegnerExplode() override;  // Gegner explodiert
    void DoKI() override;           // Gegner individuell mit seiner
    // eigenen kleinen KI bewegen
    void DoDraw() override;
};

#endif
