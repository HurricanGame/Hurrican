// --------------------------------------------------------------------------------------
// Die kleine schwebende Plattform
//
// Geht ein Stück nach unten, wenn der Hurri draufhopst
// Je schneller die Fallgeschwindigkeit beim aufkommen, desto weiter geht sie nach unten
// --------------------------------------------------------------------------------------

#include "Trigger_Floating.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerFloating::GegnerFloating(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER_LAUFEN;
    BlickRichtung = RECHTS;
    Energy = 100;
    Value1 = Wert1;
    Value2 = Wert2;
    if (Value1 == 0)
        Value1 = 16;
    ChangeLight = Light;
    Destroyable = false;
    AnimCount = 0.0f;
    playeron = false;
}

// --------------------------------------------------------------------------------------
// Floating KI
// --------------------------------------------------------------------------------------

void GegnerFloating::DoKI() {
    BlickRichtung = RECHTS;

    playeron = false;
    for (int p = 0; p < NUMPLAYERS; p++)
        if (Player[p].AufPlattform == this)
            playeron = true;

    PlattformTest(GegnerRect[GegnerArt]);

    // Spieler in diesem Frame auf die Plattform gehopst ?
    //
    for (int p = 0; p < NUMPLAYERS; p++)
        if (Player[p].AufPlattform == this && playeron == false) {
            playeron = true;

            // Plattform dopst kurz runter
            //
            if (ySpeed == 0.0f) {
                Value2 = static_cast<int>(yPos);
                ySpeed = static_cast<float>(Player[p].yspeed) / 3.0f;
                yAcc = -3.0f;
            }
        }

    // Plattform wieder an Ausgangspos ?
    //
    if (ySpeed < 0.0f && static_cast<int>(yPos) <= Value2) {
        ySpeed = 0.0f;
        yAcc = 0.0f;
        yPos = static_cast<float>(Value2);
    }

    // Counter für Partikel runterzählen
    //
    AnimCount -= 1.0f SYNC;

    // Partikel erzeugen und counter wieder hochsetzen
    //
    if (AnimCount <= 0.0f) {
        AnimCount = 0.15f;
        PartikelSystem.PushPartikel(xPos + 14.0f, yPos + 5.0f, FLOATSMOKE);
    }
}

// --------------------------------------------------------------------------------------
// Floating explodiert (nicht)
// --------------------------------------------------------------------------------------

void GegnerFloating::GegnerExplode() {}
