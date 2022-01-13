// --------------------------------------------------------------------------------------
// Warning
//
// Blinkt bei erscheinen x mal auf und verursacht Warning Stimme und typisches Hupen
// --------------------------------------------------------------------------------------

#include "Trigger_Warning.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerWarning::GegnerWarning(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::STEHEN;
    BlickRichtung = DirectionEnum::LINKS;
    Energy = 100;
    Value1 = Wert1;
    Value2 = 0;
    ChangeLight = Light;
    Destroyable = false;
    TestBlock = false;
    OwnDraw = true;
    WarningCount = 100.0f;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerWarning::DoKI() {
    xPos = TileEngine.XOffset + 230.0f;
    yPos = TileEngine.YOffset + 400.0f;

    WarningCount -= Timer.sync(8.0f);

    if (WarningCount <= 0.0f) {
        Value2++;

        if (Value2 < 3)
            WarningCount = 100.0f;
        else {
            WarningCount = 0.0f;
            Energy = 0.0f;
        }
    }
}

// --------------------------------------------------------------------------------------
// Warning explodiert
// --------------------------------------------------------------------------------------

void GegnerWarning::GegnerExplode() {}
