// --------------------------------------------------------------------------------------
// Der Lift
//
// Fährt nach oben, sobald der Hurri draufsteht, und explodiert, wenn der Countdown
// abgelaufen ist, oder wenn der Hurri runterspringt
// Value1 gibt an, wie schnell der Countdown runterzählt
// Value2 speichert die Ursprungs y-Position
// --------------------------------------------------------------------------------------

#include "Trigger_Lift.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerLift::GegnerLift(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::STEHEN;
    BlickRichtung = DirectionEnum::LINKS;
    Energy = 100;
    Value1 = Wert1;
    Value2 = Wert2;
    ChangeLight = Light;
    Destroyable = false;
    TestBlock = false;
    SmokeDelay = 0.0f;
    AnimSpeed = Value1 / 10.0f;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerLift::DoKI() {
    BlickRichtung = DirectionEnum::LINKS;

    // Animieren
    if (AnimEnde > 0)  // Soll überhaupt animiert werden ?
    {
        AnimCount += SpeedFaktor;   // Animationscounter weiterzählen
        if (AnimCount > AnimSpeed)  // Grenze überschritten ?
        {
            AnimCount = 0;              // Dann wieder auf Null setzen
            AnimPhase++;                // Und nächste Animationsphase
            if (AnimPhase >= AnimEnde)  // Animation von zu Ende	?
            {
                for (int i = 0; i < 10; i++)
                    PartikelSystem.PushPartikel(xPos - 30.0f + static_cast<float>(random(80)),
                                                yPos - 20.0f + static_cast<float>(random(20)), EXPLOSION_MEDIUM2);

                Handlung = GEGNER::STEHEN;
                AnimPhase = 0;
                AnimEnde = 0;
                yPos = static_cast<float>(Value2);
                ySpeed = 0.0f;
                yAcc = 0.0f;

                for (int p = 0; p < NUMPLAYERS; p++)
                    Player[p].AufPlattform = nullptr;

                SoundManager.PlayWave(100, 128, 8000 + random(4000), SOUND::EXPLOSION1);
            }
        }
    }  // animieren

    PlattformTest(GegnerRect[GegnerArt]);

    switch (Handlung) {
        // Lift wartet auf den Hurri
        case GEGNER::STEHEN: {
            Value2 = static_cast<int>(yPos);

            for (int p = 0; p < NUMPLAYERS; p++)
                if (Player[p].AufPlattform == this) {
                    Handlung = GEGNER::LAUFEN;
                    ySpeed = -3.0f;
                    yAcc = -3.0f;
                    AnimPhase = 0;
                    AnimEnde = 10;
                    AnimStart = 0;
                }
        } break;

        // Lift fährt nach oben
        case GEGNER::LAUFEN: {
            // Max Speed
            if (ySpeed < -30.0f) {
                yAcc = 0.0f;
                ySpeed = -30.0f;
            }
        } break;
    }
}

// --------------------------------------------------------------------------------------
// Lift explodiert (nicht)
// --------------------------------------------------------------------------------------

void GegnerLift::GegnerExplode() {}
