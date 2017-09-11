// --------------------------------------------------------------------------------------
// Diamant
//
// Bringt Punkte beim Einsammeln und aber einer gewissen Anzahl ein Extra Leben
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Gegner_Diamant.h"
#include "Gegner_Punisher.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerDiamant::GegnerDiamant(int Wert1, int Wert2, bool Light)
{
    Handlung	= GEGNER_STEHEN;
    Energy	    = 1.0f;
    Value1	    = Wert1;
    Value2	    = Wert2;
    AnimPhase	= 0;
    AnimStart   = 0;
    AnimEnde    = 9;
    AnimSpeed   = 0.75f;
    ChangeLight = Light;
    Destroyable = false;
    //Active		= true;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerDiamant::DoKI(void)
{
    BlickRichtung = LINKS;

    // Bei Animationsphase 1 kurze Pause damit er nich blinkt wie ein Feuerwehrauto =)
    if (AnimPhase == 0)
        AnimSpeed = 4.0f;
    else
        AnimSpeed = 1.0f;

    // Animieren
    AnimCount += 1.0f SYNC;				// Animationscounter weiterzählen
    if (AnimCount > AnimSpeed)			// Grenze überschritten ?
    {
        AnimCount = 0;					// Dann wieder auf Null setzen
        AnimPhase++;					// Und nächste Animationsphase
        if (AnimPhase >= AnimEnde)		// Animation von zu Ende	?
            AnimPhase = 0;				// Dann wieder von vorne beginnen
    }

    switch(Handlung)
    {
    case GEGNER_FALLEN :			// Diamant fällt runter
    {
        if (ySpeed > 35.0f)			// Fall-Limit nicht überschreiten
            ySpeed = 35.0f;

        // Am Boden abhopfen ?
        if (blocku & BLOCKWERT_WAND ||
                blocku & BLOCKWERT_PLATTFORM)
        {
            //blocku = TileEngine.BlockUnten (xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt]);

            if (ySpeed > 0.0f)
            {
                ySpeed = -ySpeed / 2.0f;

                if (ySpeed > -0.5f)				// Wieder abspringen oder liegenbleiben ?
                {
                    ySpeed   = 0.0f;
                    yAcc     = 0.0f;
                    Handlung = GEGNER_STEHEN;
                    yPos    += 1.0f;
                }
            }
        }
    }
    break;

    case GEGNER_STEHEN :
    {
        // Value1 == 0? Dann runterfallen lassen
        //
        if (Value1 == 0)
            if (!(TileEngine.BlockUnten (xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt]) & BLOCKWERT_WAND) &&
                    !(TileEngine.BlockUnten (xPos, yPos, xPosOld, yPosOld, GegnerRect[GegnerArt]) & BLOCKWERT_PLATTFORM))
            {
                yAcc	 = 2.0f;
                Handlung = GEGNER_FALLEN;
            }

    }
    break;

    } // switch

    // Testen, ob der Spieler den Diamant eingesammelt hat
    for (int i = 0; i < NUMPLAYERS; i++)
        if (SpriteCollision(xPos, yPos, GegnerRect[GegnerArt],
                            Player[i].xpos, Player[i].ypos, Player[i].CollideRect) == true)
        {
            // Partikel erzeugen
            if (Player[0].PunisherActive == false)
                PartikelSystem.PushPartikel(xPos, yPos, DIAMANTCOLLECTED);

            // Punisher anschiessen
            else
            {
                Projectiles.PushProjectile(xPos, yPos, DIAMONDSHOT);
            }

            Player[0].Score += 100;					// Punkte erhöhen
            Player[0].CollectedDiamonds++;			// Zahl gesammelter Diamanten erhöhen
            Player[0].DiamondsThisLevel++;
            Player[0].DiamondsFullGame++;
            if (Player[0].CollectedDiamonds			// Limit überchritten ?
                    >= DIAMONDS_FOR_EXTRALIFE)
            {
                Player[0].CollectedDiamonds = 0;		// Dann auf 0 setzen
                TileEngine.Timelimit += 100.0f;

                if (TileEngine.Timelimit > 999.0f)
                    TileEngine.Timelimit = 999.0f;

                // Punisher ggf verschwinden lassen
                Player[0].PunisherActive = false;
                Player[1].PunisherActive = false;

                GegnerClass *pTemp;
                GegnerPunisher *pPunisher;

                pTemp = Gegner.pStart;

                while (pTemp != NULL)
                {
                    if (pTemp->GegnerArt == PUNISHER)
                    {
                        pPunisher = (GegnerPunisher*)pTemp;
                        pPunisher->Vanish();
                    }

                    pTemp = pTemp->pNext;
                }
            }
            Energy = 0.0f;
        }
}

// --------------------------------------------------------------------------------------
// Diamant verschwindet
// --------------------------------------------------------------------------------------

void GegnerDiamant::GegnerExplode(void)
{
    SoundManager.PlayWave(100, 128, 11025, SOUND_COLLECT);	// Sound ausgeben
}
