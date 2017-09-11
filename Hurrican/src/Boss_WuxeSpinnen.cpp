// --------------------------------------------------------------------------------------
// Der Wuxe Spinnenansturm im SpinnenLevel
//
// Spinnen krabbeln von der Decke
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Boss_WuxeSpinnen.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerWuxeSpinnen::GegnerWuxeSpinnen(int Wert1, int Wert2, bool Light)
{
    Handlung		= GEGNER_VERFOLGEN;
    BlickRichtung	= LINKS;
    Energy			= 2000;
    ChangeLight		= Light;
    Destroyable		= false;
    Value1			= Wert1;
    Value2			= Wert2;
    ActionCounter	= 1.0f;
    DontMove		= true;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerWuxeSpinnen::DoKI(void)
{
    // Energie anzeigen
    if (Handlung != GEGNER_INIT		 &&
            Handlung != GEGNER_VERFOLGEN &&
            Handlung != GEGNER_EXPLODIEREN)
        HUD.ShowBossHUD(2000, Energy);

    // Boss aktivieren und Mucke laufen lassen
    //
    if (Active == true				 &&
            Handlung != GEGNER_VERFOLGEN &&
            TileEngine.Zustand == ZUSTAND_SCROLLBAR)
    {
        TileEngine.ScrollLevel((float)Value1,
                                 (float)Value2, ZUSTAND_SCROLLTOLOCK);		// Level auf den Boss zentrieren

        SoundManager.FadeSong(MUSIC_STAGEMUSIC, -2.0f, 0, true);  // Ausfaden und pausieren
        Gegner.PushGegner (xPos, yPos - 40, EXTRAS, 10, 0, false);
    }

    // Hat der Boss keine Energie mehr ? Dann explodiert er
    if (Energy <= 100.0f && Handlung != GEGNER_EXPLODIEREN)
    {
        Handlung   = GEGNER_EXPLODIEREN;

        // Endboss-Musik ausfaden und abschalten
        SoundManager.FadeSong(MUSIC_BOSS, -2.0f, 0, false);
    }

// Je nach Handlung richtig verhalten
    switch (Handlung)
    {
    case GEGNER_INIT:			// Warten bis der Screen zentriert wurde
    {
        if (TileEngine.Zustand == ZUSTAND_LOCKED)
        {
            // Zwischenboss-Musik abspielen, sofern diese noch nicht gespielt wird
            //
            //DKS - Added function SongIsPlaying() to SoundManagerClass:
            if (!SoundManager.SongIsPlaying(MUSIC_BOSS))
            {
                SoundManager.PlaySong(MUSIC_BOSS, false);

                // Und Boss erscheinen lassen
                //
                Handlung = GEGNER_STEHEN;
            }
        }
    }
    break;

    case GEGNER_STEHEN:
    {
        // ActionCounter runterzählen
        // bei null Gegner spawnen
        ActionCounter -= 0.8f SYNC;

        if (ActionCounter <= 0.0f)
        {
            ActionCounter = 1.5f + Energy / 500.0f;

            // und spawnen
            //
            if (Energy > 500.0f)
            {
                float x = (float)(rand()%520);

                Gegner.PushGegner (float (Value1) + x, float (Value2) - 100, MITTELSPINNE, 0, 1, false);
            }

            Energy -= 20;

            // ab und zu ein Paar Augen einstreuen
            //
            if (int (Energy) % 1000 == 0)
            {
                for (int i = 0; i < 2; i++)
                {
                    Gegner.PushGegner (float (Value1) - 20,  float (Value2) + i * 60 + 150, AUGE, rand()%10, 0, false);
                    Gegner.PushGegner (float (Value1) + 620, float (Value2) + i * 60 + 180, AUGE, rand()%10, 0, false);
                }
            }

            // oder AutoFire ;)
            //
            if (int (Energy+500) % 600 == 0)
            {
                Gegner.PushGegner (xPos, yPos - 40, EXTRAS, 10, 0, false);
            }
        }
    }
    break;

    // warten, bis der Spieler den Gegner berührt hat, und dann aktivieren
    //
    case GEGNER_VERFOLGEN:
    {
        if (PlayerAbstand() < 400)
            Handlung = GEGNER_INIT;

    }
    break;

    case GEGNER_EXPLODIEREN:
    {
        Energy = 0.0f;
    }
    break;

    default :
        break;
    } // switch

}

// --------------------------------------------------------------------------------------
// WuxeSpinnen explodiert (nicht)
// --------------------------------------------------------------------------------------

void GegnerWuxeSpinnen::GegnerExplode(void)
{
    // Extra Leben
    Gegner.PushGegner (xPos, yPos - 250, ONEUP, 10, 0, false);

    ScrolltoPlayeAfterBoss();
}
