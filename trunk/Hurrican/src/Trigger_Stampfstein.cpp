// --------------------------------------------------------------------------------------
// Die Stampfstein
//
// F�llt runter, sobald der Hurri in Reichweite steht
// wird danach wieder hochgezogen
// --------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Trigger_Stampfstein.h"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerStampfstein::GegnerStampfstein(int Wert1, int Wert2, bool Light)
{
    Handlung		= GEGNER_NOTVISIBLE;
    BlickRichtung	= LINKS;
    Energy			= 5000.0f;
    Value1			= Wert1;
    Value2			= Wert2;
    ChangeLight		= Light;
    Destroyable		= true;
    OwnDraw			= true;
    oldy			= 0.0f;

    //DKS - The chain retracting sound was disabled in the original code, but I've
    //      added it back in. In order to allow this, we must keep track of which
    //      channel this particular stampfstein's chain sound is playing on, so that
    //      multiple stampfsteins can play and the correct one will be stopped when
    //      fully retracted. (-1 means it is not playing)
    sfx_chain_channel = -1;
}

// --------------------------------------------------------------------------------------
// Rendern
// --------------------------------------------------------------------------------------

void GegnerStampfstein::DoDraw(void)
{
    // rendern
    //
    DirectGraphics.SetColorKeyMode();
    int size = (int)(oldy - yPos) + 165;
    pGegnerGrafix[GegnerArt]->SetRect(0, size, 80, 256);
    pGegnerGrafix[GegnerArt]->RenderSprite((float)(xPos-pTileEngine->XOffset),
                                           (float)(yPos-pTileEngine->YOffset) + size, 0xFFFFFFFF);
}

// --------------------------------------------------------------------------------------
// Bewegungs KI
// --------------------------------------------------------------------------------------

