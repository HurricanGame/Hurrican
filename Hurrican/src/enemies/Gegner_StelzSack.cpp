// --------------------------------------------------------------------------------------
// Der StelzSack
//
// fällt in den Fahrstuhl rein und ballert dann los
// --------------------------------------------------------------------------------------

#include "Gegner_StelzSack.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerStelzSack::GegnerStelzSack(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::NOTVISIBLE;
    Energy = 80;
    if (Wert1 == 0)
        Value1 = static_cast<int>(g_Fahrstuhl_Speed + 25.0f);
    else
        Value1 = Wert1;
    Value2 = Wert2;
    ChangeLight = Light;
    Destroyable = true;
    AnimStart = 0;
    AnimSpeed = 0.75f;
    ySpeed = g_Fahrstuhl_Speed;
    yAcc = 10.0f;

    // Spieler ankucken
    if (xPos + 50 > pAim->xpos + 35)
        BlickRichtung = DirectionEnum::LINKS;
    else
        BlickRichtung = DirectionEnum::RECHTS;

    TestBlock = false;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerStelzSack::DoKI() {
    // y-Position auf Höhe des Fahrstuhls setzen
    if (Handlung != GEGNER::INIT && Handlung != GEGNER::FALLEN)
        yPos = g_Fahrstuhl_yPos - 147;

    switch (Handlung) {
        // Sack wird "aktiviert" (über den Hurri gesetzt damit er von dort runterfallen kann)
        case GEGNER::NOTVISIBLE: {
            Handlung = GEGNER::FALLEN;
            yPos -= 480.0f + 160.0f;
            if (yPos < 0.0f)
                yPos = 0.0f;
        } break;

        // Sack fällt runter
        case GEGNER::FALLEN: {
            if (ySpeed > 80.0f + g_Fahrstuhl_Speed) {
                ySpeed = 80.0f + g_Fahrstuhl_Speed;
                yAcc = 0.0f;
            }

            if (yPos + 147 > g_Fahrstuhl_yPos) {
                ySpeed = 0.0f;
                yPos = g_Fahrstuhl_yPos - 147;
                Handlung = GEGNER::STEHEN;
                AnimEnde = 10;
                AnimStart = 9;
                AnimPhase = 2;
                AnimSpeed = 0.75f;
            }
        } break;

        case GEGNER::STEHEN:  // Aufkommen und abfedern
        {
            AnimCount += SpeedFaktor;   // Animationscounter weiterzählen
            if (AnimCount > AnimSpeed)  // Grenze überschritten ?
            {
                AnimCount = 0;       // Dann wieder auf Null setzen
                AnimPhase++;         // Und nächste Animationsphase
                if (AnimPhase >= 9)  // Animation von zu Ende	?
                {
                    AnimPhase = 9;
                    AnimEnde = 0;
                    AnimCount = 0.0f;
                    Handlung = GEGNER::LAUFEN;
                }
            }
        } break;

        case GEGNER::LAUFEN:  // rumstehen und ballern oder drehen
        {
            // Schiessen ?
            AnimCount += Timer.sync(1.0f);

            if (AnimCount >= 12.0f) {
                AnimCount = 0.0f;

                SoundManager.PlayWave(100, 128, 18000 + random(4000), SOUND::LASERSHOT);

                if (BlickRichtung == DirectionEnum::LINKS) {
                    PartikelSystem.PushPartikel(xPos - 50.0f, yPos, STELZFLARE);
                    Projectiles.PushProjectile(xPos - 30.0f, yPos + 25.0f, STELZLASER);
                } else {
                    PartikelSystem.PushPartikel(xPos + 40.0f, yPos, STELZFLARE);
                    Projectiles.PushProjectile(xPos + 50.0f, yPos + 25.0f, STELZLASER2);
                }
            }

            // Rumdrehen ?
            if ((BlickRichtung == DirectionEnum::LINKS && xPos + 50 < pAim->xpos + 35) ||
                (BlickRichtung == DirectionEnum::RECHTS && xPos + 50 > pAim->xpos + 35)) {
                Handlung = GEGNER::DREHEN;
                AnimCount = 0.0f;
            }
        } break;

        case GEGNER::DREHEN:  // rumdrehen
        {
            AnimCount += SpeedFaktor;   // Animationscounter weiterzählen
            if (AnimCount > AnimSpeed)  // Grenze überschritten ?
            {
                AnimCount = 0.0f;     // Dann wieder auf Null setzen
                AnimPhase++;          // Und nächste Animationsphase
                if (AnimPhase >= 15)  // Animation von zu Ende	?
                {
                    AnimPhase = 13;
                    AnimCount = 0.0f;
                    Handlung = GEGNER::DREHEN2;
                    BlickRichtung = Direction::invert(BlickRichtung);
                }
            }
        } break;

        case GEGNER::DREHEN2:  // von der Mitte aus fertig rumdrehen
        {
            AnimCount += SpeedFaktor;   // Animationscounter weiterzählen
            if (AnimCount > AnimSpeed)  // Grenze überschritten ?
            {
                AnimCount = 0.0f;    // Dann wieder auf Null setzen
                AnimPhase--;         // Und nächste Animationsphase
                if (AnimPhase <= 9)  // Animation von zu Ende	?
                {
                    AnimPhase = 9;
                    AnimCount = 0.0f;
                    Handlung = GEGNER::LAUFEN;
                }
            }
        } break;

        default:
            break;
    }  // switch

    // Testen, ob der Spieler den Stelzsack berührt hat
    TestDamagePlayers(Timer.sync(5.0f));
}

// --------------------------------------------------------------------------------------
// StelzSack explodiert
// --------------------------------------------------------------------------------------

void GegnerStelzSack::GegnerExplode() {
    PartikelSystem.PushPartikel(xPos, yPos, STELZHEAD);

    PartikelSystem.PushPartikel(xPos - 20.0f, yPos - 10.0f, STELZE);
    PartikelSystem.PushPartikel(xPos + 20.0f, yPos - 20.0f, STELZE);

    SoundManager.PlayWave(100, 128, 8000 + random(4000), SOUND::EXPLOSION4);

    Player[0].Score += 500;  // Punkte geben
}
