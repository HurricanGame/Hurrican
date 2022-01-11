// --------------------------------------------------------------------------------------
// Schalter
//
// Lässt sich mit Druck auf die Taste "nach oben kucken" aktivieren
// Aktiviert das Objekt, welches den gleichen Wert für Value2 eingestellt hat, wie der
// Schalter selbst. Auch meherer Aktivierungen gleichzeitig sind möglich, falls
// mehrere Objekte das gleiche Value2 besitzen. Am besten Value2 > 250
// --------------------------------------------------------------------------------------

#include "Trigger_Switch.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerSwitch::GegnerSwitch(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::STEHEN;
    BlickRichtung = LINKS;
    Energy = 100;
    Value1 = Wert1;
    Value2 = Wert2;
    ChangeLight = Light;
    Destroyable = false;
    TestBlock = false;
}

// --------------------------------------------------------------------------------------
// "Switch KI"
// --------------------------------------------------------------------------------------

void GegnerSwitch::DoKI() {
    // Testen, ob der Spieler den Schalter berührt hat und ihn durch Druck auf die "nach oben kucken" Taste
    // aktiviert
    //
    for (int p = 0; p < NUMPLAYERS; p++)
        if (SpriteCollision(xPos, yPos, GegnerRect[GegnerArt], Player[p].xpos, Player[p].ypos, Player[p].CollideRect) ==
                true &&
            Handlung == GEGNER::STEHEN && Player[p].Aktion[AKTION_OBEN] == true) {
            // Schalter umlegen
            AnimPhase = 1;
            Handlung = GEGNER::LAUFEN;

            SoundManager.PlayWave(100, 128, 11025, SOUND::SWITCH);

            // Und prüfen, ob ein Objekt aktiviert wird
            GegnerClass *pTemp = Gegner.pStart;  // Anfang der Liste

            while (pTemp != nullptr)  // noch nicht alle durch ?
            {
                // Value2 stimmt mit dem des Schalters überein? Dann soll dieses Objekt vom Schalter aktiviert werden
                if (pTemp != this && pTemp->Value2 == Value2) {
                    pTemp->Handlung = GEGNER::OEFFNEN;
                }

                pTemp = pTemp->pNext;  // Nächsten Gegner durchgehen
            }
        }
}

// --------------------------------------------------------------------------------------
// Switch explodiert (nicht)
// --------------------------------------------------------------------------------------

void GegnerSwitch::GegnerExplode() {}
