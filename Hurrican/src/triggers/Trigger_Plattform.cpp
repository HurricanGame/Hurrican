// --------------------------------------------------------------------------------------
// Die Plattform
//
// Fährt mit x und y Geschwindigkeit die den Values entsprechen in der Luft rum
// und dreht an einem WendePunkt um
// --------------------------------------------------------------------------------------

#include "Trigger_Plattform.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerPlattform::GegnerPlattform(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::STEHEN;
    BlickRichtung = DirectionEnum::LINKS;
    Energy = 100;
    Value1 = Wert1;
    Value2 = Wert2;
    xSpeed = static_cast<float>(Wert1);
    ySpeed = static_cast<float>(Wert2);

    ChangeLight = Light;
    Destroyable = false;
    Active = true;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerPlattform::DoKI() {
    if (TileEngine.GetUsedPowerBlock() == 2)
        AnimPhase = 1;
    else
        AnimPhase = 0;

    // An den Wendepunkten umdrehen

    if (blockl & BLOCKWERT_WENDEPUNKT)
        xSpeed = static_cast<float>(Value1);

    if (blockr & BLOCKWERT_WENDEPUNKT)
        xSpeed = -static_cast<float>(Value1);

    if (blocko & BLOCKWERT_WENDEPUNKT)
        ySpeed = static_cast<float>(Value2);

    if (blocku & BLOCKWERT_WENDEPUNKT)
        ySpeed = -static_cast<float>(Value2);

    // Spieler steht auf der Plattform
    //
    PlattformTest(GegnerRect[GegnerArt]);
}

// --------------------------------------------------------------------------------------
// Plattform explodiert
// --------------------------------------------------------------------------------------

void GegnerPlattform::GegnerExplode() {}
