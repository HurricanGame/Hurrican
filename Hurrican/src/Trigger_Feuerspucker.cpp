// --------------------------------------------------------------------------------------
// Der Flammenwerfer an der Wand
//
// Erzeugt die Feuerfalle
// Value1 gibt die Richtung an, Value2 die Länge bis zur Feuerpause
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Trigger_Feuerspucker.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerFeuerspucker::GegnerFeuerspucker(int Wert1, int Wert2, bool Light)
{
    Handlung		= GEGNER_STEHEN;
    BlickRichtung	= LINKS;
    Energy			= 10000;
    Value1			= Wert1;
    Value2			= Wert2;
    AnimPhase		= Value1;
    ChangeLight		= Light;
    Destroyable		= false;
    ShotDelay		= 1.0f;
    TestBlock		= false;
    Active			= true;
}

// --------------------------------------------------------------------------------------
// "Bewegungs KI"
// --------------------------------------------------------------------------------------

void GegnerFeuerspucker::DoKI(void)
{

// Je nach Handlung richtig verhalten
    switch (Handlung)
    {
    // Pausieren
    case GEGNER_STEHEN:
    {
        AnimCount += 1.0f SYNC;

        // Wieder losballern ?
        if (AnimCount >= 20.0f)
        {
            AnimCount = 0.0f;
            Handlung = GEGNER_SCHIESSEN;

            // Sound abspielen, je nach Player Abstand lauter oder leiser
            SoundManager.PlayWave3D(int(xPos+20), int(yPos+20), 11025, SOUND_FEUERFALLE);
        }
    }
    break;

    // Feuer spucken
    case GEGNER_SCHIESSEN:
    {
        AnimCount += 1.0f SYNC;

        // Pausieren ?
        if (AnimCount >= Value2)
        {
            AnimCount = 0.0f;
            Handlung = GEGNER_STEHEN;
        }

        if (PlayerAbstand() < 800)
        {

            // Flamme erzeugen (je nach Richtung)
            ShotDelay -= 1.0f SYNC;

            if (ShotDelay <= 0.0f)
            {
                ShotDelay = 0.3f;
                switch (Value1)
                {
                case 0  :
                    Projectiles.PushProjectile(xPos, yPos, FEUERFALLE);
                    break;
                case 1  :
                    Projectiles.PushProjectile(xPos, yPos, FEUERFALLE2);
                    break;
                case 2  :
                    Projectiles.PushProjectile(xPos, yPos, FEUERFALLE3);
                    break;
                case 3  :
                    Projectiles.PushProjectile(xPos, yPos, FEUERFALLE4);
                    break;
                default :
                    break;
                } // switch
            }
        }
    }
    break;

    default :
        break;
    } // switch
}

// --------------------------------------------------------------------------------------
// Feuerspucker explodiert
// --------------------------------------------------------------------------------------

void GegnerFeuerspucker::GegnerExplode(void)
{
    SoundManager.PlayWave(25, 128, 11025, SOUND_EXPLOSION1);

    for (int i=0; i<5; i++)
        PartikelSystem.PushPartikel(xPos-50+rand()%48, yPos-50+rand()%56, EXPLOSION_BIG);

    for (int i=0; i<8; i++)
        PartikelSystem.PushPartikel(xPos-30+rand()%48, yPos-30+rand()%56, EXPLOSION_MEDIUM);
}
