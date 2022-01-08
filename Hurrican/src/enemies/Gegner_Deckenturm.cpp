// --------------------------------------------------------------------------------------
// Der Geschützturm an der Decke
//
// Hängt geschlossen und unverwundbar an der Decke, und öffnet sich,
// sobald der Spieler in Reichweite kommt und schiesst dann in die Richtung des Spielers
// --------------------------------------------------------------------------------------

#include "Gegner_Deckenturm.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerDeckenturm::GegnerDeckenturm(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER_UNVERWUNDBAR;
    BlickRichtung = LINKS;
    Energy = 50;
    Value1 = Wert1;
    Value2 = Wert2;
    AnimSpeed = 1.0f;
    ChangeLight = Light;
    Destroyable = true;
    TestBlock = false;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerDeckenturm::DoKI() {
    BlickRichtung = LINKS;

    SimpleAnimation();

    // Je nach Handlung richtig verhalten
    switch (Handlung) {
        case GEGNER_UNVERWUNDBAR:  // Kanone hängt an der Decke und wartet
                                   // bis der Spieler in seine Reichweite kommt
        {
            DamageTaken = 0.0f;               // Auch nicht rot leuchten
            Destroyable = false;              // Gegner wird unverwundbar
            if (PlayerAbstand() <= Value2 &&  // Spieler in Sichtweite ? Dann aufmachen
                pAim->ypos + 70 >= yPos) {
                Destroyable = true;  // Gegner wird verwundbar
                Handlung = GEGNER_OEFFNEN;
                AnimPhase = 1;
                AnimStart = 0;
                AnimEnde = 10;
                AnimCount = 0.0f;
            }
        } break;

        case GEGNER_OEFFNEN: {
            if (AnimPhase == AnimStart)  // Fertig mit öffnen ?
            {
                Handlung = GEGNER_VERFOLGEN;
                AnimPhase = 10;
                AnimEnde = AnimPhase;
                AnimStart = AnimPhase;
                AnimCount = 0;
                AnimSpeed = static_cast<float>(Value1);
            }
        } break;

        case GEGNER_SCHLIESSEN: {
            if (AnimPhase == AnimStart)  // Fertig mit schliessen ?
            {
                Handlung = GEGNER_UNVERWUNDBAR;
                AnimEnde = 0;
                AnimStart = 0;
            }
        } break;

        case GEGNER_VERFOLGEN:  // Auf den Spieler ballern oder ggf
        {
            // wieder zusammenklappen
            // Je nach Winkel zum Spieler die Kanone richtig ausrichten
            float d1, d2;

            // Hypothenuse
            d1 = static_cast<float>(PlayerAbstand());

            // Ankathete
            d2 = (pAim->xpos + pAim->CollideRect.left + (pAim->CollideRect.right - pAim->CollideRect.left) / 2) -
                 (xPos + GegnerRect[GegnerArt].right / 2);

            // DKS - Use new Rad/Deg macros:
            // Winkel = (d2/d1)*180/PI;
            Winkel = RadToDeg(d2 / d1);

            AnimPhase = static_cast<int>(16 - Winkel / 12);

            // Nicht über die Seitengrenze hinausgehen
            AnimPhase = std::clamp(AnimPhase, 10, 21);

            AnimEnde = AnimPhase;
            AnimStart = AnimPhase;

            // Schiessen
            if (AnimCount == 0.0f) {
                // DKS - Support new trig sin/cos lookup table and use deg/rad versions of sin/cos:
                // float w = Winkel * PI / 180.0f;
                // Projectiles.PushProjectile(xPos + 10 + static_cast<float>(sin(w) * 20.0f),
                //                             yPos + 5 + static_cast<float>(cos(w) * 30.0f), SUCHSCHUSS);
                Projectiles.PushProjectile(xPos + 10.0f + (sin_deg(Winkel) * 20.0f),
                                           yPos + 5.0f + (cos_deg(Winkel) * 30.0f), SUCHSCHUSS);
            }

            if (PlayerAbstand() > Value2 ||  // Spieler ausserhalb der Sichtweite ? Dann zumachen
                pAim->ypos + 70 < yPos) {
                Handlung = GEGNER_SCHLIESSEN;
                AnimStart = 0;
                AnimPhase = 22;
                AnimEnde = 30;
                AnimSpeed = 1.0f;
                AnimCount = 0.0f;
            }
        } break;

        default:
            break;
    }  // switch
}

// --------------------------------------------------------------------------------------
// Deckenturm explodiert
// --------------------------------------------------------------------------------------

void GegnerDeckenturm::GegnerExplode() {
    // Explosion
    PartikelSystem.PushPartikel(static_cast<float>(xPos - 10), static_cast<float>(yPos - 10), EXPLOSION_MEDIUM2);

    SoundManager.PlayWave(100, 128, -random(2000) + 11025, SOUND_EXPLOSION1);  // Sound ausgeben

    Player[0].Score += 200;
}
