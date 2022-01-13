// --------------------------------------------------------------------------------------
// Die Stampfstein
//
// Fällt runter, sobald der Hurri in Reichweite steht
// wird danach wieder hochgezogen
// --------------------------------------------------------------------------------------

#include "Trigger_Stampfstein.hpp"
#include "stdafx.hpp"

// --------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------

GegnerStampfstein::GegnerStampfstein(int Wert1, int Wert2, bool Light) {
    Handlung = GEGNER::NOTVISIBLE;
    BlickRichtung = DirectionEnum::LINKS;
    Energy = 5000.0f;
    Value1 = Wert1;
    Value2 = Wert2;
    ChangeLight = Light;
    Destroyable = true;
    OwnDraw = true;
    oldy = 0.0f;

    // DKS - The chain retracting sound was disabled in the original code, but I've
    //      added it back in. In order to allow this, we must keep track of which
    //      channel this particular stampfstein's chain sound is playing on, so that
    //      multiple stampfsteins can play and the correct one will be stopped when
    //      fully retracted. (-1 means it is not playing)
    sfx_chain_channel = -1;
}

// --------------------------------------------------------------------------------------
// Rendern
// --------------------------------------------------------------------------------------

void GegnerStampfstein::DoDraw() {
    // rendern
    //
    DirectGraphics.SetColorKeyMode();
    int size = static_cast<int>(oldy - yPos) + 165;
    pGegnerGrafix[GegnerArt]->SetRect(0, size, 80, 256);
    pGegnerGrafix[GegnerArt]->RenderSprite(xPos - TileEngine.XOffset,
                                           yPos - TileEngine.YOffset + size, 0xFFFFFFFF);
}

// --------------------------------------------------------------------------------------
// Bewegungs KI
// --------------------------------------------------------------------------------------