void GegnerStampfstein::DoKI(void)
{
    Energy = 5000.0f;

    // Spieler kann seitlich nicht durchlaufen
    //
    Wegschieben(GegnerRect[GegnerArt], 0.0f);

    // Spieler kann von unten nicht durchspringen
    //
    for (int p = 0; p < NUMPLAYERS; p++)
        if (SpriteCollision(pPlayer[p]->xpos,
                            pPlayer[p]->ypos,
                            pPlayer[p]->CollideRect,
                            xPos, yPos, GegnerRect[GegnerArt]) == true &&
                yPos < pPlayer[p]->ypos					   &&
                pPlayer[p]->yspeed < 0.0f)
            pPlayer[p]->yspeed *= -0.25f;

    // Ja nach Handlung richtig verhalten
    switch (Handlung)
    {
    // warten, bis der Hurri vorbeikommt ;)
    //
    case GEGNER_NOTVISIBLE:
    {
        oldy = yPos;
        Handlung = STEHEN;
    }
    break;

    case STEHEN:
    {
        oldy = yPos;

        // Spieler in der N�he? Dann runterfallen lassen
        //
        for (int p = 0; p < NUMPLAYERS; p++)
        {
            pAim = pPlayer[p];

            if (PlayerAbstandHoriz() < 50 &&
                    PlayerAbstandVert()  < 600 &&
                    pPlayer[p]->ypos > yPos)
            {
                Handlung = GEGNER_FALLEN;
                ySpeed   = 20.0f;
                yAcc     = 15.0f;

                SoundManager.PlayWave(100, 128, 8000 + rand()%4000, SOUND_STONEFALL);
            }
        }
    }
    break;


    // Stein f�llt gerade runter
    //
    case GEGNER_FALLEN :
    {
        // Fallgeschwindigkeit begrenzen
        //
        if (ySpeed > 40.0f)
            ySpeed = 40.0f;

        // Spieler beim Runterfallen ber�hrt? Dann stirbt er leider ;)
        //
        for (int p = 0; p < NUMPLAYERS; p++)
        {
            if (pPlayer[p]->AufPlattform != this &&
                    SpriteCollision(pPlayer[p]->xpos, pPlayer[p]->ypos, pPlayer[p]->CollideRect,
                                    xPos, yPos, GegnerRect[GegnerArt]) == true &&
                    pPlayer[p]->ypos + pPlayer[p]->CollideRect.bottom >= yPos)
                TestDamagePlayers(500.0f);
        }

        // Am Boden ? Dann aufstampfen und wieder nach oben fahren
        if (blocku & BLOCKWERT_WAND		 ||
                blocku & BLOCKWERT_PLATTFORM ||
                blocku & BLOCKWERT_WAND)
        {
            // Staub erzeugen
            for (int i=0; i < 20; i++)
                pPartikelSystem->PushPartikel (xPos + i * 5 - 30, yPos + 220, SMOKEBIG);

            // Tr�mmer erzeugen
            for (int i=0; i < 20; i++)
                pPartikelSystem->PushPartikel (xPos + i * 5 - 10, yPos + 236, ROCKSPLITTER + rand()%2);

            SoundManager.PlayWave(175, 128, 8000 + rand()%4000, SOUND_PHARAORAMM);

            ySpeed = 0.0f;
            yAcc   = 0.0f;
            AnimCount = 20.0f;
            Handlung = GEGNER_INIT;

            // Checken, ob Gegner darunter waren
            GegnerClass *pTemp;

            pTemp = pGegner->pStart;

            while (pTemp != NULL)
            {
                if (pTemp->Destroyable == true &&
                        pTemp != this &&
                        SpriteCollision(xPos, yPos, GegnerRect[GegnerArt],
                                        pTemp->xPos, pTemp->yPos, GegnerRect[pTemp->GegnerArt]) == true)
                    pTemp->Energy = 0.0f;

                pTemp = pTemp->pNext;
            }
        }

    }
    break;

    // Stein wartet kurz und wird dann wieder hochgezogen
    case GEGNER_INIT:
    {
        PlattformTest(GegnerRect[GegnerArt]);

        AnimCount -= 1.0f SYNC;
        if (AnimCount < 0.0f)
        {
            ySpeed   = -4.0f;
            Handlung = GEGNER_SPRINGEN;

            // Hochzieh Sound
//				if (SoundManager.its_Sounds[SOUND_CHAIN]->isPlaying == false)
//					SoundManager.PlayWave(100, 128, 11000 + rand()%50, SOUND_CHAIN);

            //DKS - Chain retracting sound was commented out (above) in the original code,
            //      but I've added support for it. Since multiple trigger stampfsteins
            //      (falling blocks on chains) can be retracting at once, each one must
            //      keep track of which channel its chain sound is playing on, so when it
            //      is fully retracted, the correct sound channel is halted:
            sfx_chain_channel = SoundManager.PlayWave3D((int)xPos + 40, (int)yPos + 20, 11025, SOUND_CHAIN);
        }
    }
    break;

    // Stein wird hochgezogen
    //
    case GEGNER_SPRINGEN:
    {
        PlattformTest(GegnerRect[GegnerArt]);

//			SoundManager.Update3D((int)(xPos + 40), (int)(yPos + 20), SOUND_CHAIN);

        if (blocko & BLOCKWERT_WAND)
        {
            Handlung = GEGNER_STEHEN;
            ySpeed   = 0.0f;

//				SoundManager.StopWave(SOUND_CHAIN);

            //DKS - Added support for chain retracting sound back into original source:
            if (sfx_chain_channel != -1 && SoundManager.WaveIsPlayingOnChannel(SOUND_CHAIN, sfx_chain_channel)) {
                SoundManager.StopChannel(sfx_chain_channel);
                sfx_chain_channel = -1;
            }
        }

        // Spieler zwischen Decke und Stein eingeklemmt?
        //
        for (int p = 0; p < NUMPLAYERS; p++)
            if (pPlayer[p]->AufPlattform == this &&
                    pTileEngine->BlockOben(pPlayer[p]->xpos, pPlayer[p]->ypos,
                                           pPlayer[p]->xposold, pPlayer[p]->yposold,
                                           pPlayer[p]->CollideRect) & BLOCKWERT_WAND)
            {
                pPlayer[p]->DamagePlayer(500.0f, true);
                pPlayer[p]->AufPlattform = NULL;
                pPlayer[p]->ypos += 10.0f;
                pPlayer[p]->yspeed = 10.0f;
            }
    }
    break;

    } // switch

    // Spieler hat Stein von unten ber�hrt (an den Stacheln) ?
    //
    for (int p = 0; p < NUMPLAYERS; p++)
        if (SpriteCollision(pPlayer[p]->xpos,
                            pPlayer[p]->ypos,
                            pPlayer[p]->CollideRect,
                            xPos, yPos, GegnerRect[GegnerArt]) == true &&
                pPlayer[p]->ypos > yPos)
            pPlayer[p]->DamagePlayer(20.0f SYNC);
}

// --------------------------------------------------------------------------------------
// Stampfstein explodiert (geht ja garnich *g*)
// --------------------------------------------------------------------------------------

void GegnerStampfstein::GegnerExplode(void)
{
}
