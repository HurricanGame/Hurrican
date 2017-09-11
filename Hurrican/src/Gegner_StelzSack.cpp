// --------------------------------------------------------------------------------------
// Der StelzSack
//
// fällt in den Fahrstuhl rein und ballert dann los
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Gegner_StelzSack.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerStelzSack::GegnerStelzSack(int Wert1, int Wert2, bool Light)
{
    Handlung		= GEGNER_NOTVISIBLE;
    Energy			= 80;
    Value1			= Wert1;
    Value2			= Wert2;
    if (Wert1 == 0)	Value1 = int (g_Fahrstuhl_Speed + 25);
    ChangeLight		= Light;
    Destroyable		= true;
    AnimStart		= 0;
    AnimSpeed		= 0.75f;
    ySpeed			= g_Fahrstuhl_Speed;
    yAcc			= 10.0f;

    // Spieler ankucken
    if (xPos + 50 > pAim->xpos + 35)
        BlickRichtung = LINKS;
    else
        BlickRichtung = RECHTS;

    TestBlock = false;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerStelzSack::DoKI(void)
{
    // y-Position auf Höhe des Fahrstuhls setzen
    if (Handlung != GEGNER_INIT &&
            Handlung != GEGNER_FALLEN)
        yPos = g_Fahrstuhl_yPos - 147;

    switch (Handlung)
    {
    // Sack wird "aktiviert" (über den Hurri gesetzt damit er von dort runterfallen kann)
    case GEGNER_NOTVISIBLE:
    {
        Handlung = GEGNER_FALLEN;
        yPos -= 480.0f + 160.0f;
        if (yPos < 0.0f)
            yPos = 0.0f;
    }
    break;

    // Sack fällt runter
    case GEGNER_FALLEN:
    {
        if (ySpeed > 80.0f + g_Fahrstuhl_Speed)
        {
            ySpeed = 80.0f + g_Fahrstuhl_Speed;
            yAcc   =  0.0f;
        }

        if (yPos + 147 > g_Fahrstuhl_yPos)
        {
            ySpeed    = 0.0f;
            yPos      = g_Fahrstuhl_yPos - 147;
            Handlung  = GEGNER_STEHEN;
            AnimEnde  = 10;
            AnimStart = 9;
            AnimPhase = 2;
            AnimSpeed = 0.75f;
        }
    }
    break;

    case GEGNER_STEHEN:			// Aufkommen und abfedern
    {
        AnimCount += SpeedFaktor;			// Animationscounter weiterzählen
        if (AnimCount > AnimSpeed)			// Grenze überschritten ?
        {
            AnimCount = 0;					// Dann wieder auf Null setzen
            AnimPhase++;					// Und nächste Animationsphase
            if (AnimPhase >= 9)				// Animation von zu Ende	?
            {
                AnimPhase = 9;
                AnimEnde  = 0;
                AnimCount = 0.0f;
                Handlung = GEGNER_LAUFEN;
            }
        }
    }
    break;

    case GEGNER_LAUFEN:				// rumstehen und ballern oder drehen
    {
        // Schiessen ?
        AnimCount += 1.0f SYNC;

        if (AnimCount >= 12.0f)
        {
            AnimCount = 0.0f;

            SoundManager.PlayWave (100, 128, 18000 + rand ()%4000, SOUND_LASERSHOT);

            if (BlickRichtung == LINKS)
            {
                PartikelSystem.PushPartikel (xPos - 50, yPos, STELZFLARE);
                Projectiles.PushProjectile  (xPos - 30, yPos + 25, STELZLASER);
            }
            else
            {
                PartikelSystem.PushPartikel (xPos + 40, yPos, STELZFLARE);
                Projectiles.PushProjectile  (xPos + 50, yPos + 25, STELZLASER2);
            }

        }

        // Rumdrehen ?
        if ((BlickRichtung == LINKS  && xPos + 50 < pAim->xpos + 35) ||
                (BlickRichtung == RECHTS && xPos + 50 > pAim->xpos + 35))
        {
            Handlung  = GEGNER_DREHEN;
            AnimCount = 0.0f;
        }
    }
    break;

    case GEGNER_DREHEN:				// rumdrehen
    {
        AnimCount += SpeedFaktor;			// Animationscounter weiterzählen
        if (AnimCount > AnimSpeed)			// Grenze überschritten ?
        {
            AnimCount = 0.0f;				// Dann wieder auf Null setzen
            AnimPhase++;					// Und nächste Animationsphase
            if (AnimPhase >= 15)			// Animation von zu Ende	?
            {
                AnimPhase = 13;
                AnimCount = 0.0f;
                Handlung = GEGNER_DREHEN2;
                BlickRichtung *= -1;
            }
        }
    }
    break;

    case GEGNER_DREHEN2:			// von der Mitte aus fertig rumdrehen
    {
        AnimCount += SpeedFaktor;			// Animationscounter weiterzählen
        if (AnimCount > AnimSpeed)			// Grenze überschritten ?
        {
            AnimCount = 0.0f;				// Dann wieder auf Null setzen
            AnimPhase--;					// Und nächste Animationsphase
            if (AnimPhase <= 9)				// Animation von zu Ende	?
            {
                AnimPhase = 9;
                AnimCount = 0.0f;
                Handlung = GEGNER_LAUFEN;
            }
        }
    }
    break;

    default :
        break;
    } // switch

    // Testen, ob der Spieler den Stelzsack berührt hat
    TestDamagePlayers(5.0f SYNC);
}

// --------------------------------------------------------------------------------------
// StelzSack explodiert
// --------------------------------------------------------------------------------------

void GegnerStelzSack::GegnerExplode(void)
{
    PartikelSystem.PushPartikel (xPos, yPos, STELZHEAD);

    PartikelSystem.PushPartikel (xPos-20, yPos-10, STELZE);
    PartikelSystem.PushPartikel (xPos+20, yPos-20, STELZE);

    SoundManager.PlayWave (100, 128, 8000 + rand()%4000, SOUND_EXPLOSION4);

    Player[0].Score += 500;		// Punkte geben
}