void GegnerStampfstein::DoKI() {
    Energy = 5000.0f;

    // Spieler kann seitlich nicht durchlaufen
    //
    Wegschieben(GegnerRect[GegnerArt], 0.0f);

    // Spieler kann von unten nicht durchspringen
    //
    for (int p = 0; p < NUMPLAYERS; p++)
        if (SpriteCollision(Player[p].xpos, Player[p].ypos, Player[p].CollideRect, xPos, yPos, GegnerRect[GegnerArt]) ==
                true &&
            yPos < Player[p].ypos && Player[p].yspeed < 0.0f)
            Player[p].yspeed *= -0.25f;

    // Ja nach Handlung richtig verhalten
    switch (Handlung) {
        // warten, bis der Hurri vorbeikommt ;)
        //
        case GEGNER::NOTVISIBLE: {
            oldy = yPos;
            Handlung = GEGNER::STEHEN;
        } break;

        case GEGNER::STEHEN: {
            oldy = yPos;

            // Spieler in der Nähe? Dann runterfallen lassen
            //
            for (int p = 0; p < NUMPLAYERS; p++) {
                pAim = &Player[p];

                if (PlayerAbstandHoriz() < 50 && PlayerAbstandVert() < 600 && Player[p].ypos > yPos) {
                    Handlung = GEGNER::FALLEN;
                    ySpeed = 20.0f;
                    yAcc = 15.0f;

                    SoundManager.PlayWave(100, 128, 8000 + random(4000), SOUND::STONEFALL);
                }
            }
        } break;

        // Stein fällt gerade runter
        //
        case GEGNER::FALLEN: {
            // Fallgeschwindigkeit begrenzen
            //
            if (ySpeed > 40.0f)
                ySpeed = 40.0f;

            // Spieler beim Runterfallen berührt? Dann stirbt er leider ;)
            //
            for (int p = 0; p < NUMPLAYERS; p++) {
                if (Player[p].AufPlattform != this &&
                    SpriteCollision(Player[p].xpos, Player[p].ypos, Player[p].CollideRect, xPos, yPos,
                                    GegnerRect[GegnerArt]) == true &&
                    Player[p].ypos + Player[p].CollideRect.bottom >= yPos)
                    TestDamagePlayers(500.0f);
            }

            // Am Boden ? Dann aufstampfen und wieder nach oben fahren
            if (blocku & BLOCKWERT_WAND || blocku & BLOCKWERT_PLATTFORM || blocku & BLOCKWERT_WAND) {
                // Staub erzeugen
                for (int i = 0; i < 20; i++)
                    PartikelSystem.PushPartikel(xPos + i * 5 - 30, yPos + 220, SMOKEBIG);

                // Trümmer erzeugen
                for (int i = 0; i < 20; i++)
                    PartikelSystem.PushPartikel(xPos + static_cast<float>(i * 5 - 10),
                                                yPos + static_cast<float>(236), ROCKSPLITTER + random(2));

                SoundManager.PlayWave(175, 128, 8000 + random(4000), SOUND::PHARAORAMM);

                ySpeed = 0.0f;
                yAcc = 0.0f;
                AnimCount = 20.0f;
                Handlung = GEGNER::INIT;

                // Checken, ob Gegner darunter waren
                GegnerClass *pTemp;

                pTemp = Gegner.pStart;

                while (pTemp != nullptr) {
                    if (pTemp->Destroyable == true && pTemp != this &&
                        SpriteCollision(xPos, yPos, GegnerRect[GegnerArt], pTemp->xPos, pTemp->yPos,
                                        GegnerRect[pTemp->GegnerArt]) == true)
                        pTemp->Energy = 0.0f;

                    pTemp = pTemp->pNext;
                }
            }

        } break;

        // Stein wartet kurz und wird dann wieder hochgezogen
        case GEGNER::INIT: {
            PlattformTest(GegnerRect[GegnerArt]);

            AnimCount -= Timer.sync(1.0f);
            if (AnimCount < 0.0f) {
                ySpeed = -4.0f;
                Handlung = GEGNER::SPRINGEN;

                // Hochzieh Sound
                //				if (SoundManager.its_Sounds[SOUND::CHAIN]->isPlaying == false)
                //					SoundManager.PlayWave(100, 128, 11000 + rand()%50, SOUND::CHAIN);

                // DKS - Chain retracting sound was commented out (above) in the original code,
                //      but I've added support for it. Since multiple trigger stampfsteins
                //      (falling blocks on chains) can be retracting at once, each one must
                //      keep track of which channel its chain sound is playing on, so when it
                //      is fully retracted, the correct sound channel is halted:
                sfx_chain_channel = SoundManager.PlayWave3D(static_cast<int>(xPos) + 40,
                                                            static_cast<int>(yPos) + 20,
                                                            11025, SOUND::CHAIN);
            }
        } break;

        // Stein wird hochgezogen
        //
        case GEGNER::SPRINGEN: {
            PlattformTest(GegnerRect[GegnerArt]);

            //			SoundManager.Update3D(static_cast<int>(xPos + 40), static_cast<int>(yPos + 20), SOUND::CHAIN);

            if (blocko & BLOCKWERT_WAND) {
                Handlung = GEGNER::STEHEN;
                ySpeed = 0.0f;

                //				SoundManager.StopWave(SOUND::CHAIN);

                // DKS - Added support for chain retracting sound back into original source:
                if (sfx_chain_channel != -1 && SoundManager.WaveIsPlayingOnChannel(SOUND::CHAIN, sfx_chain_channel)) {
                    SoundManager.StopChannel(sfx_chain_channel);
                    sfx_chain_channel = -1;
                }
            }

            // Spieler zwischen Decke und Stein eingeklemmt?
            //
            for (int p = 0; p < NUMPLAYERS; p++)
                if (Player[p].AufPlattform == this &&
                    TileEngine.BlockOben(Player[p].xpos, Player[p].ypos, Player[p].xposold, Player[p].yposold,
                                         Player[p].CollideRect) &
                        BLOCKWERT_WAND) {
                    Player[p].DamagePlayer(500.0f, true);
                    Player[p].AufPlattform = nullptr;
                    Player[p].ypos += 10.0f;
                    Player[p].yspeed = 10.0f;
                }
        } break;

    }  // switch

    // Spieler hat Stein von unten berührt (an den Stacheln) ?
    //
    for (int p = 0; p < NUMPLAYERS; p++)
        if (SpriteCollision(Player[p].xpos, Player[p].ypos, Player[p].CollideRect, xPos, yPos, GegnerRect[GegnerArt]) ==
                true &&
            Player[p].ypos > yPos)
            Player[p].DamagePlayer(Timer.sync(20.0f));
}

// --------------------------------------------------------------------------------------
// Stampfstein explodiert (geht ja garnich *g*)
// --------------------------------------------------------------------------------------

void GegnerStampfstein::GegnerExplode() {}
