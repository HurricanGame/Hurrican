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
    Handlung = GEGNER_STEHEN;
    BlickRichtung = LINKS;
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
    xPos = static_cast<float>(TileEngine.XOffset + 230);
    yPos = static_cast<float>(TileEngine.YOffset + 400);

    WarningCount -= 8.0f SYNC;

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
