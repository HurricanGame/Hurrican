// --------------------------------------------------------------------------------------
// Die Röhre, aus der (evtl) ein Mutant hopft =)
// Value 1 gibt an, was genau gerendert werden soll
//		0 : Röhre ganz
//		1 : Röhre kaputt
//		2 : Röhre kaputt nur unten
//		3 : Röhre kaputt nur oben
//		4 : Röhre ganz mit Mutant
//		5 : Röhre ganz mit Mutant der rausspringt
// Value 2 gibt an, ob Flüssigkeit drinsteht oder nicht
// --------------------------------------------------------------------------------------

#include "Trigger_Tube.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerTube::GegnerTube(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::STEHEN;
    Value1 = Wert1;
    Value2 = Wert2;
    ChangeLight = Light;
    Destroyable = false;
    ForceLight = true;
    Energy = 100;
    BackGround = true;
    OwnDraw = true;
    SinOff = 0.0f;
}

// --------------------------------------------------------------------------------------
// Drawroutine
// --------------------------------------------------------------------------------------

void GegnerTube::DoDraw() {
    if (Value1 == 0 || Value1 == 1 || Value1 == 4 || Value1 == 5)
        PlattformTest(GegnerRect[GegnerArt]);

    // Mutant drin?
    if (Value1 >= 4)
        pGegnerGrafix[MUTANT]->RenderSprite(
            xPos - TileEngine.XOffset - 1.0f,
            yPos - TileEngine.YOffset + 60.0f + static_cast<float>(sin(SinOff)) * 1.8f, 0, 0xFFFFFFFF);

    // Flüssigkeit drin?
    if (Value2 == 1) {
        DirectGraphics.SetAdditiveMode();

        if (Value1 % 4 == 0 || Value1 == 5) {
            RenderRect4(xPos - TileEngine.XOffset + 18.0f,
                        yPos - TileEngine.YOffset + 75.0f, 62.0f, 3.0f, 0xFF008822, 0xFF008822, 0xFF004411,
                        0xFF004411);

            RenderRect4(xPos - TileEngine.XOffset + 18.0f,
                        yPos - TileEngine.YOffset + 78.0f, 62.0f, 89.0f, 0xFF004411, 0xFF004411, 0xFF00DD22,
                        0xFF00DD22);
        } else {
            RenderRect4(xPos - TileEngine.XOffset + 18.0f,
                        yPos - TileEngine.YOffset + 145.0f, 62.0f, 3.0f, 0xFF008822, 0xFF008822, 0xFF004411,
                        0xFF004411);

            RenderRect4(xPos - TileEngine.XOffset + 18.0f,
                        yPos - TileEngine.YOffset + 148.0f, 62.0f, 19.0f, 0xFF004411, 0xFF004411, 0xFF008822,
                        0xFF008822);
        }

        DirectGraphics.SetColorKeyMode();
    }

    // Und Röhre drüber
    int a = Value1 % 4;

    if (Value1 == 5)
        a = 0;
    pGegnerGrafix[GegnerArt]->RenderSprite(xPos - TileEngine.XOffset,
                                           yPos - TileEngine.YOffset, a, 0xFFFFFFFF);
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerTube::DoKI() {
    SinOff += Timer.sync(0.3f);

    if (SinOff > TWO_PI)
        SinOff = 0.0f;

    // Mutant kann rausspringen?
    if (Value1 == 5) {
        for (int p = 0; p < NUMPLAYERS; p++)
            if (PlayerAbstand() < 100 && Player[p].ypos > yPos) {
                Value1 = 1;

                for (int i = 0; i < 100; i++)
                    PartikelSystem.PushPartikel(xPos + 5.0f + static_cast<float>(random(55)),
                                                yPos + 70.0f + static_cast<float>(random(60)), GLASSPLITTER);

                Gegner.PushGegner(xPos - 1.0f, yPos + 60.0f, MUTANT, 1, 0, false);
                Gegner.PushGegner(xPos + 50.0f - 10.0f, yPos + 150.0f, PARTIKELSPAWN, 11, 80, false);

                SoundManager.PlayWave(100, 128, 10000 + random(2000), SOUND::GLASSBREAK);
            }
    }
}

// --------------------------------------------------------------------------------------
// LuefterGross explodiert
// --------------------------------------------------------------------------------------

void GegnerTube::GegnerExplode() {}
